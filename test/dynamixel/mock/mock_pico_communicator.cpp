#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport_c.h"
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
    return mock_c()->actualCall("pico_uart_init")
        ->withPointerParameters("uart_id", uart_id)
        ->withUnsignedIntParameters("baud_rate", baud_rate)
        ->withUnsignedIntParameters("data_bits", data_bits)
        ->withUnsignedIntParameters("stop_bits", stop_bits)
        ->withIntParameters("parity", parity)
        ->unsignedIntReturnValue();
}

void pico_uart_deinit(
    uart_inst_t *uart_id
)
{
    mock_c()->actualCall("pico_uart_deinit")
        ->withPointerParameters("uart_id", uart_id);
}

uint pico_uart_set_baudrate(
    uart_inst_t *uart_id,
    uint baud_rate
)
{
    return mock_c()->actualCall("pico_uart_set_baudrate")
        ->withPointerParameters("uart_id", uart_id)
        ->withUnsignedIntParameters("baud_rate", baud_rate)
        ->unsignedIntReturnValue();
}

void pico_uart_set_fifo_enabled(
    uart_inst_t *uart_id,
    bool enabled
)
{
    mock_c()->actualCall("pico_uart_set_fifo_enabled")
        ->withPointerParameters("uart_id", uart_id)
        ->withBoolParameters("enabled", enabled);
}

int pico_uart_write_blocking(
    uart_inst_t *uart_id, const uint8_t *src, size_t len
)
{
    return mock_c()->actualCall("pico_uart_write_blocking")
        ->withPointerParameters("uart_id", uart_id)
        ->withMemoryBufferParameter("src", src, len)
        ->withUnsignedIntParameters("len", len)
        ->intReturnValue();
}

int pico_uart_is_readable(
    uart_inst_t *uart_id
)
{
    return mock_c()->actualCall("pico_uart_is_readable")
        ->withPointerParameters("uart_id", uart_id)
        ->returnIntValueOrDefault(0);
}

int pico_uart_is_readable_within_us(
    uart_inst_t *uart_id, uint32_t us
)
{
    return mock_c()->actualCall("pico_uart_is_readable_within_us")
        ->withPointerParameters("uart_id", uart_id)
        ->withUnsignedIntParameters("us", us)
        ->returnIntValueOrDefault(0);
}

int pico_uart_read_raw(
    uart_inst_t *uart_id, uint8_t *dst
)
{
    return mock_c()->actualCall("pico_uart_read_raw")
        ->withPointerParameters("uart_id", uart_id)
        ->withOutputParameter("dst", dst)
        ->intReturnValue();
}
