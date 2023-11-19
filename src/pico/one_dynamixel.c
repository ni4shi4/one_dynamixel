#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "uart/communication.h"

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
const uint BAUD_RATE = 57600;
const uint DATA_BITS = 8;
const uint STOP_BITS = 1;
const uart_parity_t PARITY = UART_PARITY_NONE;
const uint32_t WAIT_US = 10;

// パケット処理の仕様
#define READ_SIZE 50
#define BUFFER_SIZE 100


int main()
{
    stdio_init_all();

    uint actual_baud_rate = uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    uart_set_hw_flow(UART_ID, hw_flow_cts, hw_flow_rts);
    uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY);

    uart_set_fifo_enabled(UART_ID, true);

    uint8_t buffer[BUFFER_SIZE] = {0};
    uint8_t packet[BUFFER_SIZE] = {0};
    int packet_size;

    while (1)
    {
        packet_size = create_uart_packet(packet, 0x01, 1, NULL, 0);
        uart_write_blocking(UART_ID, packet, packet_size);

        if (uart_is_readable_within_us(UART_ID, WAIT_US))
        {
            for (int i = 0; i < READ_SIZE; i++)
            {
                uart_read_blocking(UART_ID, buffer + i, 1);

                if (!uart_is_readable(UART_ID))
                    break;
            }

            puts("Read buffer:");
            for (int i = 0; i < 20; i++)
            {
                printf("0x%02x ", buffer[i]);
            }
            puts("\n");
        }
        else
        {
            puts("No data available");
        }

        sleep_ms(1000);
    }

    return 0;
}
