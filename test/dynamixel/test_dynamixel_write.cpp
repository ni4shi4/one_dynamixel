#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "dynamixel/dynamixel.h"
#include "util/analyze_packet.h"
#include "util/packet_byte.h"


TEST_GROUP(DynamixelWrite)
{
    uart_inst_t *uart_dummy;
    dynamixel_t dynamixel_id;

    void setup()
    {
        // mockを使って初期化する
        mock().ignoreOtherCalls();
        dynamixel_id = dynamixel_create(
            uart_dummy, 8, 9, 115200, 100, 10
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

TEST(DynamixelWrite, SendWriteTorqueEnableSucceed)
{
    uint8_t id = 0x01, instruction = 0x03, error;
    uint16_t parameter_size = 0x0003;
    bool torque_enable = true;
    int result;
    uint8_t parameter[] = {
        0x40, 0x00, 0x01
    };

    int expected_packet_size;
    uint8_t expected_error = 0;
    uint8_t expected_packet[100] = {0};
    size_t expected_output_size = 11;
    uint8_t expected_output[] = {
        0xff, 0xff, 0xfd, 0x00,
        0x01,
        0x04, 0x00,
        0x55,
        0x00,
        0xa1, 0x0c
    };

    expected_packet_size = create_uart_packet(
        expected_packet,
        id, instruction, parameter, parameter_size
    );

    mock().expectOneCall("pico_uart_write_blocking")
        .withPointerParameter("uart_id", uart_dummy)
        .withMemoryBufferParameter("src", expected_packet, expected_packet_size)
        .withUnsignedIntParameter("len", expected_packet_size);
    mock().expectOneCall("pico_uart_is_readable_within_us")
        .withPointerParameter("uart_id", uart_dummy)
        .withUnsignedIntParameter("us", 10)
        .andReturnValue(0);
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

    result = dynamixel_send_write_torque_enable(
        dynamixel_id, id,
        &error, torque_enable,
        0, 0
    );

    LONGS_EQUAL(DYNAMIXEL_PARSE_SUCCESS, result);
    LONGS_EQUAL(expected_error, error);
    mock().checkExpectations();
}

TEST(DynamixelWrite, SendWriteGoalPositionSucceed)
{
    uint8_t id = 0x01, instruction = 0x03, error;
    uint16_t parameter_size = 0x0006;
    float goal_position = 302;
    int result;
    uint8_t parameter[] = {
        0x74, 0x00, 0x68, 0x0d, 0x00, 0x00
    };

    int expected_packet_size;
    uint8_t expected_error = 0;
    uint8_t expected_packet[100] = {0};
    size_t expected_output_size = 11;
    uint8_t expected_output[] = {
        0xff, 0xff, 0xfd, 0x00,
        0x01,
        0x04, 0x00,
        0x55,
        0x00,
        0xa1, 0x0c
    };

    expected_packet_size = create_uart_packet(
        expected_packet,
        id, instruction, parameter, parameter_size
    );

    mock().expectOneCall("pico_uart_write_blocking")
        .withPointerParameter("uart_id", uart_dummy)
        .withMemoryBufferParameter("src", expected_packet, expected_packet_size)
        .withUnsignedIntParameter("len", expected_packet_size);
    mock().expectOneCall("pico_uart_is_readable_within_us")
        .withPointerParameter("uart_id", uart_dummy)
        .withUnsignedIntParameter("us", 10)
        .andReturnValue(0);
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

    result = dynamixel_send_write_goal_position(
        dynamixel_id, id,
        &error, goal_position,
        0, 0
    );

    LONGS_EQUAL(DYNAMIXEL_PARSE_SUCCESS, result);
    LONGS_EQUAL(expected_error, error);
    mock().checkExpectations();
}

TEST(DynamixelWrite, SendWriteGoalVelocitySucceed)
{
    uint8_t id = 0x01, instruction = 0x03, error;
    uint16_t parameter_size = 0x0006;
    float goal_velocity = 60;
    int result;
    uint8_t parameter[] = {
        0x68, 0x00, 0x06, 0x01, 0x00, 0x00
    };

    int expected_packet_size;
    uint8_t expected_error = 0;
    uint8_t expected_packet[100] = {0};
    size_t expected_output_size = 11;
    uint8_t expected_output[] = {
        0xff, 0xff, 0xfd, 0x00,
        0x01,
        0x04, 0x00,
        0x55,
        0x00,
        0xa1, 0x0c
    };

    expected_packet_size = create_uart_packet(
        expected_packet,
        id, instruction, parameter, parameter_size
    );

    mock().expectOneCall("pico_uart_write_blocking")
        .withPointerParameter("uart_id", uart_dummy)
        .withMemoryBufferParameter("src", expected_packet, expected_packet_size)
        .withUnsignedIntParameter("len", expected_packet_size);
    mock().expectOneCall("pico_uart_is_readable_within_us")
        .withPointerParameter("uart_id", uart_dummy)
        .withUnsignedIntParameter("us", 10)
        .andReturnValue(0);
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

    result = dynamixel_send_write_goal_velocity(
        dynamixel_id, id,
        &error, goal_velocity,
        0, 0
    );

    LONGS_EQUAL(DYNAMIXEL_PARSE_SUCCESS, result);
    LONGS_EQUAL(expected_error, error);
    mock().checkExpectations();
}

TEST(DynamixelWrite, SendWriteGoalCurrentSucceed)
{
    uint8_t id = 0x01, instruction = 0x03, error;
    uint16_t parameter_size = 0x0004;
    float goal_current = 1001;
    int result;
    uint8_t parameter[] = {
        0x66, 0x00, 0xe9, 0x03
    };

    int expected_packet_size;
    uint8_t expected_error = 0;
    uint8_t expected_packet[100] = {0};
    size_t expected_output_size = 11;
    uint8_t expected_output[] = {
        0xff, 0xff, 0xfd, 0x00,
        0x01,
        0x04, 0x00,
        0x55,
        0x00,
        0xa1, 0x0c
    };

    expected_packet_size = create_uart_packet(
        expected_packet,
        id, instruction, parameter, parameter_size
    );

    mock().expectOneCall("pico_uart_write_blocking")
        .withPointerParameter("uart_id", uart_dummy)
        .withMemoryBufferParameter("src", expected_packet, expected_packet_size)
        .withUnsignedIntParameter("len", expected_packet_size);
    mock().expectOneCall("pico_uart_is_readable_within_us")
        .withPointerParameter("uart_id", uart_dummy)
        .withUnsignedIntParameter("us", 10)
        .andReturnValue(0);
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

    result = dynamixel_send_write_goal_current(
        dynamixel_id, id,
        &error, goal_current,
        0, 0
    );

    LONGS_EQUAL(DYNAMIXEL_PARSE_SUCCESS, result);
    LONGS_EQUAL(expected_error, error);
    mock().checkExpectations();
}

TEST(DynamixelWrite, SendWriteReturnDelayTime)
{
    uint8_t id = 0x01, instruction = 0x03, error;
    uint16_t parameter_size = 0x0003;
    uint16_t return_delay_time = 253;
    int result;
    uint8_t parameter[] = {
        0x09, 0x00, 0x7f
    };

    int expected_packet_size;
    uint8_t expected_error = 0;
    uint8_t expected_packet[100] = {0};
    size_t expected_output_size = 11;
    uint8_t expected_output[] = {
        0xff, 0xff, 0xfd, 0x00,
        0x01,
        0x04, 0x00,
        0x55,
        0x00,
        0xa1, 0x0c
    };

    expected_packet_size = create_uart_packet(
        expected_packet,
        id, instruction, parameter, parameter_size
    );

    mock().expectOneCall("pico_uart_write_blocking")
        .withPointerParameter("uart_id", uart_dummy)
        .withMemoryBufferParameter("src", expected_packet, expected_packet_size)
        .withUnsignedIntParameter("len", expected_packet_size);
    mock().expectOneCall("pico_uart_is_readable_within_us")
        .withPointerParameter("uart_id", uart_dummy)
        .withUnsignedIntParameter("us", 10)
        .andReturnValue(0);
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

    result = dynamixel_send_write_return_delay_time(
        dynamixel_id, id,
        &error, return_delay_time,
        0, 0
    );

    LONGS_EQUAL(DYNAMIXEL_PARSE_SUCCESS, result);
    LONGS_EQUAL(expected_error, error);
    mock().checkExpectations();
}

TEST(DynamixelWrite, SendWriteDriveMode)
{
    uint8_t id = 0x01, instruction = 0x03, error, torque_on_by_goal_update = 0x00, profile_configuration = 0x01, normal_reverse_mode = 0x01;
    uint16_t parameter_size = 0x0003;
    int result;
    uint8_t parameter[] = {
        0x0a, 0x00, 0b00000101
    };

    int expected_packet_size;
    uint8_t expected_error = 0;
    uint8_t expected_packet[100] = {0};
    size_t expected_output_size = 11;
    uint8_t expected_output[] = {
        0xff, 0xff, 0xfd, 0x00,
        0x01,
        0x04, 0x00,
        0x55,
        0x00,
        0xa1, 0x0c
    };

    expected_packet_size = create_uart_packet(
        expected_packet,
        id, instruction, parameter, parameter_size
    );

    mock().expectOneCall("pico_uart_write_blocking")
        .withPointerParameter("uart_id", uart_dummy)
        .withMemoryBufferParameter("src", expected_packet, expected_packet_size)
        .withUnsignedIntParameter("len", expected_packet_size);
    mock().expectOneCall("pico_uart_is_readable_within_us")
        .withPointerParameter("uart_id", uart_dummy)
        .withUnsignedIntParameter("us", 10)
        .andReturnValue(0);
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

    result = dynamixel_send_write_drive_mode(
        dynamixel_id, id,
        &error, torque_on_by_goal_update, profile_configuration, normal_reverse_mode,
        0, 0
    );

    LONGS_EQUAL(DYNAMIXEL_PARSE_SUCCESS, result);
    LONGS_EQUAL(expected_error, error);
    mock().checkExpectations();
}

TEST(DynamixelWrite, SendWriteOperatingMode)
{
    uint8_t id = 0x01, instruction = 0x03, error;
    uint16_t parameter_size = 0x0003;
    dynamixel_operating_mode operating_mode = DYNAMIXEL_OPERATING_MODE_VELOCITY_CONTROL;
    int result;
    uint8_t parameter[] = {
        0x0b, 0x00, operating_mode
    };

    int expected_packet_size;
    uint8_t expected_error = 0;
    uint8_t expected_packet[100] = {0};
    size_t expected_output_size = 11;
    uint8_t expected_output[] = {
        0xff, 0xff, 0xfd, 0x00,
        0x01,
        0x04, 0x00,
        0x55,
        0x00,
        0xa1, 0x0c
    };

    expected_packet_size = create_uart_packet(
        expected_packet,
        id, instruction, parameter, parameter_size
    );

    mock().expectOneCall("pico_uart_write_blocking")
        .withPointerParameter("uart_id", uart_dummy)
        .withMemoryBufferParameter("src", expected_packet, expected_packet_size)
        .withUnsignedIntParameter("len", expected_packet_size);
    mock().expectOneCall("pico_uart_is_readable_within_us")
        .withPointerParameter("uart_id", uart_dummy)
        .withUnsignedIntParameter("us", 10)
        .andReturnValue(0);
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

    result = dynamixel_send_write_operating_mode(
        dynamixel_id, id,
        &error, operating_mode,
        0, 0
    );

    LONGS_EQUAL(DYNAMIXEL_PARSE_SUCCESS, result);
    LONGS_EQUAL(expected_error, error);
    mock().checkExpectations();
}

TEST(DynamixelWrite, SendWriteBaudRate)
{
    uint8_t id = 0x01, instruction = 0x03, error;
    uint16_t parameter_size = 0x0003;
    dynamixel_baud_rate baud_rate = DYNAMIXEL_BAUD_RATE_1M;
    int result;
    uint8_t parameter[] = {
        0x08, 0x00, baud_rate
    };

    int expected_packet_size;
    uint8_t expected_error = 0;
    uint8_t expected_packet[100] = {0};
    size_t expected_output_size = 11;
    uint8_t expected_output[] = {
        0xff, 0xff, 0xfd, 0x00,
        0x01,
        0x04, 0x00,
        0x55,
        0x00,
        0xa1, 0x0c
    };

    expected_packet_size = create_uart_packet(
        expected_packet,
        id, instruction, parameter, parameter_size
    );

    mock().expectOneCall("pico_uart_write_blocking")
        .withPointerParameter("uart_id", uart_dummy)
        .withMemoryBufferParameter("src", expected_packet, expected_packet_size)
        .withUnsignedIntParameter("len", expected_packet_size);
    mock().expectOneCall("pico_uart_is_readable_within_us")
        .withPointerParameter("uart_id", uart_dummy)
        .withUnsignedIntParameter("us", 10)
        .andReturnValue(0);
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

    result = dynamixel_send_write_baud_rate(
        dynamixel_id, id,
        &error, baud_rate,
        0, 0
    );

    LONGS_EQUAL(DYNAMIXEL_PARSE_SUCCESS, result);
    LONGS_EQUAL(expected_error, error);
    mock().checkExpectations();
}

TEST(DynamixelWrite, Configure)
{
    uint8_t id = 0x01, instruction = 0x03, instruction_ping = 0x01, error;
    uint16_t parameter_size0 = 0x0003, parameter_size1 = 0x0003, parameter_size2 = 0x0003, parameter_size3 = 0x0003;
    dynamixel_baud_rate baud_rate = DYNAMIXEL_BAUD_RATE_115200;
    uint16_t return_delay_time = 252;
    dynamixel_operating_mode operating_mode = DYNAMIXEL_OPERATING_MODE_CURRENT_CONTROL;
    int result;
    uint8_t parameter0[] = {
        0x40, 0x00, 0x00
    },
    parameter1[] = {
        0x08, 0x00, baud_rate
    },
    parameter2[] = {
        0x09, 0x00, 0x7e
    },
    parameter3[] = {
        0x0b, 0x00, operating_mode
    };

    int expected_packet_size_ping, expected_packet_size0, expected_packet_size1, expected_packet_size2, expected_packet_size3;
    uint8_t expected_error = 0;
    uint8_t packet_ping[100] = {0}, expected_packet0[100] = {0}, expected_packet1[100] = {0}, expected_packet2[100] = {0}, expected_packet3[100] = {0};
    size_t expected_output_size_ping = 14, expected_output_size = 11;
    uint8_t expected_output_ping[] = {
        0xff, 0xff, 0xfd, 0x00,
        0x01,
        0x07, 0x00,
        0x55,
        0x00,
        0x06, 0x04,
        0x26,
        0x65, 0x5d
    },
    expected_output[] = {
        0xff, 0xff, 0xfd, 0x00,
        0x01,
        0x04, 0x00,
        0x55,
        0x00,
        0xa1, 0x0c
    };

    expected_packet_size_ping = create_uart_packet(
        packet_ping,
        id, instruction_ping, NULL, 0
    );
    expected_packet_size0 = create_uart_packet(
        expected_packet0,
        id, instruction, parameter0, parameter_size0
    );
    expected_packet_size1 = create_uart_packet(
        expected_packet1,
        id, instruction, parameter1, parameter_size1
    );
    expected_packet_size2 = create_uart_packet(
        expected_packet2,
        id, instruction, parameter2, parameter_size2
    );
    expected_packet_size3 = create_uart_packet(
        expected_packet3,
        id, instruction, parameter3, parameter_size3
    );

    // PING
    mock().expectOneCall("pico_uart_write_blocking")
        .withPointerParameter("uart_id", uart_dummy)
        .withMemoryBufferParameter("src", packet_ping, expected_packet_size_ping)
        .withUnsignedIntParameter("len", expected_packet_size_ping);
    mock().expectOneCall("pico_uart_is_readable_within_us")
        .withPointerParameter("uart_id", uart_dummy)
        .withUnsignedIntParameter("us", 100000)
        .andReturnValue(0);
    for (int i = 0; i < expected_output_size_ping; i++)
    {
        mock().expectOneCall("pico_uart_read_raw")
            .withPointerParameter("uart_id", uart_dummy)
            .withOutputParameterReturning("dst", expected_output_ping + i, 1)
            .andReturnValue(0);
    }
    // FIFOにこれ以上のデータなし
    mock().expectOneCall("pico_uart_read_raw")
        .withPointerParameter("uart_id", uart_dummy)
        .withOutputParameterReturning("dst", NULL, 0)
        .andReturnValue(1);

    // Torque Enable
    mock().expectOneCall("pico_uart_write_blocking")
        .withPointerParameter("uart_id", uart_dummy)
        .withMemoryBufferParameter("src", expected_packet0, expected_packet_size0)
        .withUnsignedIntParameter("len", expected_packet_size0);
    mock().expectOneCall("pico_uart_is_readable_within_us")
        .withPointerParameter("uart_id", uart_dummy)
        .withUnsignedIntParameter("us", 100000)
        .andReturnValue(0);
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

    // ボーレート
    mock().expectOneCall("pico_uart_write_blocking")
        .withPointerParameter("uart_id", uart_dummy)
        .withMemoryBufferParameter("src", expected_packet1, expected_packet_size1)
        .withUnsignedIntParameter("len", expected_packet_size1);
    mock().expectOneCall("pico_uart_is_readable_within_us")
        .withPointerParameter("uart_id", uart_dummy)
        .withUnsignedIntParameter("us", 100000)
        .andReturnValue(0);
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

    // return delay time
    mock().expectOneCall("pico_uart_write_blocking")
        .withPointerParameter("uart_id", uart_dummy)
        .withMemoryBufferParameter("src", expected_packet2, expected_packet_size2)
        .withUnsignedIntParameter("len", expected_packet_size2);
    mock().expectOneCall("pico_uart_is_readable_within_us")
        .withPointerParameter("uart_id", uart_dummy)
        .withUnsignedIntParameter("us", 100000)
        .andReturnValue(0);
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

    // operating mode
    mock().expectOneCall("pico_uart_write_blocking")
        .withPointerParameter("uart_id", uart_dummy)
        .withMemoryBufferParameter("src", expected_packet3, expected_packet_size3)
        .withUnsignedIntParameter("len", expected_packet_size3);
    mock().expectOneCall("pico_uart_is_readable_within_us")
        .withPointerParameter("uart_id", uart_dummy)
        .withUnsignedIntParameter("us", 100000)
        .andReturnValue(0);
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

    result = dynamixel_configure(
        dynamixel_id, id,
        &error, return_delay_time, operating_mode,
        0, 0
    );

    LONGS_EQUAL(DYNAMIXEL_PARSE_SUCCESS, result);
    LONGS_EQUAL(expected_error, error);
    mock().checkExpectations();
}

TEST(DynamixelWrite, ConfigureMultiplePing)
{
    uint8_t id = 0x01, instruction = 0x03, instruction_ping = 0x01, error;
    uint16_t parameter_size0 = 0x0003, parameter_size1 = 0x0003, parameter_size2 = 0x0003, parameter_size3 = 0x0003;
    dynamixel_baud_rate baud_rate = DYNAMIXEL_BAUD_RATE_115200;
    uint16_t return_delay_time = 252;
    dynamixel_operating_mode operating_mode = DYNAMIXEL_OPERATING_MODE_CURRENT_CONTROL;
    int result;
    uint8_t parameter0[] = {
        0x40, 0x00, 0x00
    },
    parameter1[] = {
        0x08, 0x00, baud_rate
    },
    parameter2[] = {
        0x09, 0x00, 0x7e
    },
    parameter3[] = {
        0x0b, 0x00, operating_mode
    };

    int expected_packet_size_ping, expected_packet_size0, expected_packet_size1, expected_packet_size2, expected_packet_size3;
    uint8_t expected_error = 0;
    uint8_t packet_ping[100] = {0}, expected_packet0[100] = {0}, expected_packet1[100] = {0}, expected_packet2[100] = {0}, expected_packet3[100] = {0};
    size_t expected_output_size_ping = 14, expected_output_size = 11;
    uint8_t expected_output_ping[] = {
        0xff, 0xff, 0xfd, 0x00,
        0x01,
        0x07, 0x00,
        0x55,
        0x00,
        0x06, 0x04,
        0x26,
        0x65, 0x5d
    },
    expected_output[] = {
        0xff, 0xff, 0xfd, 0x00,
        0x01,
        0x04, 0x00,
        0x55,
        0x00,
        0xa1, 0x0c
    };

    expected_packet_size_ping = create_uart_packet(
        packet_ping,
        id, instruction_ping, NULL, 0
    );
    expected_packet_size0 = create_uart_packet(
        expected_packet0,
        id, instruction, parameter0, parameter_size0
    );
    expected_packet_size1 = create_uart_packet(
        expected_packet1,
        id, instruction, parameter1, parameter_size1
    );
    expected_packet_size2 = create_uart_packet(
        expected_packet2,
        id, instruction, parameter2, parameter_size2
    );
    expected_packet_size3 = create_uart_packet(
        expected_packet3,
        id, instruction, parameter3, parameter_size3
    );

    // PING failure1
    mock().expectOneCall("pico_uart_set_baudrate")
        .withPointerParameter("uart_id", uart_dummy)
        .withUnsignedIntParameter("baud_rate", 9600)
        .andReturnValue(9600);
    mock().expectOneCall("pico_uart_write_blocking")
        .withPointerParameter("uart_id", uart_dummy)
        .withMemoryBufferParameter("src", packet_ping, expected_packet_size_ping)
        .withUnsignedIntParameter("len", expected_packet_size_ping);
    mock().expectOneCall("pico_uart_is_readable_within_us")
        .withPointerParameter("uart_id", uart_dummy)
        .withUnsignedIntParameter("us", 100000)
        .andReturnValue(1);

    // PING
    mock().expectOneCall("pico_uart_set_baudrate")
        .withPointerParameter("uart_id", uart_dummy)
        .withUnsignedIntParameter("baud_rate", 57600)
        .andReturnValue(57600);
    mock().expectOneCall("pico_uart_write_blocking")
        .withPointerParameter("uart_id", uart_dummy)
        .withMemoryBufferParameter("src", packet_ping, expected_packet_size_ping)
        .withUnsignedIntParameter("len", expected_packet_size_ping);
    mock().expectOneCall("pico_uart_is_readable_within_us")
        .withPointerParameter("uart_id", uart_dummy)
        .withUnsignedIntParameter("us", 100000)
        .andReturnValue(0);
    for (int i = 0; i < expected_output_size_ping; i++)
    {
        mock().expectOneCall("pico_uart_read_raw")
            .withPointerParameter("uart_id", uart_dummy)
            .withOutputParameterReturning("dst", expected_output_ping + i, 1)
            .andReturnValue(0);
    }
    // FIFOにこれ以上のデータなし
    mock().expectOneCall("pico_uart_read_raw")
        .withPointerParameter("uart_id", uart_dummy)
        .withOutputParameterReturning("dst", NULL, 0)
        .andReturnValue(1);

    // Torque Enable
    mock().expectOneCall("pico_uart_write_blocking")
        .withPointerParameter("uart_id", uart_dummy)
        .withMemoryBufferParameter("src", expected_packet0, expected_packet_size0)
        .withUnsignedIntParameter("len", expected_packet_size0);
    mock().expectOneCall("pico_uart_is_readable_within_us")
        .withPointerParameter("uart_id", uart_dummy)
        .withUnsignedIntParameter("us", 100000)
        .andReturnValue(0);
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

    // ボーレート
    mock().expectOneCall("pico_uart_write_blocking")
        .withPointerParameter("uart_id", uart_dummy)
        .withMemoryBufferParameter("src", expected_packet1, expected_packet_size1)
        .withUnsignedIntParameter("len", expected_packet_size1);
    mock().expectOneCall("pico_uart_is_readable_within_us")
        .withPointerParameter("uart_id", uart_dummy)
        .withUnsignedIntParameter("us", 100000)
        .andReturnValue(0);
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
    mock().expectOneCall("pico_uart_set_baudrate")
        .withPointerParameter("uart_id", uart_dummy)
        .withUnsignedIntParameter("baud_rate", 115200)
        .andReturnValue(115200);

    // return delay time
    mock().expectOneCall("pico_uart_write_blocking")
        .withPointerParameter("uart_id", uart_dummy)
        .withMemoryBufferParameter("src", expected_packet2, expected_packet_size2)
        .withUnsignedIntParameter("len", expected_packet_size2);
    mock().expectOneCall("pico_uart_is_readable_within_us")
        .withPointerParameter("uart_id", uart_dummy)
        .withUnsignedIntParameter("us", 100000)
        .andReturnValue(0);
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

    // operating mode
    mock().expectOneCall("pico_uart_write_blocking")
        .withPointerParameter("uart_id", uart_dummy)
        .withMemoryBufferParameter("src", expected_packet3, expected_packet_size3)
        .withUnsignedIntParameter("len", expected_packet_size3);
    mock().expectOneCall("pico_uart_is_readable_within_us")
        .withPointerParameter("uart_id", uart_dummy)
        .withUnsignedIntParameter("us", 100000)
        .andReturnValue(0);
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

    result = dynamixel_configure(
        dynamixel_id, id,
        &error, return_delay_time, operating_mode,
        0, 0
    );

    LONGS_EQUAL(DYNAMIXEL_PARSE_SUCCESS, result);
    LONGS_EQUAL(expected_error, error);
    mock().checkExpectations();
}

// 1度失敗したが2度目に成功する
TEST(DynamixelWrite, ConfigureWithRetry)
{
    uint8_t id = 0x01, instruction = 0x03, instruction_ping = 0x01, error;
    uint16_t parameter_size0 = 0x0003, parameter_size1 = 0x0003, parameter_size2 = 0x0003, parameter_size3 = 0x0003;
    dynamixel_baud_rate baud_rate = DYNAMIXEL_BAUD_RATE_115200;
    uint16_t return_delay_time = 252;
    dynamixel_operating_mode operating_mode = DYNAMIXEL_OPERATING_MODE_CURRENT_CONTROL;
    int result;
    uint8_t parameter0[] = {
        0x40, 0x00, 0x00
    },
    parameter1[] = {
        0x08, 0x00, baud_rate
    },
    parameter2[] = {
        0x09, 0x00, 0x7e
    },
    parameter3[] = {
        0x0b, 0x00, operating_mode
    };

    int expected_packet_size_ping, expected_packet_size0, expected_packet_size1, expected_packet_size2, expected_packet_size3;
    uint8_t expected_error = 0;
    uint8_t packet_ping[100] = {0}, expected_packet0[100] = {0}, expected_packet1[100] = {0}, expected_packet2[100] = {0}, expected_packet3[100] = {0};
    size_t expected_output_size_ping = 14, expected_output_size = 11;
    uint8_t expected_output_ping[] = {
        0xff, 0xff, 0xfd, 0x00,
        0x01,
        0x07, 0x00,
        0x55,
        0x00,
        0x06, 0x04,
        0x26,
        0x65, 0x5d
    },
    expected_output[] = {
        0xff, 0xff, 0xfd, 0x00,
        0x01,
        0x04, 0x00,
        0x55,
        0x00,
        0xa1, 0x0c
    };

    expected_packet_size_ping = create_uart_packet(
        packet_ping,
        id, instruction_ping, NULL, 0
    );
    expected_packet_size0 = create_uart_packet(
        expected_packet0,
        id, instruction, parameter0, parameter_size0
    );
    expected_packet_size1 = create_uart_packet(
        expected_packet1,
        id, instruction, parameter1, parameter_size1
    );
    expected_packet_size2 = create_uart_packet(
        expected_packet2,
        id, instruction, parameter2, parameter_size2
    );
    expected_packet_size3 = create_uart_packet(
        expected_packet3,
        id, instruction, parameter3, parameter_size3
    );

    // PING
    mock().expectOneCall("pico_uart_write_blocking")
        .withPointerParameter("uart_id", uart_dummy)
        .withMemoryBufferParameter("src", packet_ping, expected_packet_size_ping)
        .withUnsignedIntParameter("len", expected_packet_size_ping);
    mock().expectOneCall("pico_uart_is_readable_within_us")
        .withPointerParameter("uart_id", uart_dummy)
        .withUnsignedIntParameter("us", 100000)
        .andReturnValue(0);
    for (int i = 0; i < expected_output_size_ping; i++)
    {
        mock().expectOneCall("pico_uart_read_raw")
            .withPointerParameter("uart_id", uart_dummy)
            .withOutputParameterReturning("dst", expected_output_ping + i, 1)
            .andReturnValue(0);
    }
    // FIFOにこれ以上のデータなし
    mock().expectOneCall("pico_uart_read_raw")
        .withPointerParameter("uart_id", uart_dummy)
        .withOutputParameterReturning("dst", NULL, 0)
        .andReturnValue(1);

    // Torque Enable1
    mock().expectOneCall("pico_uart_write_blocking")
        .withPointerParameter("uart_id", uart_dummy)
        .withMemoryBufferParameter("src", expected_packet0, expected_packet_size0)
        .withUnsignedIntParameter("len", expected_packet_size0);
    // レスポンスなし
    mock().expectOneCall("pico_uart_is_readable_within_us")
        .withPointerParameter("uart_id", uart_dummy)
        .withUnsignedIntParameter("us", 100000)
        .andReturnValue(1);

    // Torque Enable2
    mock().expectOneCall("pico_uart_write_blocking")
        .withPointerParameter("uart_id", uart_dummy)
        .withMemoryBufferParameter("src", expected_packet0, expected_packet_size0)
        .withUnsignedIntParameter("len", expected_packet_size0);
    mock().expectOneCall("pico_uart_is_readable_within_us")
        .withPointerParameter("uart_id", uart_dummy)
        .withUnsignedIntParameter("us", 100000)
        .andReturnValue(0);
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

    // ボーレート
    mock().expectOneCall("pico_uart_write_blocking")
        .withPointerParameter("uart_id", uart_dummy)
        .withMemoryBufferParameter("src", expected_packet1, expected_packet_size1)
        .withUnsignedIntParameter("len", expected_packet_size1);
    mock().expectOneCall("pico_uart_is_readable_within_us")
        .withPointerParameter("uart_id", uart_dummy)
        .withUnsignedIntParameter("us", 100000)
        .andReturnValue(0);
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

    // return delay time
    mock().expectOneCall("pico_uart_write_blocking")
        .withPointerParameter("uart_id", uart_dummy)
        .withMemoryBufferParameter("src", expected_packet2, expected_packet_size2)
        .withUnsignedIntParameter("len", expected_packet_size2);
    mock().expectOneCall("pico_uart_is_readable_within_us")
        .withPointerParameter("uart_id", uart_dummy)
        .withUnsignedIntParameter("us", 100000)
        .andReturnValue(0);
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

    // operating mode
    mock().expectOneCall("pico_uart_write_blocking")
        .withPointerParameter("uart_id", uart_dummy)
        .withMemoryBufferParameter("src", expected_packet3, expected_packet_size3)
        .withUnsignedIntParameter("len", expected_packet_size3);
    mock().expectOneCall("pico_uart_is_readable_within_us")
        .withPointerParameter("uart_id", uart_dummy)
        .withUnsignedIntParameter("us", 100000)
        .andReturnValue(0);
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

    result = dynamixel_configure(
        dynamixel_id, id,
        &error, return_delay_time, operating_mode,
        0, 0
    );

    LONGS_EQUAL(DYNAMIXEL_PARSE_SUCCESS, result);
    LONGS_EQUAL(expected_error, error);
    mock().checkExpectations();
}
