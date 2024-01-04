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

    // uart_set_hw_flow(uart_id, false, false);
    // uart_set_format(uart_id, data_bits, stop_bits, parity);

    return actual_baud_rate;
}


void pico_uart_deinit(
    uart_inst_t *uart_id
)
{
}


uint pico_uart_set_baudrate(
    uart_inst_t *uart_id,
    uint baud_rate
)
{
    return baud_rate;
}

void pico_uart_set_fifo_enabled(
    uart_inst_t *uart_id,
    bool enabled
)
{
}


int pico_uart_is_readable_within_us(
    uart_inst_t *uart_id,
    uint us
)
{
    return 0;
}
