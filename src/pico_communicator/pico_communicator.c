#include "hardware/uart.h"
#include "pico_communicator/pico_communicator.h"


int pico_uart_write_blocking(
    uart_inst_t *uart_id,
    const uint8_t *src,
    size_t len
)
{
    uart_write_blocking(uart_id, src, len);

    return 0;
}

int pico_uart_is_readable(
    uart_inst_t *uart_id
)
{
    return !uart_is_readable(uart_id);
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
