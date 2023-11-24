#include <stdlib.h>
#include <string.h>
#include "hardware/gpio.h"
#include "dynamixel/dynamixel.h"
#include "util/analyze_packet.h"
#include "pico_communicator/pico_communicator.h"


// 同じGPIOピン、UARTインスタンスを使わないように記録する
#define MAX_GPIO_NUM 40
#define MAX_UART_NUM 10
static size_t gpio_use_count[MAX_GPIO_NUM] = {0};
static size_t uart_use_count = 0;
static uart_inst_t *uart_use[MAX_UART_NUM] = {NULL};

typedef struct dynamixel_struct
{
    uart_inst_t *uart_id;
    size_t buffer_size;
    size_t read_size;
    uint8_t *read_buffer;
    uint8_t *write_buffer;
    uint wait_us;
    uint gpio_uart_rx;
    uint gpio_uart_tx;
} dynamixel_struct;


dynamixel_t dynamixel_create(
    uart_inst_t *uart_id,
    uint gpio_uart_rx,
    uint gpio_uart_tx,
    uint baud_rate,
    size_t buffer_size,
    uint wait_us
)
{
    if (gpio_uart_rx > MAX_GPIO_NUM || gpio_uart_tx > MAX_GPIO_NUM)
        return NULL;
    if (uart_use_count > MAX_UART_NUM)
        return NULL;

    if (gpio_use_count[gpio_uart_rx] > 0 || gpio_use_count[gpio_uart_tx] > 0)
        return NULL;
    for (size_t i = 0; i < uart_use_count; i++)
    {
        if (uart_id == uart_use[i])
            return NULL;
    }


    dynamixel_t self = calloc(1, sizeof(dynamixel_struct));

    uint actual_baud_rate = pico_uart_init(
        uart_id, baud_rate,
        // Dynamixel2.0のフォーマット
        8, 1, UART_PARITY_NONE
    );

    pico_uart_set_fifo_enabled(uart_id, true);

    // GPIOピンをUART用に設定する
    gpio_set_function(gpio_uart_rx, GPIO_FUNC_UART);
    gpio_set_function(gpio_uart_tx, GPIO_FUNC_UART);
    gpio_use_count[gpio_uart_rx]++;
    gpio_use_count[gpio_uart_tx]++;
    uart_use[uart_use_count] = uart_id;
    uart_use_count++;

    self->uart_id = uart_id;
    self->gpio_uart_rx = gpio_uart_rx;
    self->gpio_uart_tx = gpio_uart_tx;
    self->buffer_size = buffer_size >= 20 ? buffer_size : 20;
    self->read_size = self->buffer_size / 2;
    self->wait_us = wait_us;

    self->read_buffer = (uint8_t *)calloc(
        self->buffer_size, sizeof(uint8_t)
    );
    self->write_buffer = (uint8_t *)calloc(
        self->buffer_size, sizeof(uint8_t)
    );

    return self;
}


void dynamixel_destroy(
    dynamixel_t dynamixel_id
)
{
    // GPIOピンの設定を解除する
    gpio_set_function(dynamixel_id->gpio_uart_rx, GPIO_FUNC_NULL);
    gpio_set_function(dynamixel_id->gpio_uart_tx, GPIO_FUNC_NULL);
    if (gpio_use_count[dynamixel_id->gpio_uart_rx] > 0)
        gpio_use_count[dynamixel_id->gpio_uart_rx]--;
    if (gpio_use_count[dynamixel_id->gpio_uart_tx] > 0)
        gpio_use_count[dynamixel_id->gpio_uart_tx]--;

    // UARTの設定を解除する
    pico_uart_deinit(dynamixel_id->uart_id);
    if (uart_use_count > 0)
    {
        for (size_t i = 0; i < uart_use_count; i++)
        {
            if (dynamixel_id->uart_id == uart_use[i])
            {
                uart_use[i] = NULL;
                uart_use_count--;
                break;
            }
        }

    }

    free(dynamixel_id->read_buffer);
    free(dynamixel_id->write_buffer);
    free(dynamixel_id);
    dynamixel_id = NULL;
}


int dynamixel_send_packet(
    dynamixel_t dynamixel_id,
    uint8_t id,
    uint8_t instruction,
    uint16_t parameter_size,
    const uint8_t *parameter,
    uint8_t *error,
    size_t *status_parameter_size,
    uint8_t *status_parameter
)
{
    size_t packet_size, status_packet_size;
    int header_position, parse_result;
    uint8_t status_id, status_instruction;

    packet_size = create_uart_packet(
        dynamixel_id->write_buffer,
        id, instruction, parameter, parameter_size
    );
    pico_uart_write_blocking(
        dynamixel_id->uart_id, dynamixel_id->write_buffer, packet_size
    );

    // 読み込み前に初期化
    status_packet_size = 0;
    memset(dynamixel_id->read_buffer, 0, dynamixel_id->buffer_size);

    if (!pico_uart_is_readable_within_us(
        dynamixel_id->uart_id, dynamixel_id->wait_us
    ))
    {
        do
        {
            for (
                size_t i = 0;
                i < dynamixel_id->read_size
                && status_packet_size < dynamixel_id->buffer_size;
                i++
            )
            {
                if(pico_uart_read_raw(
                    dynamixel_id->uart_id,
                    dynamixel_id->read_buffer + status_packet_size
                ))
                    // 読み取りを行えなかった場合
                    break;
                else
                    status_packet_size++;
            }

            if (
                status_packet_size == dynamixel_id->buffer_size
                && !pico_uart_is_readable(dynamixel_id->uart_id)
            )
            {
                // readバッファーが満杯になったが、まだデータが残っている
                return 4;
            }

            parse_result = parse_uart_packet(
                dynamixel_id->read_buffer, status_packet_size,
                &header_position, &status_id, &status_instruction,
                error, status_parameter, status_parameter_size
            );

            if (parse_result == 0 || parse_result == 1)
            {
                if (parse_result == 0 && *error > 0)
                {
                    // 応答パケットがエラーだった
                    return 2;
                }
                else
                {
                    // ステータスパケットを読み込めた(checksumが誤っている場合も含む)
                    return parse_result;
                }
            }
            else
            {
                // ステータスパケットが不完全な場合
                if (parse_result == 2)
                {
                    // パケットの先頭がバッファーの先頭になるように移動する
                    status_packet_size -= header_position;
                    memmove(
                        dynamixel_id->read_buffer,
                        dynamixel_id->read_buffer + header_position,
                        status_packet_size
                    );
                }
                else if (parse_result == 3)
                {
                    // パケットの先頭となりうる位置がバッファーの先頭になるように移動する
                    // パケットの先頭は末尾から3つ前までの可能性がある(ヘッダーが見つからなかったため、3つより前は先頭となりえない)
                    if (status_packet_size < 3)
                    {
                        memmove(
                            dynamixel_id->read_buffer,
                            dynamixel_id->read_buffer + status_packet_size,
                            status_packet_size
                        );
                    }
                    else
                    {
                        memmove(
                            dynamixel_id->read_buffer,
                            dynamixel_id->read_buffer + status_packet_size - 3,
                            status_packet_size
                        );
                        status_packet_size = 3;
                    }
                }
            }
        }
        while (!pico_uart_is_readable(
            dynamixel_id->uart_id
        ));

        // 応答パケットが返って来たが不十分であった(最後のparse結果を見る)
        if (parse_result == 2 || parse_result == 3)
        {
            return 3;
        }
    }

    // 応答が返ってこなかった
    return 5;
}
