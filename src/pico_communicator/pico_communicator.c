#include "hardware/uart.h"
#include "pico_communicator/pico_communicator.h"

uint pico_uart_init(
    uart_inst_t *uart_id,
    uint baud_rate,
    uint data_bits,
    uint stop_bits,
    uart_parity_t parity
)
{
    uint actual_baud_rate;

    actual_baud_rate = uart_init(uart_id, baud_rate);

    // picoではCTS/RTSをサポートしていない
    uart_set_hw_flow(uart_id, false, false);
    uart_set_format(uart_id, data_bits, stop_bits, parity);

    return actual_baud_rate;
}


void pico_uart_deinit(
    uart_inst_t *uart_id
)
{
    uart_deinit(uart_id);
}


int pico_uart_write_blocking(
    uart_inst_t *uart_id,
    const uint8_t *src,
    size_t len
)
{
    uart_write_blocking(uart_id, src, len);

    return 0;
}

int pico_uart_is_readabe(
    uart_inst_t *uart_id
)
{
    return !pico_uart_is_readable(uart_id);
}

int pico_uart_read_raw(
    uart_inst_t *uart_id,
    uint8_t *dst
)
{
    if (uart_is_readable(uart_id))
    {
        uart_read_blocking(uart_id, dst, 1);
        return 0;
    }
    else
    {
        return 1;
    }
}
