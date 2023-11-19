#include "unity.h"
#include "unity_fixture.h"
#include "uart/communication.h"

#define PARAMETER_SIZE 100


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
    int header_position, parameter_size, result;
    uint8_t parameter[PARAMETER_SIZE] = {0};

    result = parse_uart_packet(
        packet, packet_size,
        &header_position, &id, &instruction, &error,
        parameter, &parameter_size
    );

    TEST_ASSERT_EQUAL_INT_MESSAGE(expected_header_position, header_position, "output test [header_posotion]");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(expected_id, id, "output test [id]");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(expected_instruction, instruction, "output test [instruction]");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(expected_error, error, "output test [error]");
    TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(expected_parameter, parameter, compare_parameter_size, "output test [parameter]");
    TEST_ASSERT_EQUAL_INT_MESSAGE(expected_parameter_size, parameter_size, "output test [parameter_size]");
    TEST_ASSERT_EQUAL_INT_MESSAGE(expected_result, result, "return test");
}

void test_parse_uart_return_value(
    uint8_t *packet,
    int packet_size,
    int expected_result
)
{
    uint8_t id, instruction, error;
    int header_position, parameter_size, result;
    uint8_t parameter[PARAMETER_SIZE] = {0};

    result = parse_uart_packet(
        packet, packet_size,
        &header_position, &id, &instruction, &error,
        parameter, &parameter_size
    );

    TEST_ASSERT_EQUAL_INT_MESSAGE(expected_result, result, "return test");
}

void test_parse_uart_return_value_and_position(
    uint8_t *packet,
    int packet_size,
    int expected_header_position,
    int expected_result
)
{
    uint8_t id, instruction, error;
    int header_position, parameter_size, result;
    uint8_t parameter[PARAMETER_SIZE] = {0};

    result = parse_uart_packet(
        packet, packet_size,
        &header_position, &id, &instruction, &error,
        parameter, &parameter_size
    );

    TEST_ASSERT_EQUAL_INT_MESSAGE(expected_header_position, header_position, "output test [header_posotion]");
    TEST_ASSERT_EQUAL_INT_MESSAGE(expected_result, result, "return test");
}


TEST_GROUP(COMMUNICATION);

TEST_SETUP(COMMUNICATION)
{
}

TEST_TEAR_DOWN(COMMUNICATION)
{
}

TEST(COMMUNICATION, test_create_uart_packet_CreateExpectedOutputForSpecificInput)
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
    TEST_ASSERT_EQUAL_INT(expected_packet_size1, packet_size1);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_packet1, packet1, 20);


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
    TEST_ASSERT_EQUAL_INT(expected_packet_size2, packet_size2);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_packet2, packet2, 16);

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
    TEST_ASSERT_EQUAL_INT(expected_packet_size3, packet_size3);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_packet3, packet3, 30);
}

TEST(COMMUNICATION, test_parse_uart_packet_CreateExpectedOutputForSpecificInput)
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

TEST(COMMUNICATION, test_parse_uart_packet_ReturnErrorValueWhenDataIsInvalid)
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

TEST(COMMUNICATION, test_parse_uart_packet_ReturnErrorValueWhenOnlySomeDataIsAvailable)
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
}
