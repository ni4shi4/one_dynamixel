#include "unity.h"
#include "util/crc.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_bit_reflect_8_ReturnExpectedValueForSpecificInput(void)
{
    uint8_t input, expected;

    input = 0x00;
    expected = 0x00;
    TEST_ASSERT_EQUAL_HEX8(expected, bit_reflect_8(input));

    input = 0x05;
    expected = 0xa0;
    TEST_ASSERT_EQUAL_HEX8(expected, bit_reflect_8(input));

}

void test_bit_reflect_16_ReturnExpectedValueForSpecificInput(void)
{
    uint16_t input, expected;

    input = 0x0000;
    expected = 0x0000;
    TEST_ASSERT_EQUAL_HEX16(expected, bit_reflect_16(input));

    input = 0x1345;
    expected = 0xa2c8;
    TEST_ASSERT_EQUAL_HEX16(expected, bit_reflect_16(input));

}

void test_crc_16_ibm_ReturnExpectedValueForSpecificData(void)
{
    int len1 = 1;
    uint8_t data1[] = {
        0x55
    };
    uint16_t expected1 = 0x01fe;
    TEST_ASSERT_EQUAL_HEX16(expected1, crc_16_ibm(data1, len1));

    int len2 = 2;
    uint8_t data2[] = {
        0x55, 0x44,
    };
    uint16_t expected2 = 0x7f9d;
    TEST_ASSERT_EQUAL_HEX16(expected2, crc_16_ibm(data2, len2));

    int len3 = 8;
    uint8_t data3[] = {
        0xff, 0xff, 0xfd, 0x00,
        0x01,
        0x03, 0x00,
        0x01,
    };
    uint16_t expected3 = 0x4e19;
    TEST_ASSERT_EQUAL_HEX16(expected3, crc_16_ibm(data3, len3));

    int len4 = 12;
    uint8_t data4[] = {
        0xff, 0xff, 0xfd, 0x00,
        0x01,
        0x07, 0x00,
        0x02,
        0x84, 0x00,
        0x04, 0x00,
    };
    uint16_t expected4 = 0x151d;
    TEST_ASSERT_EQUAL_HEX16(expected4, crc_16_ibm(data4, len4));

    int len5 = 23;
    uint8_t data5[] = {
        0xff, 0xff, 0xfd, 0x00,
        0x01,
        0x12, 0x00,
        0x03,
        0x7a, 0x02, 0xff, 0xff, 0xfd, 0xfd, 0xff, 0xff, 0xfd, 0xfd, 0xff, 0xff, 0xfd, 0xfd, 0xff
    };
    uint16_t expected5 = 0xe2a3;
    TEST_ASSERT_EQUAL_HEX16(expected5, crc_16_ibm(data5, len5));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_bit_reflect_8_ReturnExpectedValueForSpecificInput);
    RUN_TEST(test_bit_reflect_16_ReturnExpectedValueForSpecificInput);
    RUN_TEST(test_crc_16_ibm_ReturnExpectedValueForSpecificData);
    return UNITY_END();
}
