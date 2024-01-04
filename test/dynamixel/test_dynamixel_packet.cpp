#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "dynamixel/dynamixel.h"
#include "util/analyze_packet.h"

TEST_GROUP(DynamixelPacket)
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

TEST(DynamixelPacket, WritePacketSucceed)
{
    uint8_t id = 0x01, instruction = 0x02;
    uint16_t parameter_size = 0x0004;
    uint8_t parameter[] = {
        0x84, 0x00, 0x04, 0x00
    };
    int result;
    uint8_t error;
    size_t status_parameter_size;
    uint8_t status_parameter[100] = {0};

    int expected_packet_size;
    uint8_t expected_packet[100] = {0};

    expected_packet_size = create_uart_packet(
        expected_packet,
        id, instruction, parameter, parameter_size
    );

    mock().expectOneCall("pico_uart_write_blocking")
        .withPointerParameter("uart_id", uart_dummy)
        .withMemoryBufferParameter("src", expected_packet, expected_packet_size)
        .withUnsignedIntParameter("len", expected_packet_size)
        .andReturnValue(0);

    result = dynamixel_write_uart_packet(
        dynamixel_id,
        id, instruction, parameter_size, parameter
    );

    LONGS_EQUAL(0, result);
    mock().checkExpectations();
}

TEST(DynamixelPacket, SendPacketSucceed)
{
    uint8_t id = 0x01, instruction = 0x02;
    uint16_t parameter_size = 0x0004;
    uint8_t parameter[] = {
        0x84, 0x00, 0x04, 0x00
    };
    int result;
    uint8_t error;
    size_t status_parameter_size;
    uint8_t status_parameter[100] = {0};

    int expected_packet_size;
    uint8_t expected_packet[100] = {0};
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
    size_t expected_status_parameter_size = 4;
    uint8_t expected_status_parameter[100] = {
        0, 0, 0x5d, 0x0e, 0x00, 0x00, 0, 0
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

    result = dynamixel_send_packet(
        dynamixel_id,
        id, instruction, parameter_size, parameter,
        &error, &status_parameter_size, status_parameter + 2, 0
    );

    LONGS_EQUAL(DYNAMIXEL_PARSE_SUCCESS, result);
    UNSIGNED_LONGS_EQUAL(expected_error,error);
    UNSIGNED_LONGS_EQUAL(expected_status_parameter_size, status_parameter_size);
    for (int i = 0; i < expected_status_parameter_size + 4; i++)
    {
        UNSIGNED_LONGS_EQUAL(expected_status_parameter[i], status_parameter[i]);
    }
    mock().checkExpectations();
}

TEST(DynamixelPacket, SendPacketSucceedLongWait)
{
    uint8_t id = 0x01, instruction = 0x02;
    uint16_t parameter_size = 0x0004;
    uint8_t parameter[] = {
        0x84, 0x00, 0x04, 0x00
    };
    int result;
    uint8_t error;
    size_t status_parameter_size;
    uint8_t status_parameter[100] = {0};

    int expected_packet_size;
    uint8_t expected_packet[100] = {0};
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
    size_t expected_status_parameter_size = 4;
    uint8_t expected_status_parameter[100] = {
        0, 0, 0x5d, 0x0e, 0x00, 0x00, 0, 0
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
        .withUnsignedIntParameter("us", 800)
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

    result = dynamixel_send_packet(
        dynamixel_id,
        id, instruction, parameter_size, parameter,
        &error, &status_parameter_size, status_parameter + 2, 80
    );

    LONGS_EQUAL(DYNAMIXEL_PARSE_SUCCESS, result);
    UNSIGNED_LONGS_EQUAL(expected_error,error);
    UNSIGNED_LONGS_EQUAL(expected_status_parameter_size, status_parameter_size);
    for (int i = 0; i < expected_status_parameter_size + 4; i++)
    {
        UNSIGNED_LONGS_EQUAL(expected_status_parameter[i], status_parameter[i]);
    }
    mock().checkExpectations();
}

// ステータスパケットが複数に分割されて受信した場合でも読み取れる
TEST(DynamixelPacket, ReadAndParsePacketSucceedWithMultipleReadTwoTimes)
{
    int result;
    uint8_t error, id = 0x01;
    size_t status_parameter_size;
    uint8_t status_parameter[100] = {0};

    size_t expected_output_size = 15;
    uint8_t expected_output[] = {
        0,
        0xff, 0xff, 0xfd, 0x00,
        0x01,
        0x08, 0x00,
        0x55,
        0x00,
        0x5d, 0x0e, 0x00, 0x00,
        0x7c, 0x9c
    };

    uint8_t expected_error = 0;
    size_t expected_status_parameter_size = 4;
    uint8_t expected_status_parameter[100] = {
        0, 0, 0x5d, 0x0e, 0x00, 0x00, 0, 0
    };

    mock().expectNCalls(2, "pico_uart_is_readable_within_us")
        .withPointerParameter("uart_id", uart_dummy)
        .withUnsignedIntParameter("us", 10)
        .andReturnValue(0);
    mock().expectNCalls(44, "pico_uart_read_raw")
        .withPointerParameter("uart_id", uart_dummy)
        .withOutputParameterReturning("dst", expected_output, 1)
        .andReturnValue(0);
    for (int i = 0; i < expected_output_size; i++)
    {
        mock().expectOneCall("pico_uart_read_raw")
            .withPointerParameter("uart_id", uart_dummy)
            .withOutputParameterReturning("dst", expected_output + i + 1, 1)
            .andReturnValue(0);
    }
    // FIFOにこれ以上のデータなし
    mock().expectOneCall("pico_uart_read_raw")
        .withPointerParameter("uart_id", uart_dummy)
        .withOutputParameterReturning("dst", NULL, 0)
        .andReturnValue(1);

    result = dynamixel_read_uart_packet(
        dynamixel_id, id,
        &error, &status_parameter_size, status_parameter + 2, 0
    );

    LONGS_EQUAL(DYNAMIXEL_PARSE_SUCCESS, result);
    UNSIGNED_LONGS_EQUAL(expected_error,error);
    UNSIGNED_LONGS_EQUAL(expected_status_parameter_size, status_parameter_size);
    for (int i = 0; i < expected_status_parameter_size + 4; i++)
    {
        UNSIGNED_LONGS_EQUAL(expected_status_parameter[i], status_parameter[i]);
    }
    mock().checkExpectations();
}

// ステータスパケットが複数に分割されて受信した場合でも読み取れる
TEST(DynamixelPacket, ReadAndParsePacketSucceedWithMultipleReadThreeTimes)
{
    int result;
    uint8_t error, id = 0x01;
    size_t status_parameter_size;
    uint8_t status_parameter[100] = {0};

    size_t expected_output_size = 59;
    uint8_t expected_output[] = {
        0,
        0xff, 0xff, 0xfd, 0x00,
        0x01,
        0x34, 0x00,
        0x55,
        0x00,
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
        0x10, 0x11, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
        0x87, 0x92
    };

    uint8_t expected_error = 0;
    size_t expected_status_parameter_size = 48;
    uint8_t expected_status_parameter[100] = {
        0, 0,
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
        0x10, 0x11, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
        0, 0
    };

    mock().expectNCalls(3, "pico_uart_is_readable_within_us")
        .withPointerParameter("uart_id", uart_dummy)
        .withUnsignedIntParameter("us", 10)
        .andReturnValue(0);
    mock().expectNCalls(45, "pico_uart_read_raw")
        .withPointerParameter("uart_id", uart_dummy)
        .withOutputParameterReturning("dst", expected_output, 1)
        .andReturnValue(0);
    for (int i = 0; i < expected_output_size; i++)
    {
        mock().expectOneCall("pico_uart_read_raw")
            .withPointerParameter("uart_id", uart_dummy)
            .withOutputParameterReturning("dst", expected_output + i + 1, 1)
            .andReturnValue(0);
    }
    // FIFOにこれ以上のデータなし
    mock().expectOneCall("pico_uart_read_raw")
        .withPointerParameter("uart_id", uart_dummy)
        .withOutputParameterReturning("dst", NULL, 0)
        .andReturnValue(1);

    result = dynamixel_read_uart_packet(
        dynamixel_id, id,
        &error, &status_parameter_size, status_parameter + 2, 0
    );

    LONGS_EQUAL(DYNAMIXEL_PARSE_SUCCESS, result);
    UNSIGNED_LONGS_EQUAL(expected_error, error);
    UNSIGNED_LONGS_EQUAL(expected_status_parameter_size, status_parameter_size);
    for (int i = 0; i < expected_status_parameter_size + 4; i++)
    {
        UNSIGNED_LONGS_EQUAL(expected_status_parameter[i], status_parameter[i]);
    }
    mock().checkExpectations();
}

// ステータスパケットが複数に分割されて受信した場合でも読み取れる
TEST(DynamixelPacket, ReadAndParsePacketSucceedWithMultipleReadTNotFoundHeaderAtFirstRead)
{
    int result;
    uint8_t error, id = 0x01;
    size_t status_parameter_size;
    uint8_t status_parameter[100] = {0};

    size_t expected_output_size = 15;
    uint8_t expected_output[] = {
        0,
        0xff, 0xff, 0xfd, 0x00,
        0x01,
        0x08, 0x00,
        0x55,
        0x00,
        0x5d, 0x0e, 0x00, 0x00,
        0x7c, 0x9c
    };

    uint8_t expected_error = 0;
    size_t expected_status_parameter_size = 4;
    uint8_t expected_status_parameter[100] = {
        0, 0, 0x5d, 0x0e, 0x00, 0x00, 0, 0
    };

    mock().expectNCalls(2, "pico_uart_is_readable_within_us")
        .withPointerParameter("uart_id", uart_dummy)
        .withUnsignedIntParameter("us", 10)
        .andReturnValue(0);
    mock().expectNCalls(47, "pico_uart_read_raw")
        .withPointerParameter("uart_id", uart_dummy)
        .withOutputParameterReturning("dst", expected_output, 1)
        .andReturnValue(0);
    for (int i = 0; i < expected_output_size; i++)
    {
        mock().expectOneCall("pico_uart_read_raw")
            .withPointerParameter("uart_id", uart_dummy)
            .withOutputParameterReturning("dst", expected_output + i + 1, 1)
            .andReturnValue(0);
    }
    // FIFOにこれ以上のデータなし
    mock().expectOneCall("pico_uart_read_raw")
        .withPointerParameter("uart_id", uart_dummy)
        .withOutputParameterReturning("dst", NULL, 0)
        .andReturnValue(1);

    result = dynamixel_read_uart_packet(
        dynamixel_id, id,
        &error, &status_parameter_size, status_parameter + 2, 0
    );

    LONGS_EQUAL(DYNAMIXEL_PARSE_SUCCESS, result);
    UNSIGNED_LONGS_EQUAL(expected_error,error);
    UNSIGNED_LONGS_EQUAL(expected_status_parameter_size, status_parameter_size);
    for (int i = 0; i < expected_status_parameter_size + 4; i++)
    {
        UNSIGNED_LONGS_EQUAL(expected_status_parameter[i], status_parameter[i]);
    }
    mock().checkExpectations();
}

// ステータスパケットが複数に分割されて受信した場合でも読み取れる(1文字ごとに一瞬読み取り不可になる)
TEST(DynamixelPacket, ReadAndParsePacketSucceedWithMultipleReadWithTemporarilyUnavailablePacket)
{
    int result;
    uint8_t error, id = 0x01;
    size_t status_parameter_size;
    uint8_t status_parameter[100] = {0};

    size_t expected_output_size = 11;
    uint8_t expected_output[] = {
        0,
        0xff, 0xff, 0xfd, 0x00,
        0x01,
        0x04, 0x00,
        0x55,
        0x00,
        0xa1, 0x0c
    };

    uint8_t expected_error = 0;
    size_t expected_status_parameter_size = 0;
    uint8_t expected_status_parameter[100] = {
        0, 0, 0, 0
    };

    mock().expectNCalls(expected_output_size, "pico_uart_is_readable_within_us")
        .withPointerParameter("uart_id", uart_dummy)
        .withUnsignedIntParameter("us", 10)
        .andReturnValue(0);
    for (int i = 0; i < expected_output_size; i++)
    {
        mock().expectOneCall("pico_uart_read_raw")
            .withPointerParameter("uart_id", uart_dummy)
            .withOutputParameterReturning("dst", expected_output + i + 1, 1)
            .andReturnValue(0);
        mock().expectOneCall("pico_uart_read_raw")
            .withPointerParameter("uart_id", uart_dummy)
            .withOutputParameterReturning("dst", NULL, 0)
            .andReturnValue(1);
    }
    // mock().expectNCalls(1, "pico_uart_is_readable_within_us")
    //     .withPointerParameter("uart_id", uart_dummy)
    //     .withUnsignedIntParameter("us", 10)
    //     .andReturnValue(1);
    mock().ignoreOtherCalls();

    result = dynamixel_read_uart_packet(
        dynamixel_id, id,
        &error, &status_parameter_size, status_parameter + 2, 0
    );

    LONGS_EQUAL(DYNAMIXEL_PARSE_SUCCESS, result);
    UNSIGNED_LONGS_EQUAL(expected_error,error);
    UNSIGNED_LONGS_EQUAL(expected_status_parameter_size, status_parameter_size);
    for (int i = 0; i < expected_status_parameter_size + 4; i++)
    {
        UNSIGNED_LONGS_EQUAL(expected_status_parameter[i], status_parameter[i]);
    }
    mock().checkExpectations();
}

TEST(DynamixelPacket, ReadAndParsePacketWrongChecksumError)
{
    int result;
    uint8_t error, id = 0x01;
    size_t status_parameter_size;
    uint8_t status_parameter[100] = {0};

    size_t expected_output_size = 15;
    uint8_t expected_output[] = {
        0xff, 0xff, 0xfd, 0x00,
        0x01,
        0x08, 0x00,
        0x55,
        0x00,
        0x5d, 0x0e, 0x00, 0x00,
        0x7c, 0x9b
    };
    uint8_t expected_error = 0;
    size_t expected_status_parameter_size = 4;
    uint8_t expected_status_parameter[100] = {
        0, 0, 0x5d, 0x0e, 0x00, 0x00, 0, 0
    };

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

    result = dynamixel_read_uart_packet(
        dynamixel_id, id,
        &error, &status_parameter_size, status_parameter + 2, 0
    );

    LONGS_EQUAL(DYNAMIXEL_PARSE_WRONG_CHECKSUM, result);
    UNSIGNED_LONGS_EQUAL(expected_error,error);
    UNSIGNED_LONGS_EQUAL(expected_status_parameter_size, status_parameter_size);
    for (int i = 0; i < expected_status_parameter_size + 4; i++)
    {
        UNSIGNED_LONGS_EQUAL(expected_status_parameter[i], status_parameter[i]);
    }
    mock().checkExpectations();
}

TEST(DynamixelPacket, ReadAndParsePacketStatusError)
{
    int result;
    uint8_t error, id = 0x01;
    size_t status_parameter_size;
    uint8_t status_parameter[100] = {0};

    int expected_packet_size;
    uint8_t expected_packet[100] = {0};
    size_t expected_output_size = 15;
    uint8_t expected_output[] = {
        0xff, 0xff, 0xfd, 0x00,
        0x01,
        0x08, 0x00,
        0x55,
        0x01,
        0x5d, 0x0e, 0x00, 0x00,
        0x07, 0x1c
    };
    uint8_t expected_error = 1;
    size_t expected_status_parameter_size = 4;
    uint8_t expected_status_parameter[100] = {
        0, 0, 0x5d, 0x0e, 0x00, 0x00, 0, 0
    };

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

    result = dynamixel_read_uart_packet(
        dynamixel_id, id,
        &error, &status_parameter_size, status_parameter + 2, 0
    );

    LONGS_EQUAL(DYNAMIXEL_PARSE_STATUS_ERROR, result);
    UNSIGNED_LONGS_EQUAL(expected_error,error);
    UNSIGNED_LONGS_EQUAL(expected_status_parameter_size, status_parameter_size);
    for (int i = 0; i < expected_status_parameter_size + 4; i++)
    {
        UNSIGNED_LONGS_EQUAL(expected_status_parameter[i], status_parameter[i]);
    }
    mock().checkExpectations();
}

TEST(DynamixelPacket, ReadAndParsePacketInadequateResponseErrorWithoutFindingHeader)
{
    int result;
    uint8_t error, id = 0x01;
    size_t status_parameter_size;
    uint8_t status_parameter[100] = {0};

    size_t expected_output_size = 3;
    uint8_t expected_output[] = {
        0, 0xff, 0xff, 0xa1
    };

    mock().expectNCalls(2, "pico_uart_is_readable_within_us")
        .withPointerParameter("uart_id", uart_dummy)
        .withUnsignedIntParameter("us", 10)
        .andReturnValue(0);
    mock().expectOneCall("pico_uart_is_readable_within_us")
        .withPointerParameter("uart_id", uart_dummy)
        .withUnsignedIntParameter("us", 10)
        .andReturnValue(1);
    mock().expectNCalls(48, "pico_uart_read_raw")
        .withPointerParameter("uart_id", uart_dummy)
        .withOutputParameterReturning("dst", expected_output, 1)
        .andReturnValue(0);
    for (int i = 0; i < expected_output_size; i++)
    {
        mock().expectOneCall("pico_uart_read_raw")
            .withPointerParameter("uart_id", uart_dummy)
            .withOutputParameterReturning("dst", expected_output + i + 1, 1)
            .andReturnValue(0);
    }
    // FIFOにこれ以上のデータなし
    mock().expectOneCall("pico_uart_read_raw")
        .withPointerParameter("uart_id", uart_dummy)
        .withOutputParameterReturning("dst", NULL, 0)
        .andReturnValue(1);

    result = dynamixel_read_uart_packet(
        dynamixel_id, id,
        &error, &status_parameter_size, status_parameter, 0
    );

    LONGS_EQUAL(DYNAMIXEL_PARSE_INADEQUATE_DATA, result);
    mock().checkExpectations();
}

TEST(DynamixelPacket, ReadAndParsePacketInadequateResponseErrorWithFindingHeader)
{
    int result;
    uint8_t error, id = 0x01;
    size_t status_parameter_size;
    uint8_t status_parameter[100] = {0};

    size_t expected_output_size = 7;
    uint8_t expected_output[] = {
        0, 0xff, 0xff, 0xfd, 0x00, 0x04, 0x00, 0x55
    };

    mock().expectNCalls(2, "pico_uart_is_readable_within_us")
        .withPointerParameter("uart_id", uart_dummy)
        .withUnsignedIntParameter("us", 10)
        .andReturnValue(0);
    mock().expectOneCall("pico_uart_is_readable_within_us")
        .withPointerParameter("uart_id", uart_dummy)
        .withUnsignedIntParameter("us", 10)
        .andReturnValue(1);
    mock().expectNCalls(45, "pico_uart_read_raw")
        .withPointerParameter("uart_id", uart_dummy)
        .withOutputParameterReturning("dst", expected_output, 1)
        .andReturnValue(0);
    for (int i = 0; i < expected_output_size; i++)
    {
        mock().expectOneCall("pico_uart_read_raw")
            .withPointerParameter("uart_id", uart_dummy)
            .withOutputParameterReturning("dst", expected_output + i + 1, 1)
            .andReturnValue(0);
    }
    // FIFOにこれ以上のデータなし
    mock().expectOneCall("pico_uart_read_raw")
        .withPointerParameter("uart_id", uart_dummy)
        .withOutputParameterReturning("dst", NULL, 0)
        .andReturnValue(1);

    result = dynamixel_read_uart_packet(
        dynamixel_id, id,
        &error, &status_parameter_size, status_parameter, 0
    );

    LONGS_EQUAL(DYNAMIXEL_PARSE_INADEQUATE_DATA, result);
    mock().checkExpectations();
}

TEST(DynamixelPacket, ReadAndParsePacketInadequateResponseErrorWithFewResponse)
{
    int result;
    uint8_t error, id = 0x01;
    size_t status_parameter_size;
    uint8_t status_parameter[100] = {0};

    uint8_t expected_output[] = {
        0xa1
    };

    mock().expectOneCall("pico_uart_is_readable_within_us")
        .withPointerParameter("uart_id", uart_dummy)
        .withUnsignedIntParameter("us", 10)
        .andReturnValue(0);
    mock().expectOneCall("pico_uart_is_readable_within_us")
        .withPointerParameter("uart_id", uart_dummy)
        .withUnsignedIntParameter("us", 10)
        .andReturnValue(1);
    mock().expectOneCall("pico_uart_read_raw")
        .withPointerParameter("uart_id", uart_dummy)
        .withOutputParameterReturning("dst", expected_output, 1)
        .andReturnValue(0);
    // FIFOにこれ以上のデータなし
    mock().expectOneCall("pico_uart_read_raw")
        .withPointerParameter("uart_id", uart_dummy)
        .withOutputParameterReturning("dst", NULL, 0)
        .andReturnValue(1);

    result = dynamixel_read_uart_packet(
        dynamixel_id, id,
        &error, &status_parameter_size, status_parameter, 0
    );

    LONGS_EQUAL(DYNAMIXEL_PARSE_INADEQUATE_DATA, result);
    mock().checkExpectations();
}

TEST(DynamixelPacket, ReadAndParsePacketWithHugeResponse)
{
    int result;
    uint8_t error, id = 0x01;
    size_t status_parameter_size;
    uint8_t status_parameter[100] = {0};

    int expected_packet_size;
    uint8_t expected_packet[100] = {0};
    uint8_t expected_output[] = {
        0,
        0xff, 0xff, 0xfd, 0x00,
        0x01,
        0x72, 0x00,
        0x55,
        0x00,
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
        0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
        0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
        0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
        0xd8, 0x8b
    };

    mock().expectNCalls(3, "pico_uart_is_readable_within_us")
        .withPointerParameter("uart_id", uart_dummy)
        .withUnsignedIntParameter("us", 10)
        .andReturnValue(0);
    mock().expectNCalls(1, "pico_uart_is_readable")
        .withPointerParameter("uart_id", uart_dummy)
        .andReturnValue(0);
    mock().expectNCalls(45, "pico_uart_read_raw")
        .withPointerParameter("uart_id", uart_dummy)
        .withOutputParameterReturning("dst", expected_output, 1)
        .andReturnValue(0);
    for (int i = 0; i < 100; i++)
    {
        mock().expectOneCall("pico_uart_read_raw")
            .withPointerParameter("uart_id", uart_dummy)
            .withOutputParameterReturning("dst", expected_output + i + 1, 1)
            .andReturnValue(0);
    }

    result = dynamixel_read_uart_packet(
        dynamixel_id, id,
        &error, &status_parameter_size, status_parameter + 2, 0
    );

    LONGS_EQUAL(DYNAMIXEL_PARSE_HUGE_DATA, result);
    mock().checkExpectations();
}

TEST(DynamixelPacket, ReadAndParsePacketWithWrongID)
{
    int result;
    uint8_t error, id = 0x01;
    size_t status_parameter_size;
    uint8_t status_parameter[100] = {0};

    size_t expected_output_size = 14;
    uint8_t expected_output[] = {
        0xff, 0xff, 0xfd, 0x00,
        0x02,
        0x07, 0x00,
        0x55,
        0x00,
        0x06, 0x04,
        0x26,
        0x6f, 0x6d
    };
    uint8_t expected_error = 0;
    size_t expected_status_parameter_size = 3;
    uint8_t expected_status_parameter[100] = {
        0, 0, 0x06, 0x04, 0x26, 0, 0
    };

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

    result = dynamixel_read_uart_packet(
        dynamixel_id, id,
        &error, &status_parameter_size, status_parameter + 2, 0
    );

    LONGS_EQUAL(DYNAMIXEL_PARSE_WRONG_ID, result);
    UNSIGNED_LONGS_EQUAL(expected_error, error);
    UNSIGNED_LONGS_EQUAL(expected_status_parameter_size, status_parameter_size);
    for (int i = 0; i < expected_status_parameter_size + 4; i++)
    {
        UNSIGNED_LONGS_EQUAL(expected_status_parameter[i], status_parameter[i]);
    }
    mock().checkExpectations();
}

TEST(DynamixelPacket, ReadAndParsePacketTimeoutError)
{
    int result;
    uint8_t error, id = 0x01;
    size_t status_parameter_size;
    uint8_t status_parameter[100] = {0};

    mock().expectOneCall("pico_uart_is_readable_within_us")
        .withPointerParameter("uart_id", uart_dummy)
        .withUnsignedIntParameter("us", 10)
        .andReturnValue(1);
    mock().ignoreOtherCalls();

    result = dynamixel_read_uart_packet(
        dynamixel_id, id,
        &error, &status_parameter_size, status_parameter + 2, 0
    );

    LONGS_EQUAL(DYNAMIXEL_PARSE_NO_RESPONSE, result);
    mock().checkExpectations();
}

TEST(DynamixelPacket, ReadPacketSucceedLongWaite)
{
    uint8_t id = 0x01;
    int result;
    uint8_t error;
    size_t status_parameter_size;
    uint8_t status_parameter[100] = {0};

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
    size_t expected_status_parameter_size = 4;
    uint8_t expected_status_parameter[100] = {
        0, 0, 0x5d, 0x0e, 0x00, 0x00, 0, 0
    };

    mock().expectOneCall("pico_uart_is_readable_within_us")
        .withPointerParameter("uart_id", uart_dummy)
        .withUnsignedIntParameter("us", 90)
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

    result = dynamixel_read_uart_packet(
        dynamixel_id, id,
        &error, &status_parameter_size, status_parameter + 2, 9
    );

    LONGS_EQUAL(DYNAMIXEL_PARSE_SUCCESS, result);
    UNSIGNED_LONGS_EQUAL(expected_error,error);
    UNSIGNED_LONGS_EQUAL(expected_status_parameter_size, status_parameter_size);
    for (int i = 0; i < expected_status_parameter_size + 4; i++)
    {
        UNSIGNED_LONGS_EQUAL(expected_status_parameter[i], status_parameter[i]);
    }
    mock().checkExpectations();
}

// TODO: ID=0xfeのときのパケット解析
