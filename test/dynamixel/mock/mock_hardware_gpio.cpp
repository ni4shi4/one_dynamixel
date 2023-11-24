#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport_c.h"
#include "hardware/gpio.h"


void gpio_set_function(uint gpio, gpio_function fn)
{
    mock_c()->actualCall("gpio_set_function")
        ->withUnsignedIntParameters("gpio", gpio)
        ->withIntParameters("fn", fn);
}
