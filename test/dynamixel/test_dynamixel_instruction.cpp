#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "dynamixel/dynamixel.h"
#include "util/analyze_packet.h"
#include "util/packet_byte.h"


TEST_GROUP(DynamixelInstruction)
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

TEST(DynamixelInstruction, SendPingSucceed)
{
    uint8_t id = 0x01, instruction = 0x01;
    int result;
    uint8_t error, dynamixel_version_of_firmware;
    uint16_t dynamixel_model_no;
    size_t status_parameter_size;
    uint8_t status_parameter[100] = {0};

    int expected_packet_size;
    uint8_t expected_packet[100] = {0};
    size_t expected_output_size = 14;
    uint8_t expected_output[] = {
        0xff, 0xff, 0xfd, 0x00,
        0x01,
        0x07, 0x00,
        0x55,
        0x00,
        0x06, 0x04,
        0x26,
        0x65, 0x5d
    };
    uint8_t expected_error = 0;
    uint8_t expected_dynamixel_version_of_firmware = 0x26;
    uint16_t expected_dynamixel_model_no = 0x0406;

    expected_packet_size = create_uart_packet(
        expected_packet,
        id, instruction, NULL, 0
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

    result = dynamixel_send_ping(
        dynamixel_id, id,
        &error, &dynamixel_model_no, &dynamixel_version_of_firmware,
        0
    );

    LONGS_EQUAL(DYNAMIXEL_PARSE_SUCCESS, result);
    UNSIGNED_LONGS_EQUAL(expected_error,error);
    UNSIGNED_LONGS_EQUAL(expected_dynamixel_model_no, dynamixel_model_no);
    UNSIGNED_LONGS_EQUAL(expected_dynamixel_version_of_firmware, dynamixel_version_of_firmware);
    mock().checkExpectations();
}

TEST(DynamixelInstruction, SendPingWaitLong)
{
    uint8_t id = 0x01, instruction = 0x01;
    int result;
    uint8_t error, dynamixel_version_of_firmware;
    uint16_t dynamixel_model_no;
    size_t status_parameter_size;
    uint8_t status_parameter[100] = {0};

    int expected_packet_size;
    uint8_t expected_packet[100] = {0};
    size_t expected_output_size = 14;
    uint8_t expected_output[] = {
        0xff, 0xff, 0xfd, 0x00,
        0x01,
        0x07, 0x00,
        0x55,
        0x00,
        0x06, 0x04,
        0x26,
        0x65, 0x5d
    };
    uint8_t expected_error = 0;
    uint8_t expected_dynamixel_version_of_firmware = 0x26;
    uint16_t expected_dynamixel_model_no = 0x0406;

    expected_packet_size = create_uart_packet(
        expected_packet,
        id, instruction, NULL, 0
    );

    mock().expectOneCall("pico_uart_write_blocking")
        .withPointerParameter("uart_id", uart_dummy)
        .withMemoryBufferParameter("src", expected_packet, expected_packet_size)
        .withUnsignedIntParameter("len", expected_packet_size);
    mock().expectOneCall("pico_uart_is_readable_within_us")
        .withPointerParameter("uart_id", uart_dummy)
        .withUnsignedIntParameter("us", 1000)
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

    result = dynamixel_send_ping(
        dynamixel_id, id,
        &error, &dynamixel_model_no, &dynamixel_version_of_firmware,
        100
    );

    LONGS_EQUAL(DYNAMIXEL_PARSE_SUCCESS, result);
    UNSIGNED_LONGS_EQUAL(expected_error,error);
    UNSIGNED_LONGS_EQUAL(expected_dynamixel_model_no, dynamixel_model_no);
    UNSIGNED_LONGS_EQUAL(expected_dynamixel_version_of_firmware, dynamixel_version_of_firmware);
    mock().checkExpectations();
}

TEST(DynamixelInstruction, SendPingWithWrongParameter)
{
    uint8_t id = 0x01;
    int result;
    uint8_t error, dynamixel_version_of_firmware;
    uint16_t dynamixel_model_no;
    size_t status_parameter_size;
    uint8_t status_parameter[100] = {0};

    size_t expected_output_size = 12;
    uint8_t expected_output[] = {
        0xff, 0xff, 0xfd, 0x00,
        0x01,
        0x05, 0x00,
        0x55,
        0x00,
        0x26,
        0x84, 0xa1
    };
    uint8_t expected_error = 0;

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

    result = dynamixel_send_ping(
        dynamixel_id, id,
        &error, &dynamixel_model_no, &dynamixel_version_of_firmware,
        0
    );

    LONGS_EQUAL(DYNAMIXEL_PARSE_WRONG_PARAMETER, result);
    UNSIGNED_LONGS_EQUAL(expected_error,error);
    mock().checkExpectations();
}

TEST(DynamixelInstruction, SendReadOnceSucceed)
{
    uint8_t id = 0x01, instruction = 0x02, error;
    uint16_t parameter_size = 0x0004, data_size = 0x0004, start_address = 0x0084;
    uint8_t parameter[] = {
        0x84, 0x00, 0x04, 0x00
    };
    uint8_t data[100] = {0};
    int result;
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
    uint8_t expected_data[8] = {
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

    result = dynamixel_send_read_once(
        dynamixel_id, id, start_address, data_size,
        &error, data + 2, 0
    );

    LONGS_EQUAL(DYNAMIXEL_PARSE_SUCCESS, result);
    UNSIGNED_LONGS_EQUAL(expected_error,error);
    for (size_t i = 0; i < data_size + 4; i++)
        UNSIGNED_LONGS_EQUAL(expected_data[i], data[i]);
    mock().checkExpectations();
}

TEST(DynamixelInstruction, SendReadOnceWaitLong)
{
    uint8_t id = 0x01, instruction = 0x02, error;
    uint16_t parameter_size = 0x0004, data_size = 0x0004, start_address = 0x0084;
    uint8_t parameter[] = {
        0x84, 0x00, 0x04, 0x00
    };
    uint8_t data[100] = {0};
    int result;
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
    uint8_t expected_data[8] = {
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
        .withUnsignedIntParameter("us", 980)
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

    result = dynamixel_send_read_once(
        dynamixel_id, id, start_address, data_size,
        &error, data + 2, 98
    );

    LONGS_EQUAL(DYNAMIXEL_PARSE_SUCCESS, result);
    UNSIGNED_LONGS_EQUAL(expected_error,error);
    for (size_t i = 0; i < data_size + 4; i++)
        UNSIGNED_LONGS_EQUAL(expected_data[i], data[i]);
    mock().checkExpectations();
}

TEST(DynamixelInstruction, SendReadOnceWithWrongParameter)
{
    uint8_t id = 0x01, instruction = 0x02, error;
    uint16_t parameter_size = 0x0004, data_size = 0x0004, start_address = 0x0084;
    uint8_t parameter[] = {
        0x84, 0x00, 0x04, 0x00
    };
    uint8_t data[100] = {0};
    int result;
    size_t status_parameter_size;
    uint8_t status_parameter[100] = {0};

    int expected_packet_size;
    uint8_t expected_packet[100] = {0};
    size_t expected_output_size = 16;
    uint8_t expected_output[] = {
        0xff, 0xff, 0xfd, 0x00,
        0x01,
        0x09, 0x00,
        0x55,
        0x00,
        0x5d, 0x0e, 0x00, 0x00, 0x01,
        0x2e, 0x79
    };
    uint8_t expected_error = 0;
    uint8_t expected_data[9] = {
        0, 0, 0x5d, 0x0e, 0x00, 0x00, 0x01, 0, 0
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

    result = dynamixel_send_read_once(
        dynamixel_id, id, start_address, data_size,
        &error, data + 2, 0
    );

    LONGS_EQUAL(DYNAMIXEL_PARSE_WRONG_PARAMETER, result);
    UNSIGNED_LONGS_EQUAL(expected_error, error);
    for (size_t i = 0; i < 9; i++)
        UNSIGNED_LONGS_EQUAL(expected_data[i], data[i]);
    mock().checkExpectations();
}

TEST(DynamixelInstruction, SendReadSucceedWithRetry)
{
    uint8_t id = 0x01, instruction = 0x02, error;
    uint16_t parameter_size = 0x0004, data_size = 0x0004, start_address = 0x0084;
    uint8_t parameter[] = {
        0x84, 0x00, 0x04, 0x00
    };
    uint8_t data[100] = {0};
    int result;
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
    uint8_t expected_data[8] = {
        0, 0, 0x5d, 0x0e, 0x00, 0x00, 0, 0
    };

    expected_packet_size = create_uart_packet(
        expected_packet,
        id, instruction, parameter, parameter_size
    );

    // 最初はレスポンスなし
    mock().expectOneCall("pico_uart_write_blocking")
        .withPointerParameter("uart_id", uart_dummy)
        .withMemoryBufferParameter("src", expected_packet, expected_packet_size)
        .withUnsignedIntParameter("len", expected_packet_size);
    mock().expectOneCall("pico_uart_is_readable_within_us")
        .withPointerParameter("uart_id", uart_dummy)
        .withUnsignedIntParameter("us", 10)
        .andReturnValue(1);

    // 再試行
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

    result = dynamixel_send_read(
        dynamixel_id, id, start_address, data_size,
        &error, data + 2, 0, 0
    );

    LONGS_EQUAL(DYNAMIXEL_PARSE_SUCCESS, result);
    UNSIGNED_LONGS_EQUAL(expected_error,error);
    for (size_t i = 0; i < data_size + 4; i++)
        UNSIGNED_LONGS_EQUAL(expected_data[i], data[i]);
    mock().checkExpectations();
}

TEST(DynamixelInstruction, SendWriteOnceSucceed)
{
    uint8_t id = 0x01, instruction = 0x03, error;
    uint16_t parameter_size = 0x0006, start_address = 0x0074, data_size = 4;
    uint8_t data[100] = {
        0xe7, 0x03, 0x00, 0x00
    };
    uint8_t parameter[] = {
        0x74, 0x00, 0xe7, 0x03, 0x00, 0x00
    };
    int result;
    size_t status_parameter_size;
    uint8_t status_parameter[100] = {0};

    int expected_packet_size;
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
    uint8_t expected_error = 0;

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

    result = dynamixel_send_write_once(
        dynamixel_id, id, start_address, data_size, data,
        &error, 0
    );

    LONGS_EQUAL(DYNAMIXEL_PARSE_SUCCESS, result);
    UNSIGNED_LONGS_EQUAL(expected_error,error);
    mock().checkExpectations();
}

TEST(DynamixelInstruction, SendWriteSucceedWithRetry)
{
    uint8_t id = 0x01, instruction = 0x03, error;
    uint16_t parameter_size = 0x0006, start_address = 0x0074, data_size = 4;
    uint8_t data[100] = {
        0xe7, 0x03, 0x00, 0x00
    };
    uint8_t parameter[] = {
        0x74, 0x00, 0xe7, 0x03, 0x00, 0x00
    };
    int result;
    size_t status_parameter_size;
    uint8_t status_parameter[100] = {0};

    int expected_packet_size;
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
    uint8_t expected_error = 0;

    expected_packet_size = create_uart_packet(
        expected_packet,
        id, instruction, parameter, parameter_size
    );

    // 最初はレスポンスなし
    mock().expectOneCall("pico_uart_write_blocking")
        .withPointerParameter("uart_id", uart_dummy)
        .withMemoryBufferParameter("src", expected_packet, expected_packet_size)
        .withUnsignedIntParameter("len", expected_packet_size);
    mock().expectOneCall("pico_uart_is_readable_within_us")
        .withPointerParameter("uart_id", uart_dummy)
        .withUnsignedIntParameter("us", 10)
        .andReturnValue(1);

    // 再試行
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

    result = dynamixel_send_write(
        dynamixel_id, id, start_address, data_size, data,
        &error, 0, 0
    );

    LONGS_EQUAL(DYNAMIXEL_PARSE_SUCCESS, result);
    UNSIGNED_LONGS_EQUAL(expected_error,error);
    mock().checkExpectations();
}

TEST(DynamixelInstruction, SendWriteOnceLongWaite)
{
    uint8_t id = 0x01, instruction = 0x03, error;
    uint16_t parameter_size = 0x0006, start_address = 0x0074, data_size = 4;
    uint8_t data[100] = {
        0xe7, 0x03, 0x00, 0x00
    };
    uint8_t parameter[] = {
        0x74, 0x00, 0xe7, 0x03, 0x00, 0x00
    };
    int result;
    size_t status_parameter_size;
    uint8_t status_parameter[100] = {0};

    int expected_packet_size;
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
    uint8_t expected_error = 0;

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
        .withUnsignedIntParameter("us", 990)
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

    result = dynamixel_send_write_once(
        dynamixel_id, id, start_address, data_size, data,
        &error, 99
    );

    LONGS_EQUAL(DYNAMIXEL_PARSE_SUCCESS, result);
    UNSIGNED_LONGS_EQUAL(expected_error,error);
    mock().checkExpectations();
}

TEST(DynamixelInstruction, SendRegWriteSucceed)
{
    uint8_t id = 0x01, instruction = 0x04, error;
    uint16_t parameter_size = 0x0006, start_address = 0x0068, data_size = 4;
    uint8_t data[100] = {
        0xc8, 0x00, 0x00, 0x00
    };
    uint8_t parameter[] = {
        0x68, 0x00, 0xc8, 0x00, 0x00, 0x00
    };
    int result;
    size_t status_parameter_size;
    uint8_t status_parameter[100] = {0};

    int expected_packet_size;
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
    uint8_t expected_error = 0;

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
        .withUnsignedIntParameter("us", 960)
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

    result = dynamixel_send_reg_write(
        dynamixel_id, id, start_address, data_size, data,
        &error, 96
    );

    LONGS_EQUAL(DYNAMIXEL_PARSE_SUCCESS, result);
    UNSIGNED_LONGS_EQUAL(expected_error,error);
    mock().checkExpectations();
}

TEST(DynamixelInstruction, SendActionSucceed)
{
    uint8_t id = 0x01, instruction = 0x05, error;
    int result;

    int expected_packet_size;
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
    uint8_t expected_error = 0;

    expected_packet_size = create_uart_packet(
        expected_packet,
        id, instruction, NULL, 0
    );

    mock().expectOneCall("pico_uart_write_blocking")
        .withPointerParameter("uart_id", uart_dummy)
        .withMemoryBufferParameter("src", expected_packet, expected_packet_size)
        .withUnsignedIntParameter("len", expected_packet_size);
    mock().expectOneCall("pico_uart_is_readable_within_us")
        .withPointerParameter("uart_id", uart_dummy)
        .withUnsignedIntParameter("us", 950)
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

    result = dynamixel_send_action(
        dynamixel_id, id, &error, 95
    );

    LONGS_EQUAL(DYNAMIXEL_PARSE_SUCCESS, result);
    UNSIGNED_LONGS_EQUAL(expected_error,error);
    mock().checkExpectations();
}

TEST(DynamixelInstruction, SendFactoryResetSucceed)
{
    uint8_t id = 0x01, instruction = 0x06, error;
    dynamixel_factory_reset_paramerter factory_reset = DYNAMIXEL_FACTORY_RESET_ID;
    uint16_t parameter_size = 1;
    uint8_t parameter[] = {
        0x01
    };
    int result;

    int expected_packet_size;
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
    uint8_t expected_error = 0;

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
        .withUnsignedIntParameter("us", 940)
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

    result = dynamixel_send_factory_reset(
        dynamixel_id, id, &error, factory_reset, 94
    );

    LONGS_EQUAL(DYNAMIXEL_PARSE_SUCCESS, result);
    UNSIGNED_LONGS_EQUAL(expected_error, error);
    mock().checkExpectations();
}

TEST(DynamixelInstruction, SendRebootSucceed)
{
    uint8_t id = 0x01, instruction = 0x08, error;
    int result;

    int expected_packet_size;
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
    uint8_t expected_error = 0;

    expected_packet_size = create_uart_packet(
        expected_packet,
        id, instruction, NULL, 0
    );

    mock().expectOneCall("pico_uart_write_blocking")
        .withPointerParameter("uart_id", uart_dummy)
        .withMemoryBufferParameter("src", expected_packet, expected_packet_size)
        .withUnsignedIntParameter("len", expected_packet_size);
    mock().expectOneCall("pico_uart_is_readable_within_us")
        .withPointerParameter("uart_id", uart_dummy)
        .withUnsignedIntParameter("us", 930)
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

    result = dynamixel_send_reboot(
        dynamixel_id, id, &error, 93
    );

    LONGS_EQUAL(DYNAMIXEL_PARSE_SUCCESS, result);
    UNSIGNED_LONGS_EQUAL(expected_error, error);
    mock().checkExpectations();
}
