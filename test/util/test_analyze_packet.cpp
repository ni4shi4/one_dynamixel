#include "CppUTest/TestHarness.h"
#include "util/analyze_packet.h"

#define PARAMETER_SIZE 100


TEST_GROUP(ANALYZE_PACKET)
{
    void setup()
    {
    }

    void teardown()
    {
    }
};

void test_parse_uart_packet_all_parameters(
    uint8_t *packet,
    int packet_size,
    uint8_t expected_id,
    uint8_t expected_instruction,
    uint8_t expected_error,
    int expected_header_position,
    int expected_parameter_size,
    int expected_result,
    const uint8_t *expected_parameter,
    int compare_parameter_size
)
{
    uint8_t id, instruction, error;
    int header_position, result;
    size_t parameter_size;
    uint8_t parameter[PARAMETER_SIZE] = {0};

    result = parse_uart_packet(
        packet, packet_size,
        &header_position, &id, &instruction, &error,
        parameter, &parameter_size
    );

    CHECK_EQUAL_TEXT(expected_header_position, header_position, "output test [header_posotion]");
    CHECK_EQUAL_TEXT(expected_id, id, "output test [id]");
    CHECK_EQUAL_TEXT(expected_instruction, instruction, "output test [instruction]");
    CHECK_EQUAL_TEXT(expected_error, error, "output test [error]");
    CHECK_EQUAL_TEXT(expected_parameter_size, parameter_size, "output test [parameter_size]");
    CHECK_EQUAL_TEXT(expected_result, result, "return test");
    for (int i = 0; i < compare_parameter_size; i++)
        CHECK_EQUAL_TEXT(
            expected_parameter[i],
            parameter[i],
            "output test [parameter]"
        );
}

void test_parse_uart_return_value(
    uint8_t *packet,
    int packet_size,
    int expected_result
)
{
    uint8_t id, instruction, error;
    int header_position, result;
    size_t parameter_size;
    uint8_t parameter[PARAMETER_SIZE] = {0};

    result = parse_uart_packet(
        packet, packet_size,
        &header_position, &id, &instruction, &error,
        parameter, &parameter_size
    );

    CHECK_EQUAL_TEXT(expected_result, result, "return test");
}

void test_parse_uart_return_value_and_position(
    uint8_t *packet,
    int packet_size,
    int expected_header_position,
    int expected_result
)
{
    uint8_t id, instruction, error;
    int header_position, result;
    size_t parameter_size;
    uint8_t parameter[PARAMETER_SIZE] = {0};

    result = parse_uart_packet(
        packet, packet_size,
        &header_position, &id, &instruction, &error,
        parameter, &parameter_size
    );

    CHECK_EQUAL_TEXT(expected_header_position, header_position, "output test [header_posotion]");
    CHECK_EQUAL_TEXT(expected_result, result, "return test");
}


TEST(ANALYZE_PACKET, test_create_uart_packet_CreateExpectedOutputForSpecificInput)
{
    uint8_t id1 = 0x01, instruction1 = 0x02;
    uint16_t parameter_size1 = 0x0004;
    uint8_t parameter1[] = {
        0x84, 0x00, 0x04, 0x00
    };
    int packet_size1;
    uint8_t packet1[20] = {0};

    int expected_packet_size1 = 14;
    uint8_t expected_packet1[] = {
        0xff, 0xff, 0xfd, 0x00,
        0x01,
        0x07, 0x00,
        0x02,
        0x84, 0x00,
        0x04, 0x00,
        0x1d, 0x15,
        0, 0, 0, 0, 0, 0 // 余計にデータを詰めて余計な処理がされていないかを確認する
    };

    packet_size1 = create_uart_packet(
        packet1,
        id1, instruction1, parameter1, parameter_size1
    );
    CHECK_EQUAL(expected_packet_size1, packet_size1);
    for (int i = 0; i < 20; i++)
        CHECK_EQUAL(
            expected_packet1[i],
            packet1[i]
        );


    uint8_t id2 = 0x01, instruction2 = 0x01;
    uint16_t parameter_size2 = 0x0000;
    uint8_t *parameter2 = NULL;
    int packet_size2;
    uint8_t packet2[16] = {0};

    int expected_packet_size2 = 10;
    uint8_t expected_packet2[] = {
        0xff, 0xff, 0xfd, 0x00,
        0x01,
        0x03, 0x00,
        0x01,
        0x19, 0x4e,
        0, 0, 0, 0, 0, 0 // 余計にデータを詰めて余計な処理がされていないかを確認する
    };

    packet_size2 = create_uart_packet(
        packet2,
        id2, instruction2, parameter2, parameter_size2
    );
    CHECK_EQUAL(expected_packet_size2, packet_size2);
    for (int i = 0; i < 16; i++)
        CHECK_EQUAL(
            expected_packet2[i],
            packet2[i]
        );


    // 例外処理をテスト
    uint8_t id3 = 0x01, instruction3 = 0x03;
    uint16_t parameter_size3 = 0x000c;
    uint8_t parameter3[] = {
        0x7a, 0x02, 0xff, 0xff, 0xfd,
        0xff, 0xff, 0xfd, 0xff, 0xff, 0xfd, 0xff
    };
    int packet_size3;
    uint8_t packet3[30] = {0};

    int expected_packet_size3 = 25;
    uint8_t expected_packet3[] = {
        0xff, 0xff, 0xfd, 0x00,
        0x01,
        0x12, 0x00,
        0x03,
        0x7a, 0x02, 0xff, 0xff, 0xfd, 0xfd,
        0xff, 0xff, 0xfd, 0xfd, 0xff, 0xff, 0xfd, 0xfd, 0xff,
        0xa3, 0xe2,
        0, 0, 0, 0, 0 // 余計にデータを詰めて余計な処理がされていないかを確認する
    };

    packet_size3 = create_uart_packet(
        packet3,
        id3, instruction3, parameter3, parameter_size3
    );
    CHECK_EQUAL(expected_packet_size3, packet_size3);
    for (int i = 0; i < 30; i++)
        CHECK_EQUAL(
            expected_packet3[i],
            packet3[i]
        );
}


TEST(ANALYZE_PACKET, create_uart_packet_CreateExpectedOutputForParameterSize1)
{
    uint8_t id1 = 0x01, instruction1 = 0x06;
    uint16_t parameter_size1 = 0x0001;
    uint8_t parameter1[] = {
        0xff
    };
    int packet_size1;
    uint8_t packet1[20] = {0};

    int expected_packet_size1 = 11;
    uint8_t expected_packet1[] = {
        0, 0,
        0xff, 0xff, 0xfd, 0x00,
        0x01,
        0x04, 0x00,
        0x06,
        0xff,
        0xa6, 0x64,
        0, 0, 0, 0, 0, 0, 0 // 余計にデータを詰めて余計な処理がされていないかを確認する
    };

    packet_size1 = create_uart_packet(
        packet1 + 2,
        id1, instruction1, parameter1, parameter_size1
    );
    CHECK_EQUAL(expected_packet_size1, packet_size1);
    for (int i = 0; i < 20; i++)
        CHECK_EQUAL(
            expected_packet1[i],
            packet1[i]
        );
}


TEST(ANALYZE_PACKET, test_parse_uart_packet_CreateExpectedOutputForSpecificInput)
{
    int packet_size1 = 20;
    uint8_t packet1[] = {
        0, 0,
        0xff, 0xff, 0xfd, 0x00,
        0x01,
        0x07, 0x00,
        0x55,
        0x00,
        0x06, 0x04,
        0x26,
        0x65, 0x5d,
        0, 0, 0, 0
    };

    uint8_t expected_id1 = 0x01, expected_instruction1 = 0x55, expected_error1 = 0x00;
    int expected_header_position1 = 2, expected_parameter_size1 = 3, expected_result1 = 0;
    uint8_t expected_parameter1[] = {
        0x06, 0x04, 0x26, 0, 0
    };

    test_parse_uart_packet_all_parameters(
        packet1, packet_size1,
        expected_id1, expected_instruction1, expected_error1,
        expected_header_position1, expected_parameter_size1, expected_result1,
        expected_parameter1, 5
    );


    int packet_size2 = 30;
    uint8_t packet2[] = {
        0, 0, 0, 0,
        0xff, 0xff, 0xfd, 0x00,
        0x01,
        0x11, 0x00,
        0x55,
        0x00,
        0xff, 0xff, 0xfd, 0xfd, 0xff, 0xff, 0xfd, 0xfd,
        0xff, 0xff, 0xfd, 0xfd, 0xff,
        0x18, 0x99,
        0, 0
    };

    uint8_t expected_id2 = 0x01, expected_instruction2 = 0x55, expected_error2 = 0x00;
    int expected_header_position2 = 4, expected_parameter_size2 = 10, expected_result2 = 0;
    uint8_t expected_parameter2[] = {
        0xff, 0xff, 0xfd, 0xff, 0xff, 0xfd,
        0xff, 0xff, 0xfd, 0xff,
        0, 0, 0, 0, 0
    };

    test_parse_uart_packet_all_parameters(
        packet2, packet_size2,
        expected_id2, expected_instruction2, expected_error2,
        expected_header_position2, expected_parameter_size2, expected_result2,
        expected_parameter2, 15
    );
}

TEST(ANALYZE_PACKET, ParseParameterSize0)
{
    int packet_size = 17;
    uint8_t packet[] = {
        0, 0,
        0xff, 0xff, 0xfd, 0x00,
        0x01,
        0x04, 0x00,
        0x55,
        0x00,
        0xa1, 0x0c,
        0, 0, 0, 0
    };

    uint8_t expected_id = 0x01, expected_instruction = 0x55, expected_error = 0x00;
    int expected_header_position = 2, expected_parameter_size = 0, expected_result = 0;
    uint8_t expected_parameter[] = {
        0, 0
    };

    test_parse_uart_packet_all_parameters(
        packet, packet_size,
        expected_id, expected_instruction, expected_error,
        expected_header_position, expected_parameter_size, expected_result,
        expected_parameter, 2
    );
}

TEST(ANALYZE_PACKET, test_parse_uart_packet_ReturnErrorValueWhenDataIsInvalid)
{
    int packet_size1 = 20;
    uint8_t packet1[] = {
        0, 0,
        0xff, 0xff, 0xfd, 0x00,
        0x01,
        0x07, 0x00,
        0x55,
        0x00,
        0x06, 0x04,
        0x26,
        0x65, 0x5e,
        0, 0, 0, 0
    };

    int expected_result1 = 1;

    test_parse_uart_return_value(
        packet1, packet_size1, expected_result1
    );


    int packet_size2 = 10;
    uint8_t packet2[] = {
        0, 0,
        0xff, 0xff, 0xfd, 0xff, 0x00,
        0, 0, 0, 0
    };

    int expected_result2 = 3;

    test_parse_uart_return_value(
        packet2, packet_size2, expected_result2
    );
}

TEST(ANALYZE_PACKET, test_parse_uart_packet_ReturnErrorValueWhenOnlySomeDataIsAvailable)
{
    int packet_size1 = 10;
    uint8_t packet1[] = {
        0, 0, 0, 0, 0,
        0xff, 0xff, 0xfd, 0x00,
        0x01
    };

    int expected_position1 = 5, expected_result1 = 2;

    test_parse_uart_return_value_and_position(
        packet1, packet_size1, expected_position1, expected_result1
    );


    int packet_size2 = 15;
    uint8_t packet2[] = {
        0, 0, 0, 0, 0, 0,
        0xff, 0xff, 0xfd, 0x00,
        0x01,
        0x07, 0x00,
        0x55,
        0x00
    };

    int expected_position2 = 6, expected_result2 = 2;

    test_parse_uart_return_value(
        packet2, packet_size2, expected_result2
    );


    int packet_size3 = 12;
    uint8_t packet3[] = {
        0, 0, 0, 0, 0, 0, 0,
        0xff, 0xff, 0xfd, 0x00,
        0x01,
        0x08
    };

    int expected_position3 = 7, expected_result3 = 2;

    test_parse_uart_return_value(
        packet3, packet_size3, expected_result3
    );
}


TEST(ANALYZE_PACKET, CombinePositive4Bytes)
{
    int32_t result = combine_signed_4_byte(0xff, 0x00, 0x00, 0x00);
    LONGS_EQUAL(255, result);
}

TEST(ANALYZE_PACKET, CombineNegative4Bytes)
{
    int32_t result = combine_signed_4_byte(0xff, 0xff, 0xff, 0xff);
    LONGS_EQUAL(-1, result);
}

TEST(ANALYZE_PACKET, CombinePositive2Bytes)
{
    int16_t result = combine_signed_2_byte(0xff, 0x00);
    LONGS_EQUAL(255, result);
}

TEST(ANALYZE_PACKET, CombineNegative2Bytes)
{
    int16_t result = combine_signed_2_byte(0xff, 0xff);
    LONGS_EQUAL(-1, result);
}

TEST(ANALYZE_PACKET, DivideInto4Bytes)
{
    int32_t input = 0x30201ff;
    uint8_t bytes[4];
    uint8_t expected_bytes[] = {0xff, 0x01, 0x02, 0x03};
    divide_into_4_byte(input, bytes, bytes + 1, bytes + 2, bytes + 3);

    for (size_t i = 0; i < 4; i++)
    {
        LONGS_EQUAL(expected_bytes[i], bytes[i]);
    }
}
