#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "pico/time.h"
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

#define DYNAMIXEL_BAUD_RATE_NUM 7
static dynamixel_baud_rate DYNAMIXEL_BAUD_RATE_LIST[DYNAMIXEL_BAUD_RATE_NUM] = {
    DYNAMIXEL_BAUD_RATE_9600,
    DYNAMIXEL_BAUD_RATE_57600,
    DYNAMIXEL_BAUD_RATE_115200,
    DYNAMIXEL_BAUD_RATE_1M,
    DYNAMIXEL_BAUD_RATE_2M,
    DYNAMIXEL_BAUD_RATE_3M,
    DYNAMIXEL_BAUD_RATE_4M
};

/// iterative_countのデフォルト値を設定するマクロ
#define ITERATIVE_COUNT_DEFAULT(c) ((c) == 0 ? 5 : (c))


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
    dynamixel_baud_rate baud_rate;
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
    // check device
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
    dynamixel_baud_rate baud_rate_byte;
    if (get_baud_rate_byte(baud_rate, &baud_rate_byte) == 1)
        return NULL;

    dynamixel_t self = calloc(1, sizeof(dynamixel_struct));
    self->baud_rate = baud_rate_byte;

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


dynamixel_parse_result dynamixel_configure(
    dynamixel_t self,
    uint8_t id,
    uint8_t *error,
    uint16_t return_delay_time,
    dynamixel_operating_mode operating_mode,
    uint wait_us_multiplier,
    size_t iterative_count
)
{
    dynamixel_parse_result result;

    if (wait_us_multiplier == 0 && self->wait_us < 100000)
    {
        wait_us_multiplier = ceil(100000 / self->wait_us);
    }

    // TODO: util/pakcet_byte.hで渡された関数をボーレートの分だけループ処理する関数作ったほうが疎結合になる
    for (size_t i = 0; i < DYNAMIXEL_BAUD_RATE_NUM; i++)
    {
        dynamixel_baud_rate baud_rate_byte = DYNAMIXEL_BAUD_RATE_LIST[i];
        pico_uart_set_baudrate(self->uart_id, get_baud_rate(baud_rate_byte));

        result = dynamixel_send_ping(
            self, id, error, NULL, NULL, wait_us_multiplier
        );

        if (result == DYNAMIXEL_PARSE_SUCCESS)
        {
            result = dynamixel_send_write_torque_enable(
                self, id, error, false, wait_us_multiplier, iterative_count
            );
            if (result != DYNAMIXEL_PARSE_SUCCESS)
                break;

            // Dynamixelのボーレートに合わせた状態でボーレートを変える
            result = dynamixel_send_write_baud_rate(
                self, id, error, self->baud_rate, wait_us_multiplier, iterative_count
            );
            break;
        }
    }

    pico_uart_set_baudrate(self->uart_id, get_baud_rate(self->baud_rate));
    if (result != DYNAMIXEL_PARSE_SUCCESS)
        return result;

    result = dynamixel_send_write_return_delay_time(
        self, id, error, return_delay_time, wait_us_multiplier, iterative_count
    );

    if (result != DYNAMIXEL_PARSE_SUCCESS)
        return result;

    result = dynamixel_send_write_operating_mode(
        self, id, error, operating_mode, wait_us_multiplier, iterative_count
    );

    return result;
}


dynamixel_parse_result dynamixel_send_ping(
    dynamixel_t self,
    uint8_t id,
    uint8_t *error,
    uint16_t *dynamixel_model_no,
    uint8_t *dynamixel_version_of_firmware,
    uint wait_us_multiplier
)
{
    dynamixel_parse_result result;
    size_t parameter_size;
    uint8_t *parameter = (uint8_t *)calloc(self->buffer_size, sizeof(uint8_t));

    result = dynamixel_send_packet(
        self, id, DYNAMIXEL__INSTRUCTION_PING, 0, NULL,
        error, &parameter_size, parameter, wait_us_multiplier
    );

    if (result == DYNAMIXEL_PARSE_SUCCESS)
    {
        if (parameter_size == 3)
        {
            if (dynamixel_model_no)
                *dynamixel_model_no = combine_byte_pair(parameter[0], parameter[1]);
            if (dynamixel_version_of_firmware)
                *dynamixel_version_of_firmware = parameter[2];
        }
        else
        {
            result = DYNAMIXEL_PARSE_WRONG_PARAMETER;
        }
    }

    free(parameter);
    return result;
}


dynamixel_parse_result dynamixel_send_read_once(
    dynamixel_t self,
    uint8_t id,
    uint16_t start_address,
    uint16_t data_size,
    uint8_t *error,
    uint8_t *data,
    uint wait_us_multiplier
)
{
    dynamixel_parse_result result;
    size_t parameter_size, status_parameter_size;
    uint8_t parameter[4];

    // 開始アドレス
    divide_into_byte_pair(start_address, parameter, parameter + 1);
    // バイトサイズ
    divide_into_byte_pair(data_size, parameter + 2, parameter + 3);

    result = dynamixel_send_packet(
        self, id, DYNAMIXEL__INSTRUCTION_READ, 4, parameter,
        error, &status_parameter_size, data, wait_us_multiplier
    );

    // 応答パケットのパラメータサイズは、インストラクションで指定したデータサイズと等しい必要がある
    if (result == DYNAMIXEL_PARSE_SUCCESS && status_parameter_size != data_size)
        result = DYNAMIXEL_PARSE_WRONG_PARAMETER;

    return result;
}

dynamixel_parse_result dynamixel_send_read(
    dynamixel_t self,
    uint8_t id,
    uint16_t start_address,
    uint16_t data_size,
    uint8_t *error,
    uint8_t *data,
    uint wait_us_multiplier,
    size_t iterative_count
)
{
    dynamixel_parse_result result;

    iterative_count = ITERATIVE_COUNT_DEFAULT(iterative_count);

    for (size_t i = 0; i < iterative_count; i++)
    {
        result = dynamixel_send_read_once(
            self, id, start_address, data_size,
            error, data, wait_us_multiplier
        );

        if (result == DYNAMIXEL_PARSE_SUCCESS)
            break;
    }

    return result;
}

dynamixel_parse_result dynamixel_send_read_torque_enable(
    dynamixel_t self,
    uint8_t id,
    uint8_t *error,
    bool *torque_enable,
    uint wait_us_multiplier,
    size_t iterative_count
)
{
    dynamixel_parse_result result;
    uint8_t *data;
    uint16_t start_address, data_size;

    start_address = 64;
    data_size = 1;
    data = (uint8_t *)calloc(data_size + 2, sizeof(uint8_t));

    result = dynamixel_send_read(
        self, id, start_address, data_size,
        error, data, wait_us_multiplier, iterative_count
    );

    if (result == DYNAMIXEL_PARSE_SUCCESS)
        *torque_enable = data[0] == 0x01;
    
    free(data);
    return result;
}

dynamixel_parse_result dynamixel_send_read_position(
    dynamixel_t self,
    uint8_t id,
    uint8_t *error,
    float *position,
    uint wait_us_multiplier,
    size_t iterative_count
)
{
    dynamixel_parse_result result;
    uint8_t *data;
    uint16_t start_address, data_size;

    start_address = 132;
    data_size = 4;
    data = (uint8_t *)calloc(data_size + 2, sizeof(uint8_t));

    result = dynamixel_send_read(
        self, id, start_address, data_size,
        error, data, wait_us_multiplier, iterative_count
    );

    if (result == DYNAMIXEL_PARSE_SUCCESS)
        *position = 0.088 * combine_signed_4_byte(
            data[0], data[1], data[2], data[3]
        );
    
    free(data);
    return result;
}

dynamixel_parse_result dynamixel_send_read_velocity(
    dynamixel_t self,
    uint8_t id,
    uint8_t *error,
    float *velocity,
    uint wait_us_multiplier,
    size_t iterative_count
)
{
    dynamixel_parse_result result;
    uint8_t *data;
    uint16_t start_address, data_size;

    start_address = 128;
    data_size = 4;
    data = (uint8_t *)calloc(data_size + 2, sizeof(uint8_t));

    result = dynamixel_send_read(
        self, id, start_address, data_size,
        error, data, wait_us_multiplier, iterative_count
    );

    if (result == DYNAMIXEL_PARSE_SUCCESS)
        *velocity = 0.229 * combine_signed_4_byte(
            data[0], data[1], data[2], data[3]
        );
    
    free(data);
    return result;
}

dynamixel_parse_result dynamixel_send_read_current(
    dynamixel_t self,
    uint8_t id,
    uint8_t *error,
    float *current,
    uint wait_us_multiplier,
    size_t iterative_count
)
{
    dynamixel_parse_result result;
    uint8_t *data;
    uint16_t start_address, data_size;

    start_address = 126;
    data_size = 2;
    data = (uint8_t *)calloc(data_size + 2, sizeof(uint8_t));

    result = dynamixel_send_read(
        self, id, start_address, data_size,
        error, data, wait_us_multiplier, iterative_count
    );

    if (result == DYNAMIXEL_PARSE_SUCCESS)
        *current = 1.0 * combine_signed_2_byte(data[0], data[1]);
    
    free(data);
    return result;
}

dynamixel_parse_result dynamixel_send_read_temperature(
    dynamixel_t self,
    uint8_t id,
    uint8_t *error,
    uint8_t *temperature,
    uint wait_us_multiplier,
    size_t iterative_count
)
{
    dynamixel_parse_result result;
    uint8_t *data;
    uint16_t start_address, data_size;

    start_address = 146;
    data_size = 1;
    data = (uint8_t *)calloc(data_size + 2, sizeof(uint8_t));

    result = dynamixel_send_read(
        self, id, start_address, data_size,
        error, data, wait_us_multiplier, iterative_count
    );

    if (result == DYNAMIXEL_PARSE_SUCCESS)
        *temperature = data[0];
    
    free(data);
    return result;
}

dynamixel_parse_result dynamixel_send_read_return_delay_time(
    dynamixel_t self,
    uint8_t id,
    uint8_t *error,
    uint16_t *return_delay_time,
    uint wait_us_multiplier,
    size_t iterative_count
)
{
    dynamixel_parse_result result;
    uint8_t *data;
    uint16_t start_address, data_size;

    start_address = 9;
    data_size = 1;
    data = (uint8_t *)calloc(data_size + 2, sizeof(uint8_t));

    result = dynamixel_send_read(
        self, id, start_address, data_size,
        error, data, wait_us_multiplier, iterative_count
    );

    if (result == DYNAMIXEL_PARSE_SUCCESS)
        *return_delay_time = 2 * data[0];
    
    free(data);
    return result;
}

dynamixel_parse_result dynamixel_send_read_drive_mode(
    dynamixel_t self,
    uint8_t id,
    uint8_t *error,
    uint8_t *torque_on_by_goal_update,
    uint8_t *profile_configuration,
    uint8_t *normal_reverse_mode,
    uint wait_us_multiplier,
    size_t iterative_count
)
{
    dynamixel_parse_result result;
    uint8_t *data;
    uint16_t start_address, data_size;

    start_address = 10;
    data_size = 1;
    data = (uint8_t *)calloc(data_size + 2, sizeof(uint8_t));

    result = dynamixel_send_read(
        self, id, start_address, data_size,
        error, data, wait_us_multiplier, iterative_count
    );

    if (result == DYNAMIXEL_PARSE_SUCCESS)
    {
        if (torque_on_by_goal_update)
            *torque_on_by_goal_update = (data[0] & 0b00001000) >> 3;
        if (profile_configuration)
            *profile_configuration = (data[0] & 0b00000100) >> 2;
        if (normal_reverse_mode)
            *normal_reverse_mode = (data[0] & 0b00000001);
    }
    
    free(data);
    return result;
}

dynamixel_parse_result dynamixel_send_read_operating_mode(
    dynamixel_t self,
    uint8_t id,
    uint8_t *error,
    dynamixel_operating_mode *operating_mode,
    uint wait_us_multiplier,
    size_t iterative_count
)
{
    dynamixel_parse_result result;
    uint8_t *data;
    uint16_t start_address, data_size;

    start_address = 11;
    data_size = 1;
    data = (uint8_t *)calloc(data_size + 2, sizeof(uint8_t));

    result = dynamixel_send_read(
        self, id, start_address, data_size,
        error, data, wait_us_multiplier, iterative_count
    );

    if (result == DYNAMIXEL_PARSE_SUCCESS)
        *operating_mode = data[0];
    
    free(data);
    return result;
}

dynamixel_parse_result dynamixel_send_read_baud_rate(
    dynamixel_t self,
    uint8_t id,
    uint8_t *error,
    dynamixel_baud_rate *baud_rate,
    uint wait_us_multiplier,
    size_t iterative_count
)
{
    dynamixel_parse_result result;
    uint8_t *data;
    uint16_t start_address, data_size;

    start_address = 8;
    data_size = 1;
    data = (uint8_t *)calloc(data_size + 2, sizeof(uint8_t));

    result = dynamixel_send_read(
        self, id, start_address, data_size,
        error, data, wait_us_multiplier, iterative_count
    );

    if (result == DYNAMIXEL_PARSE_SUCCESS)
        *baud_rate = data[0];
    
    free(data);
    return result;
}


dynamixel_parse_result dynamixel_send_write_once(
    dynamixel_t self,
    uint8_t id,
    uint16_t start_address,
    uint16_t data_size,
    const uint8_t *data,
    uint8_t *error,
    uint wait_us_multiplier
)
{
    dynamixel_parse_result result;
    size_t parameter_size, status_parameter_size;
    uint8_t *parameter, *status_parameter;

    parameter = (uint8_t *)calloc(2 + data_size, sizeof(uint8_t));
    status_parameter = (uint8_t *)calloc(self->buffer_size, sizeof(uint8_t));

    // 開始アドレス
    divide_into_byte_pair(start_address, parameter, parameter + 1);
    // 書き込みデータ
    memcpy(parameter + 2, data, data_size);

    result = dynamixel_send_packet(
        self, id, DYNAMIXEL__INSTRUCTION_WRITE, 2 + data_size, parameter,
        error, &status_parameter_size, status_parameter, wait_us_multiplier
    );

    free(parameter);
    free(status_parameter);
    return result;
}

dynamixel_parse_result dynamixel_send_write(
    dynamixel_t self,
    uint8_t id,
    uint16_t start_address,
    uint16_t data_size,
    const uint8_t *data,
    uint8_t *error,
    uint wait_us_multiplier,
    size_t iterative_count
)
{
    dynamixel_parse_result result;

    iterative_count = ITERATIVE_COUNT_DEFAULT(iterative_count);

    for (size_t i = 0; i < iterative_count; i++)
    {
        result = dynamixel_send_write_once(
            self, id, start_address, data_size, data,
            error, wait_us_multiplier
        );

        if (result == DYNAMIXEL_PARSE_SUCCESS)
            break;
    }

    return result;
}

dynamixel_parse_result dynamixel_send_write_torque_enable(
    dynamixel_t self,
    uint8_t id,
    uint8_t *error,
    bool torque_enable,
    uint wait_us_multiplier,
    size_t iterative_count
)
{
    dynamixel_parse_result result;
    uint8_t *data;
    uint16_t start_address, data_size;

    start_address = 64;
    data_size = 1;
    data = (uint8_t *)calloc(data_size, sizeof(uint8_t));
    
    if (torque_enable)
        *data = 0x01;
    else
        *data = 0x00;

    result = dynamixel_send_write(
        self, id, start_address, data_size, data,
        error, wait_us_multiplier, iterative_count
    );

    free(data);
    return result;
}

dynamixel_parse_result dynamixel_send_write_goal_position(
    dynamixel_t self,
    uint8_t id,
    uint8_t *error,
    float goal_position,
    uint wait_us_multiplier,
    size_t iterative_count
)
{
    dynamixel_parse_result result;
    uint8_t *data;
    uint16_t start_address, data_size;
    uint32_t goal_position_int;

    start_address = 116;
    data_size = 4;
    data = (uint8_t *)calloc(data_size, sizeof(uint8_t));
    goal_position_int = round(goal_position / 0.088);

    divide_into_4_byte(
        goal_position_int,
        data, data + 1, data + 2, data + 3
    );

    result = dynamixel_send_write(
        self, id, start_address, data_size, data,
        error, wait_us_multiplier, iterative_count
    );

    free(data);
    return result;
}

dynamixel_parse_result dynamixel_send_write_goal_velocity(
    dynamixel_t self,
    uint8_t id,
    uint8_t *error,
    float goal_velocity,
    uint wait_us_multiplier,
    size_t iterative_count
)
{
    dynamixel_parse_result result;
    uint8_t *data;
    uint16_t start_address, data_size;
    uint32_t goal_velocity_int;

    start_address = 104;
    data_size = 4;
    data = (uint8_t *)calloc(data_size, sizeof(uint8_t));
    goal_velocity_int = round(goal_velocity / 0.229);

    divide_into_4_byte(
        goal_velocity_int,
        data, data + 1, data + 2, data + 3
    );

    result = dynamixel_send_write(
        self, id, start_address, data_size, data,
        error, wait_us_multiplier, iterative_count
    );

    free(data);
    return result;
}

dynamixel_parse_result dynamixel_send_write_goal_current(
    dynamixel_t self,
    uint8_t id,
    uint8_t *error,
    float goal_current,
    uint wait_us_multiplier,
    size_t iterative_count
)
{
    dynamixel_parse_result result;
    uint8_t *data;
    uint16_t start_address, data_size;
    uint16_t goal_current_int;

    start_address = 102;
    data_size = 2;
    data = (uint8_t *)calloc(data_size, sizeof(uint8_t));
    goal_current_int = round(goal_current);

    divide_into_byte_pair(
        goal_current_int,
        data, data + 1
    );

    result = dynamixel_send_write(
        self, id, start_address, data_size, data,
        error, wait_us_multiplier, iterative_count
    );

    free(data);
    return result;
}

dynamixel_parse_result dynamixel_send_write_return_delay_time(
    dynamixel_t self,
    uint8_t id,
    uint8_t *error,
    uint16_t return_delay_time,
    uint wait_us_multiplier,
    size_t iterative_count
)
{
    dynamixel_parse_result result;
    uint8_t *data;
    uint16_t start_address, data_size;

    start_address = 9;
    data_size = 1;
    data = (uint8_t *)calloc(data_size, sizeof(uint8_t));
    *data = (return_delay_time + 1) / 2;

    result = dynamixel_send_write(
        self, id, start_address, data_size, data,
        error, wait_us_multiplier, iterative_count
    );

    free(data);
    return result;
}

dynamixel_parse_result dynamixel_send_write_drive_mode(
    dynamixel_t self,
    uint8_t id,
    uint8_t *error,
    uint8_t torque_on_by_goal_update,
    uint8_t profile_configuration,
    uint8_t normal_reverse_mode,
    uint wait_us_multiplier,
    size_t iterative_count
)
{
    dynamixel_parse_result result;
    uint8_t *data;
    uint16_t start_address, data_size;

    start_address = 10;
    data_size = 1;
    data = (uint8_t *)calloc(data_size, sizeof(uint8_t));
    
    *data = 0;
    if (torque_on_by_goal_update == 0x01)
        *data |= (0b1 << 3);
    if (profile_configuration == 0x01)
        *data |= (0b1 << 2);
    if (normal_reverse_mode == 0x01)
        *data |= 0b1;

    result = dynamixel_send_write(
        self, id, start_address, data_size, data,
        error, wait_us_multiplier, iterative_count
    );

    free(data);
    return result;
}

dynamixel_parse_result dynamixel_send_write_operating_mode(
    dynamixel_t self,
    uint8_t id,
    uint8_t *error,
    dynamixel_operating_mode operating_mode,
    uint wait_us_multiplier,
    size_t iterative_count
)
{
    dynamixel_parse_result result;
    uint8_t *data;
    uint16_t start_address, data_size;

    start_address = 11;
    data_size = 1;
    data = (uint8_t *)calloc(data_size, sizeof(uint8_t));
    *data = operating_mode;

    result = dynamixel_send_write(
        self, id, start_address, data_size, data,
        error, wait_us_multiplier, iterative_count
    );

    free(data);
    return result;
}

dynamixel_parse_result dynamixel_send_write_baud_rate(
    dynamixel_t self,
    uint8_t id,
    uint8_t *error,
    dynamixel_baud_rate baud_rate,
    uint wait_us_multiplier,
    size_t iterative_count
)
{
    dynamixel_parse_result result;
    uint8_t *data;
    uint16_t start_address, data_size;

    start_address = 8;
    data_size = 1;
    data = (uint8_t *)calloc(data_size, sizeof(uint8_t));
    *data = baud_rate;

    result = dynamixel_send_write(
        self, id, start_address, data_size, data,
        error, wait_us_multiplier, iterative_count
    );

    free(data);
    return result;
}


dynamixel_parse_result dynamixel_send_reg_write(
    dynamixel_t self,
    uint8_t id,
    uint16_t start_address,
    uint16_t data_size,
    const uint8_t *data,
    uint8_t *error,
    uint wait_us_multiplier
)
{
    dynamixel_parse_result result;
    size_t parameter_size, status_parameter_size;
    uint8_t *parameter, *status_parameter;

    parameter = (uint8_t *)calloc(2 + data_size, sizeof(uint8_t));
    status_parameter = (uint8_t *)calloc(self->buffer_size, sizeof(uint8_t));

    // 開始アドレス
    divide_into_byte_pair(start_address, parameter, parameter + 1);
    // 書き込みデータ
    memcpy(parameter + 2, data, data_size);

    result = dynamixel_send_packet(
        self, id, DYNAMIXEL__INSTRUCTION_REG_WRITE, 2 + data_size, parameter,
        error, &status_parameter_size, status_parameter, wait_us_multiplier
    );

    free(parameter);
    free(status_parameter);
    return result;
}


dynamixel_parse_result dynamixel_send_action(
    dynamixel_t self,
    uint8_t id,
    uint8_t *error,
    uint wait_us_multiplier
)
{
    dynamixel_parse_result result;
    size_t status_parameter_size;
    uint8_t *status_parameter;

    status_parameter = (uint8_t *)calloc(self->buffer_size, sizeof(uint8_t));

    result = dynamixel_send_packet(
        self, id, DYNAMIXEL__INSTRUCTION_ACTION, 0, NULL,
        error, &status_parameter_size, status_parameter, wait_us_multiplier
    );

    free(status_parameter);
    return result;
}


dynamixel_parse_result dynamixel_send_factory_reset(
    dynamixel_t self,
    uint8_t id,
    uint8_t *error,
    dynamixel_factory_reset_paramerter factory_reset,
    uint wait_us_multiplier
)
{
    dynamixel_parse_result result;
    size_t status_parameter_size;
    uint8_t *status_parameter, parameter[1];

    status_parameter = (uint8_t *)calloc(self->buffer_size, sizeof(uint8_t));

    parameter[0] = factory_reset;

    result = dynamixel_send_packet(
        self, id, DYNAMIXEL__INSTRUCTION_FACTORY_RESET, 1, parameter,
        error, &status_parameter_size, status_parameter, wait_us_multiplier
    );

    free(status_parameter);
    return result;
}


dynamixel_parse_result dynamixel_send_reboot(
    dynamixel_t self,
    uint8_t id,
    uint8_t *error,
    uint wait_us_multiplier
)
{
    dynamixel_parse_result result;
    size_t status_parameter_size;
    uint8_t *status_parameter;

    status_parameter = (uint8_t *)calloc(self->buffer_size, sizeof(uint8_t));

    result = dynamixel_send_packet(
        self, id, DYNAMIXEL__INSTRUCTION_REBOOT, 0, NULL,
        error, &status_parameter_size, status_parameter, wait_us_multiplier
    );

    free(status_parameter);
    return result;
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


dynamixel_parse_result dynamixel_partial_uart_packet(
    dynamixel_t self,
    uint8_t id,
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

    if (parse_result == 0)
    {
        // 応答パケットがエラーだった
        if (*error > 0)
            return DYNAMIXEL_PARSE_STATUS_ERROR;

        // インストラクションパケットのIDと応答パケットのIDが違う
        if (status_id != id)
            return DYNAMIXEL_PARSE_WRONG_ID;

        return DYNAMIXEL_PARSE_SUCCESS;
    }

    if (parse_result == 1)
        return DYNAMIXEL_PARSE_WRONG_CHECKSUM;

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

    return DYNAMIXEL_PARSE_INADEQUATE_DATA;
}


dynamixel_parse_result dynamixel_partial_read_and_parse_uart_packet(
    dynamixel_t self,
    uint8_t id,
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
        return DYNAMIXEL_PARSE_HUGE_DATA;
    }

    return dynamixel_partial_uart_packet(
        self, id, error, status_parameter_size,
        status_parameter, initial_status_packet_size
    );
}


dynamixel_parse_result dynamixel_read_uart_packet(
    dynamixel_t self,
    uint8_t id,
    uint8_t *error,
    size_t *status_parameter_size,
    uint8_t *status_parameter,
    uint wait_us_multiplier
)
{
    int uart_read_result;
    size_t status_packet_size;
    uint wait_us = self->wait_us;

    if (wait_us_multiplier)
        wait_us = wait_us_multiplier * wait_us;

    if (!pico_uart_is_readable_within_us(
        self->uart_id, wait_us
    ))
    {
        // 読み込み前に初期化
        status_packet_size = 0;
        memset(self->read_buffer, 0, self->buffer_size);

        do
        {
            uart_read_result = dynamixel_partial_read_and_parse_uart_packet(
                self, id, error, status_parameter_size,
                status_parameter, &status_packet_size
            );

            if (
                uart_read_result == DYNAMIXEL_PARSE_SUCCESS
                || uart_read_result == DYNAMIXEL_PARSE_WRONG_CHECKSUM
                || uart_read_result == DYNAMIXEL_PARSE_STATUS_ERROR
                || uart_read_result == DYNAMIXEL_PARSE_WRONG_ID
                || uart_read_result == DYNAMIXEL_PARSE_HUGE_DATA)
            {
                return uart_read_result;
            }
        }
        while (!pico_uart_is_readable_within_us(
            self->uart_id, wait_us
        ));

        // 応答パケットが返って来たが不十分であった(最後のparse結果を見る)
        if (uart_read_result == DYNAMIXEL_PARSE_INADEQUATE_DATA)
        {
            return DYNAMIXEL_PARSE_INADEQUATE_DATA;
        }
    }

    // 応答が返ってこなかった
    return DYNAMIXEL_PARSE_NO_RESPONSE;
}


dynamixel_parse_result dynamixel_send_packet(
    dynamixel_t self,
    uint8_t id,
    uint8_t instruction,
    uint16_t parameter_size,
    const uint8_t *parameter,
    uint8_t *error,
    size_t *status_parameter_size,
    uint8_t *status_parameter,
    uint wait_us_multiplier
)
{
    dynamixel_write_uart_packet(
        self,
        id, instruction, parameter_size, parameter
    );

    return dynamixel_read_uart_packet(
        self, id,
        error, status_parameter_size, status_parameter,
        wait_us_multiplier
    );
}
