#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "dynamixel/dynamixel.h"
#include "util/analyze_packet.h"
#include "util/packet_byte.h"


TEST_GROUP(DynamixelRead)
{
    uart_inst_t *uart_dummy;
    dynamixel_t dynamixel_id;

    void setup()
    {
        // mockを使って初期化する
        mock().ignoreOtherCalls();
        dynamixel_id = dynamixel_create(
            uart_dummy, 8, 9, 57600, 100, 10
        );
        mock().clear();
    }

    void teardown()
    {
        mock().ignoreOtherCalls();
        dynamixel_destroy(dynamixel_id);
        mock().clear();
    }

};

TEST(DynamixelRead, SendReadTorqueEnableSucceed)
{
    uint8_t id = 0x01, error;
    bool torque_enable;
    int result;

    size_t expected_output_size = 12;
    uint8_t expected_output[] = {
        0xff, 0xff, 0xfd, 0x00,
        0x01,
        0x05, 0x00,
        0x55,
        0x00,
        0x01,
        0x56, 0xa1
    };
    uint8_t expected_error = 0;
    bool expected_torque_enable = true;

    for (int i = 0; i < expected_output_size; i++)
    {
        mock().expectOneCall("pico_uart_read_raw")
            .withPointerParameter("uart_id", uart_dummy)
            .withOutputParameterReturning("dst", expected_output + i, 1)
            .andReturnValue(0);
    }
    // FIFOにこれ以上のデータなし
    mock().expectOneCall("pico_uart_read_raw")
        .withPointerParameter("uart_id", uart_dummy)
        .withOutputParameterReturning("dst", NULL, 0)
        .andReturnValue(1);
    mock().ignoreOtherCalls();

    result = dynamixel_send_read_torque_enable(
        dynamixel_id, id,
        &error, &torque_enable,
        0, 0
    );

    LONGS_EQUAL(DYNAMIXEL_PARSE_SUCCESS, result);
    LONGS_EQUAL(expected_error, error);
    LONGS_EQUAL(expected_torque_enable, torque_enable);
    mock().checkExpectations();
}

TEST(DynamixelRead, SendReadPositionSucceed)
{
    uint8_t id = 0x01, error;
    float position;
    int result;

    size_t expected_output_size = 15;
    uint8_t expected_output[] = {
        0xff, 0xff, 0xfd, 0x00,
        0x01,
        0x08, 0x00,
        0x55,
        0x00,
        0x5d, 0x0e, 0x00, 0x00,
        0x7c, 0x9c
    };
    uint8_t expected_error = 0;
    float expected_position = 323.576;

    for (int i = 0; i < expected_output_size; i++)
    {
        mock().expectOneCall("pico_uart_read_raw")
            .withPointerParameter("uart_id", uart_dummy)
            .withOutputParameterReturning("dst", expected_output + i, 1)
            .andReturnValue(0);
    }
    // FIFOにこれ以上のデータなし
    mock().expectOneCall("pico_uart_read_raw")
        .withPointerParameter("uart_id", uart_dummy)
        .withOutputParameterReturning("dst", NULL, 0)
        .andReturnValue(1);
    mock().ignoreOtherCalls();

    result = dynamixel_send_read_position(
        dynamixel_id, id,
        &error, &position,
        0, 0
    );

    LONGS_EQUAL(DYNAMIXEL_PARSE_SUCCESS, result);
    LONGS_EQUAL(expected_error, error);
    DOUBLES_EQUAL(expected_position, position, 0.088);
    mock().checkExpectations();
}

TEST(DynamixelRead, SendReadVelocitySucceed)
{
    uint8_t id = 0x01, error;
    float velocity;
    int result;

    size_t expected_output_size = 15;
    uint8_t expected_output[] = {
        0xff, 0xff, 0xfd, 0x00,
        0x01,
        0x08, 0x00,
        0x55,
        0x00,
        0x5d, 0x0e, 0x00, 0x00,
        0x7c, 0x9c
    };
    uint8_t expected_error = 0;
    float expected_velocity = 842.033;

    for (int i = 0; i < expected_output_size; i++)
    {
        mock().expectOneCall("pico_uart_read_raw")
            .withPointerParameter("uart_id", uart_dummy)
            .withOutputParameterReturning("dst", expected_output + i, 1)
            .andReturnValue(0);
    }
    // FIFOにこれ以上のデータなし
    mock().expectOneCall("pico_uart_read_raw")
        .withPointerParameter("uart_id", uart_dummy)
        .withOutputParameterReturning("dst", NULL, 0)
        .andReturnValue(1);
    mock().ignoreOtherCalls();

    result = dynamixel_send_read_velocity(
        dynamixel_id, id,
        &error, &velocity,
        0, 0
    );

    LONGS_EQUAL(DYNAMIXEL_PARSE_SUCCESS, result);
    LONGS_EQUAL(expected_error, error);
    DOUBLES_EQUAL(expected_velocity, velocity, 0.229);
    mock().checkExpectations();
}

TEST(DynamixelRead, SendReadCurrentSucceed)
{
    uint8_t id = 0x01, error;
    float current;
    int result;

    size_t expected_output_size = 15;
    uint8_t expected_output[] = {
        0xff, 0xff, 0xfd, 0x00,
        0x01,
        0x06, 0x00,
        0x55,
        0x00,
        0x5d, 0x00,
        0xc0, 0x15
    };
    uint8_t expected_error = 0;
    float expected_current = 93;

    for (int i = 0; i < expected_output_size; i++)
    {
        mock().expectOneCall("pico_uart_read_raw")
            .withPointerParameter("uart_id", uart_dummy)
            .withOutputParameterReturning("dst", expected_output + i, 1)
            .andReturnValue(0);
    }
    // FIFOにこれ以上のデータなし
    mock().expectOneCall("pico_uart_read_raw")
        .withPointerParameter("uart_id", uart_dummy)
        .withOutputParameterReturning("dst", NULL, 0)
        .andReturnValue(1);
    mock().ignoreOtherCalls();

    result = dynamixel_send_read_current(
        dynamixel_id, id,
        &error, &current,
        0, 0
    );

    LONGS_EQUAL(DYNAMIXEL_PARSE_SUCCESS, result);
    LONGS_EQUAL(expected_error, error);
    DOUBLES_EQUAL(expected_current, current, 1e-10);
    mock().checkExpectations();
}

TEST(DynamixelRead, SendReadCurrentTemperature)
{
    uint8_t id = 0x01, error;
    uint8_t temperature;
    int result;

    size_t expected_output_size = 12;
    uint8_t expected_output[] = {
        0xff, 0xff, 0xfd, 0x00,
        0x01,
        0x05, 0x00,
        0x55,
        0x00,
        0x32,
        0xfc, 0xa1
    };
    uint8_t expected_error = 0;
    float expected_temperature = 50;

    for (int i = 0; i < expected_output_size; i++)
    {
        mock().expectOneCall("pico_uart_read_raw")
            .withPointerParameter("uart_id", uart_dummy)
            .withOutputParameterReturning("dst", expected_output + i, 1)
            .andReturnValue(0);
    }
    // FIFOにこれ以上のデータなし
    mock().expectOneCall("pico_uart_read_raw")
        .withPointerParameter("uart_id", uart_dummy)
        .withOutputParameterReturning("dst", NULL, 0)
        .andReturnValue(1);
    mock().ignoreOtherCalls();

    result = dynamixel_send_read_temperature(
        dynamixel_id, id,
        &error, &temperature,
        0, 0
    );

    LONGS_EQUAL(DYNAMIXEL_PARSE_SUCCESS, result);
    LONGS_EQUAL(expected_error, error);
    LONGS_EQUAL(expected_temperature, temperature);
    mock().checkExpectations();
}

TEST(DynamixelRead, SendReadReturnDelayTime)
{
    uint8_t id = 0x01, error;
    uint16_t return_delay_time;
    int result;

    size_t expected_output_size = 12;
    uint8_t expected_output[] = {
        0xff, 0xff, 0xfd, 0x00,
        0x01,
        0x05, 0x00,
        0x55,
        0x00,
        0x32,
        0xfc, 0xa1
    };
    uint8_t expected_error = 0;
    uint16_t expected_return_delay_time = 100;

    for (int i = 0; i < expected_output_size; i++)
    {
        mock().expectOneCall("pico_uart_read_raw")
            .withPointerParameter("uart_id", uart_dummy)
            .withOutputParameterReturning("dst", expected_output + i, 1)
            .andReturnValue(0);
    }
    // FIFOにこれ以上のデータなし
    mock().expectOneCall("pico_uart_read_raw")
        .withPointerParameter("uart_id", uart_dummy)
        .withOutputParameterReturning("dst", NULL, 0)
        .andReturnValue(1);
    mock().ignoreOtherCalls();

    result = dynamixel_send_read_return_delay_time(
        dynamixel_id, id,
        &error, &return_delay_time,
        0, 0
    );

    LONGS_EQUAL(DYNAMIXEL_PARSE_SUCCESS, result);
    LONGS_EQUAL(expected_error, error);
    LONGS_EQUAL(expected_return_delay_time, return_delay_time);
    mock().checkExpectations();
}

TEST(DynamixelRead, SendReadDriveMode)
{
    uint8_t id = 0x01, error, torque_on_by_goal_update, profile_configuration, normal_reverse_mode;
    int result;

    size_t expected_output_size = 12;
    uint8_t expected_output[] = {
        0xff, 0xff, 0xfd, 0x00,
        0x01,
        0x05, 0x00,
        0x55,
        0x00,
        0b00000101,
        0x4d, 0x21
    };
    uint8_t expected_error = 0;
    uint8_t expected_torque_on_by_goal_update = 0;
    uint8_t expected_profile_configuration = 1;
    uint8_t expected_normal_reverse_mode = 1;

    for (int i = 0; i < expected_output_size; i++)
    {
        mock().expectOneCall("pico_uart_read_raw")
            .withPointerParameter("uart_id", uart_dummy)
            .withOutputParameterReturning("dst", expected_output + i, 1)
            .andReturnValue(0);
    }
    // FIFOにこれ以上のデータなし
    mock().expectOneCall("pico_uart_read_raw")
        .withPointerParameter("uart_id", uart_dummy)
        .withOutputParameterReturning("dst", NULL, 0)
        .andReturnValue(1);
    mock().ignoreOtherCalls();

    result = dynamixel_send_read_drive_mode(
        dynamixel_id, id,
        &error, &torque_on_by_goal_update, &profile_configuration, &normal_reverse_mode,
        0, 0
    );

    LONGS_EQUAL(DYNAMIXEL_PARSE_SUCCESS, result);
    LONGS_EQUAL(expected_error, error);
    LONGS_EQUAL(expected_torque_on_by_goal_update, torque_on_by_goal_update);
    LONGS_EQUAL(expected_profile_configuration, profile_configuration);
    LONGS_EQUAL(expected_normal_reverse_mode, normal_reverse_mode);
    mock().checkExpectations();
}

TEST(DynamixelRead, SendReadDriveModeWithNull)
{
    uint8_t id = 0x01, error, torque_on_by_goal_update;
    int result;

    size_t expected_output_size = 12;
    uint8_t expected_output[] = {
        0xff, 0xff, 0xfd, 0x00,
        0x01,
        0x05, 0x00,
        0x55,
        0x00,
        0b00001001,
        0x65, 0x21
    };
    uint8_t expected_error = 0;
    uint8_t expected_torque_on_by_goal_update = 1;

    for (int i = 0; i < expected_output_size; i++)
    {
        mock().expectOneCall("pico_uart_read_raw")
            .withPointerParameter("uart_id", uart_dummy)
            .withOutputParameterReturning("dst", expected_output + i, 1)
            .andReturnValue(0);
    }
    // FIFOにこれ以上のデータなし
    mock().expectOneCall("pico_uart_read_raw")
        .withPointerParameter("uart_id", uart_dummy)
        .withOutputParameterReturning("dst", NULL, 0)
        .andReturnValue(1);
    mock().ignoreOtherCalls();

    result = dynamixel_send_read_drive_mode(
        dynamixel_id, id,
        &error, &torque_on_by_goal_update, NULL, NULL,
        0, 0
    );

    LONGS_EQUAL(DYNAMIXEL_PARSE_SUCCESS, result);
    LONGS_EQUAL(expected_error, error);
    LONGS_EQUAL(expected_torque_on_by_goal_update, torque_on_by_goal_update);
    mock().checkExpectations();
}

TEST(DynamixelRead, SendReadOperatingMode)
{
    uint8_t id = 0x01, error;
    dynamixel_operating_mode operating_mode;
    int result;

    size_t expected_output_size = 12;
    uint8_t expected_output[] = {
        0xff, 0xff, 0xfd, 0x00,
        0x01,
        0x05, 0x00,
        0x55,
        0x00,
        0x03,
        0x59, 0x21
    };
    uint8_t expected_error = 0;
    dynamixel_operating_mode expected_operating_mode = DYNAMIXEL_OPERATING_MODE_POSITION_CONTROL;

    for (int i = 0; i < expected_output_size; i++)
    {
        mock().expectOneCall("pico_uart_read_raw")
            .withPointerParameter("uart_id", uart_dummy)
            .withOutputParameterReturning("dst", expected_output + i, 1)
            .andReturnValue(0);
    }
    // FIFOにこれ以上のデータなし
    mock().expectOneCall("pico_uart_read_raw")
        .withPointerParameter("uart_id", uart_dummy)
        .withOutputParameterReturning("dst", NULL, 0)
        .andReturnValue(1);
    mock().ignoreOtherCalls();

    result = dynamixel_send_read_operating_mode(
        dynamixel_id, id,
        &error, &operating_mode,
        0, 0
    );

    LONGS_EQUAL(DYNAMIXEL_PARSE_SUCCESS, result);
    LONGS_EQUAL(expected_error, error);
    LONGS_EQUAL(expected_operating_mode, operating_mode);
    mock().checkExpectations();
}

TEST(DynamixelRead, SendReadBaudRate)
{
    uint8_t id = 0x01, error;
    dynamixel_baud_rate baud_rate;
    int result;

    size_t expected_output_size = 12;
    uint8_t expected_output[] = {
        0xff, 0xff, 0xfd, 0x00,
        0x01,
        0x05, 0x00,
        0x55,
        0x00,
        0x04,
        0x48, 0xa1
    };
    uint8_t expected_error = 0;
    dynamixel_baud_rate expected_baud_rate = DYNAMIXEL_BAUD_RATE_2M;

    for (int i = 0; i < expected_output_size; i++)
    {
        mock().expectOneCall("pico_uart_read_raw")
            .withPointerParameter("uart_id", uart_dummy)
            .withOutputParameterReturning("dst", expected_output + i, 1)
            .andReturnValue(0);
    }
    // FIFOにこれ以上のデータなし
    mock().expectOneCall("pico_uart_read_raw")
        .withPointerParameter("uart_id", uart_dummy)
        .withOutputParameterReturning("dst", NULL, 0)
        .andReturnValue(1);
    mock().ignoreOtherCalls();

    result = dynamixel_send_read_baud_rate(
        dynamixel_id, id,
        &error, &baud_rate,
        0, 0
    );

    LONGS_EQUAL(DYNAMIXEL_PARSE_SUCCESS, result);
    LONGS_EQUAL(expected_error, error);
    LONGS_EQUAL(expected_baud_rate, baud_rate);
    mock().checkExpectations();
}
