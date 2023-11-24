#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "hardware/gpio.h"
#include "dynamixel/dynamixel.h"

TEST_GROUP(DynamixelInitialization)
{
    uart_inst_t *uart_dummy;

    void setup()
    {
    }

    void teardown()
    {
        mock().clear();
    }
};


TEST(DynamixelInitialization, CreateAfterDestruction)
{
    mock().ignoreOtherCalls();

    dynamixel_t dynamixel_id = dynamixel_create(
        uart_dummy, 8, 9, 57600, 100, 10
    );
    dynamixel_destroy(dynamixel_id);
    dynamixel_id = dynamixel_create(
        uart_dummy, 8, 9, 57600, 100, 10
    );
    dynamixel_destroy(dynamixel_id);
}

TEST(DynamixelInitialization, SetUARTDuringCreation)
{
    mock().expectOneCall("pico_uart_init")
        .withPointerParameter("uart_id", uart_dummy)
        .withUnsignedIntParameter("baud_rate", 57600)
        .withUnsignedIntParameter("data_bits", 8)
        .withUnsignedIntParameter("stop_bits", 1)
        .withIntParameter("parity", UART_PARITY_NONE)
        .andReturnValue(50000);
    mock().expectOneCall("pico_uart_set_fifo_enabled")
        .withPointerParameter("uart_id", uart_dummy)
        .withBoolParameter("enabled", true);
    mock().ignoreOtherCalls();

    dynamixel_t dynamixel_id = dynamixel_create(
        uart_dummy, 8, 9, 57600, 100, 10
    );

    mock().checkExpectations();
    dynamixel_destroy(dynamixel_id);
}

TEST(DynamixelInitialization, SetGPIODuringCreation)
{
    mock().expectOneCall("gpio_set_function")
        .withUnsignedIntParameter("gpio", 8)
        .withIntParameter("fn", GPIO_FUNC_UART);
    mock().expectOneCall("gpio_set_function")
        .withUnsignedIntParameter("gpio", 9)
        .withIntParameter("fn", GPIO_FUNC_UART);
    mock().ignoreOtherCalls();

    dynamixel_t dynamixel_id = dynamixel_create(
        uart_dummy, 8, 9, 57600, 100, 10
    );
    mock().checkExpectations();

    mock().clear();
    mock().ignoreOtherCalls();
    dynamixel_destroy(dynamixel_id);
}

TEST(DynamixelInitialization, DeinitializeGPIODuringDestruction)
{
    mock().ignoreOtherCalls();
    dynamixel_t dynamixel_id = dynamixel_create(
        uart_dummy, 8, 9, 57600, 100, 10
    );
    mock().clear();

    mock().expectOneCall("gpio_set_function")
        .withUnsignedIntParameter("gpio", 8)
        .withIntParameter("fn", GPIO_FUNC_NULL);
    mock().expectOneCall("gpio_set_function")
        .withUnsignedIntParameter("gpio", 9)
        .withIntParameter("fn", GPIO_FUNC_NULL);
    mock().ignoreOtherCalls();

    dynamixel_destroy(dynamixel_id);

    mock().checkExpectations();
}

TEST(DynamixelInitialization, DeinitializeUARTDuringDestruction)
{
    mock().ignoreOtherCalls();
    dynamixel_t dynamixel_id = dynamixel_create(
        uart_dummy, 8, 9, 57600, 100, 10
    );
    mock().clear();

    mock().expectOneCall("pico_uart_deinit")
        .withPointerParameter("uart_id", uart_dummy);
    mock().ignoreOtherCalls();

    dynamixel_destroy(dynamixel_id);

    mock().checkExpectations();
}

TEST(DynamixelInitialization, ProhibitUseOfSameGPIO)
{
    uart_inst_t *uart_dummy2;

    mock().ignoreOtherCalls();
    dynamixel_t dynamixel_id = dynamixel_create(
        uart_dummy, 8, 9, 57600, 100, 10
    );
    dynamixel_t dynamixel_id2 = dynamixel_create(
        uart_dummy2, 8, 17, 57600, 100, 10
    );

    CHECK(dynamixel_id2 == NULL);
    dynamixel_destroy(dynamixel_id);
    if (dynamixel_id2)
        dynamixel_destroy(dynamixel_id2);
}

TEST(DynamixelInitialization, ProhibitUseOfSameUART)
{
    mock().ignoreOtherCalls();
    dynamixel_t dynamixel_id = dynamixel_create(
        uart_dummy, 8, 9, 57600, 100, 10
    );
    dynamixel_t dynamixel_id2 = dynamixel_create(
        uart_dummy, 16, 17, 57600, 100, 10
    );

    CHECK(dynamixel_id2 == NULL);
    dynamixel_destroy(dynamixel_id);
    if (dynamixel_id2)
        dynamixel_destroy(dynamixel_id2);
}
