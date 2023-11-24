#include "hardware/uart.h"
#include "pico_communicator/pico_communicator.h"


void pico_uart_set_fifo_enabled(
    uart_inst_t *uart_id,
    bool enabled
)
{
    uart_set_fifo_enabled(uart_id, enabled);
}


int pico_uart_is_readable_within_us(
    uart_inst_t *uart_id,
    uint us
)
{
    return !pico_uart_is_readable_within_us(
        uart_id, us
    );
}
