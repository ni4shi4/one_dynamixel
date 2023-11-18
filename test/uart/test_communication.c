#include "unity.h"
#include "uart/communication.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_create_uart_packet_CreateExpectedOutputForSpecificInput(void)
{
    uint8_t id1 = 0x01, instruction1 = 0x02;
    uint16_t parameter_length1 = 0x0004;
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
        id1, parameter_length1, instruction1, parameter1
    );
    TEST_ASSERT_EQUAL_INT(expected_packet_size1, packet_size1);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_packet1, packet1, 20);


    uint8_t id2 = 0x01, instruction2 = 0x01;
    uint16_t parameter_length2 = 0x0000;
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
        id2, parameter_length2, instruction2, parameter2
    );
    TEST_ASSERT_EQUAL_INT(expected_packet_size2, packet_size2);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_packet2, packet2, 16);

    // 例外処理をテスト
    uint8_t id3 = 0x01, instruction3 = 0x03;
    uint16_t parameter_length3 = 0x000c;
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
        id3, parameter_length3, instruction3, parameter3
    );
    TEST_ASSERT_EQUAL_INT(expected_packet_size3, packet_size3);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_packet3, packet3, 30);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_create_uart_packet_CreateExpectedOutputForSpecificInput);
    return UNITY_END();
}
