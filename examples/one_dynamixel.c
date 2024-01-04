#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pico/stdlib.h"
// #include "hardware/uart.h"
// #include "util/analyze_packet.h"
#include "dynamixel/dynamixel.h"

// UART PIN Defines
#define UART_ID (uart1)
const uint UART_TX_PIN = 8;
const uint UART_RX_PIN = 9;
const bool hw_flow_cts = false;
const bool hw_flow_rts = false;

/**
 * シリアル通信の仕様
 * - https://emanual.robotis.com/docs/en/dxl/protocol2/#dynamixel-protocol
 * -
 */
const uint BAUD_RATE = 1000000;
const uint DATA_BITS = 8;
const uint STOP_BITS = 1;
const uart_parity_t PARITY = UART_PARITY_NONE;
/// 命令の種類が多い場合は、多めに取る必要がありそう
const uint32_t WAIT_US = 500;

// Dynamixelの設定
const uint16_t RETURN_DELAY_TIME = 20;
const dynamixel_operating_mode OPERATING_MODE = DYNAMIXEL_OPERATING_MODE_POSITION_CONTROL;

// パケット処理の仕様
#define BUFFER_SIZE 100


int main()
{
    dynamixel_parse_result result;
    uint8_t error = 0, dynamixel_version_of_firmware;
    uint16_t dynamixel_model_no;
    size_t count = 0;
    bool torque_enable;
    float position, velocity, goal_position;
    dynamixel_baud_rate baud_rate;
    dynamixel_operating_mode operating_mode;
    uint8_t temperature, return_delay_time;

    stdio_init_all();

    dynamixel_t dynamixel_id = dynamixel_create(
        UART_ID, UART_RX_PIN, UART_TX_PIN,
        BAUD_RATE, BUFFER_SIZE, WAIT_US
    );

    result = dynamixel_configure(
        dynamixel_id, 0x01,
        &error, RETURN_DELAY_TIME, OPERATING_MODE, 0, 0
    );

    printf("Configure result: %d, error: 0x%02x\n", result, error);

    result = dynamixel_send_write_torque_enable(
        dynamixel_id, 0x01, &error, true, 10, 0
    );
    
    printf(
        "Write Torque Enable result: %d, error: 0x%02x\n", result, error
    );

    srand((unsigned int)time(NULL));

    while (1)
    {
        error = 0;

        // result = dynamixel_send_ping(
        //     dynamixel_id, 0x01,
        //     &error, &dynamixel_model_no, &dynamixel_version_of_firmware, 10
        // );
        
        // printf("Ping result: %d, error: 0x%02x\n", result, error);

        // if (result == DYNAMIXEL_PARSE_SUCCESS)
        // {
        //     printf("  model no.     : 0x%04x\n", dynamixel_model_no);
        //     printf("  firmware ver. : 0x%02x\n", dynamixel_version_of_firmware);
        // }
        
        // sleep_ms(2);

        // result = dynamixel_send_read_torque_enable(
        //     dynamixel_id, 0x01, &error, &torque_enable, 0, 0
        // );
        
        // printf("[%08d] Read torque enable result: %d, error: 0x%02x\n", count, result, error);

        // if (result == DYNAMIXEL_PARSE_SUCCESS)
        // {
        //     printf("  torque enable     : %d\n", torque_enable);
        // }

        // sleep_ms(2);

        result = dynamixel_send_read_position(
            dynamixel_id, 0x01, &error, &position, 0, 0
        );
        
        printf("[%08d] Read position result: %d, error: 0x%02x\n", count, result, error);

        if (result == DYNAMIXEL_PARSE_SUCCESS)
        {
            printf("  position[deg]     : %4.2f\n", position);
        }

        sleep_ms(2);

        result = dynamixel_send_read_baud_rate(
            dynamixel_id, 0x01, &error, &baud_rate, 0, 0
        );
        
        printf("[%08d] Read baud rate result: %d, error: 0x%02x\n", count, result, error);

        if (result == DYNAMIXEL_PARSE_SUCCESS)
        {
            printf("  baud rate[bps]     : %d\n", get_baud_rate(baud_rate));
        }

        // result = dynamixel_send_read_return_delay_time(
        //     dynamixel_id, 0x01, &error, &return_delay_time
        // );
        
        // printf("[%08d] Read return delay time result: %d, error: 0x%02x\n", count, result, error);

        // if (result == DYNAMIXEL_PARSE_SUCCESS)
        // {
        //     printf("  return delay time[us]     : %d\n", return_delay_time);
        // }

        // result = dynamixel_send_read_operating_mode(
        //     dynamixel_id, 0x01, &error, &operating_mode, 0
        // );
        
        // printf("[%08d] Read operating mode result: %d, error: 0x%02x\n", count, result, error);

        // if (result == DYNAMIXEL_PARSE_SUCCESS)
        // {
        //     printf("  operating mode       : %d\n", operating_mode);
        // }

        // result = dynamixel_send_read_temperature(
        //     dynamixel_id, 0x01, &error, &temperature
        // );
        
        // printf("[%08d] Read temperature result: %d, error: 0x%02x\n", count, result, error);

        // if (result == DYNAMIXEL_PARSE_SUCCESS)
        // {
        //     printf("  position[C]     : %d\n", temperature);
        // }

        // sleep_ms(1000);

        // result = dynamixel_send_read_velocity(
        //     dynamixel_id, 0x01, &error, &velocity
        // );
        
        // printf("Read velocity result: %d, error: 0x%02x\n", result, error);

        // if (result == DYNAMIXEL_PARSE_SUCCESS)
        // {
        //     printf("  velocity[rpm]     : %4.2f\n", velocity);
        // }

        goal_position = 360 * (rand() / (1.0 + RAND_MAX));

        result = dynamixel_send_write_goal_position(
            dynamixel_id, 0x01, &error, goal_position, 0, 0
        );
        
        printf(
            "Write Goal Position (%6.2f) result: %d, error: 0x%02x\n",
            goal_position, result, error
        );

        sleep_ms(2000);

        count++;
    }

    dynamixel_destroy(dynamixel_id);

    return 0;
}
