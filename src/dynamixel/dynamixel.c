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
    dynamixel_t self
)
{
    // GPIOピンの設定を解除する
    gpio_set_function(self->gpio_uart_rx, GPIO_FUNC_NULL);
    gpio_set_function(self->gpio_uart_tx, GPIO_FUNC_NULL);
    if (gpio_use_count[self->gpio_uart_rx] > 0)
        gpio_use_count[self->gpio_uart_rx]--;
    if (gpio_use_count[self->gpio_uart_tx] > 0)
        gpio_use_count[self->gpio_uart_tx]--;

    // UARTの設定を解除する
    pico_uart_deinit(self->uart_id);
    if (uart_use_count > 0)
    {
        for (size_t i = 0; i < uart_use_count; i++)
        {
            if (self->uart_id == uart_use[i])
            {
                uart_use[i] = NULL;
                uart_use_count--;
                break;
            }
        }

    }

    free(self->read_buffer);
    free(self->write_buffer);
    free(self);
    self = NULL;
}


int dynamixel_write_uart_packet(
    dynamixel_t self,
    uint8_t id,
    uint8_t instruction,
    uint16_t parameter_size,
    const uint8_t *parameter
)
{
    size_t packet_size;

    // dynamixel_write_uart_packet
    packet_size = create_uart_packet(
        self->write_buffer,
        id, instruction, parameter, parameter_size
    );

    return pico_uart_write_blocking(
        self->uart_id, self->write_buffer, packet_size
    );
}


int dynamixel_partial_read_uart_packet(
    dynamixel_t self,
    size_t *initial_status_packet_size
)
{
    size_t status_packet_size = *initial_status_packet_size;

    for (
        size_t i = 0;
        i < self->read_size
        && status_packet_size < self->buffer_size;
        i++
    )
    {
        if(pico_uart_read_raw(
            self->uart_id,
            self->read_buffer + status_packet_size
        ))
            // 読み取りを行えなかった場合
            break;
        else
            status_packet_size++;
    }

    *initial_status_packet_size = status_packet_size;

    if (
        status_packet_size == self->buffer_size
        && !pico_uart_is_readable(self->uart_id)
    )
    {
        // readバッファーが満杯になったが、まだデータが残っている
        return 1;
    }

    return 0;
}


int dynamixel_partial_uart_packet(
    dynamixel_t self,
    uint8_t *error,
    size_t *status_parameter_size,
    uint8_t *status_parameter,
    size_t *initial_status_packet_size
)
{
    uint8_t status_id, status_instruction;
    int header_position, parse_result;
    size_t status_packet_size = *initial_status_packet_size;

    parse_result = parse_uart_packet(
        self->read_buffer, status_packet_size,
        &header_position, &status_id, &status_instruction,
        error, status_parameter, status_parameter_size
    );

    if (parse_result == 0 && *error > 0)
    {
        // 応答パケットがエラーだった
        return 2;
    }

    if (parse_result == 0 || parse_result == 1)
    {
        return parse_result;
    }

    // ステータスパケットが不完全な場合
    if (parse_result == 2)
    {
        // パケットの先頭がバッファーの先頭になるように移動する
        *initial_status_packet_size = status_packet_size - header_position;
        memmove(
            self->read_buffer,
            self->read_buffer + header_position,
            *initial_status_packet_size
        );
    }
    else if (parse_result == 3)
    {
        // パケットの先頭となりうる位置がバッファーの先頭になるように移動する
        // パケットの先頭は末尾から3つ前までの可能性がある(ヘッダーが見つからなかったため、3つより前は先頭となりえない)
        if (status_packet_size < 3)
        {
            memmove(
                self->read_buffer,
                self->read_buffer + status_packet_size,
                status_packet_size
            );
            *initial_status_packet_size = status_packet_size;
        }
        else
        {
            memmove(
                self->read_buffer,
                self->read_buffer + status_packet_size - 3,
                status_packet_size
            );
            *initial_status_packet_size = 3;
        }
    }

    return 3;
}


int dynamixel_partial_read_and_parse_uart_packet(
    dynamixel_t self,
    uint8_t *error,
    size_t *status_parameter_size,
    uint8_t *status_parameter,
    size_t *initial_status_packet_size
)
{
    if (dynamixel_partial_read_uart_packet(
        self, initial_status_packet_size
    ))
    {
        return 4;
    }

    return dynamixel_partial_uart_packet(
        self, error, status_parameter_size,
        status_parameter, initial_status_packet_size
    );
}


int dynamixel_read_uart_packet(
    dynamixel_t self,
    uint8_t *error,
    size_t *status_parameter_size,
    uint8_t *status_parameter
)
{
    int uart_read_result;
    size_t status_packet_size;

    if (!pico_uart_is_readable_within_us(
        self->uart_id, self->wait_us
    ))
    {
        // 読み込み前に初期化
        status_packet_size = 0;
        memset(self->read_buffer, 0, self->buffer_size);

        do
        {
            uart_read_result = dynamixel_partial_read_and_parse_uart_packet(
                self, error, status_parameter_size,
                status_parameter, &status_packet_size
            );

            if (
                uart_read_result == 0
                || uart_read_result == 1
                || uart_read_result == 2
                || uart_read_result == 4)
            {
                return uart_read_result;
            }
        }
        while (!pico_uart_is_readable(
            self->uart_id
        ));

        // 応答パケットが返って来たが不十分であった(最後のparse結果を見る)
        if (uart_read_result == 3)
        {
            return 3;
        }
    }

    // 応答が返ってこなかった
    return 5;
}


int dynamixel_send_packet(
    dynamixel_t self,
    uint8_t id,
    uint8_t instruction,
    uint16_t parameter_size,
    const uint8_t *parameter,
    uint8_t *error,
    size_t *status_parameter_size,
    uint8_t *status_parameter
)
{
    dynamixel_write_uart_packet(
        self,
        id, instruction, parameter_size, parameter
    );

    return dynamixel_read_uart_packet(
        self,
        error, status_parameter_size, status_parameter
    );
}
