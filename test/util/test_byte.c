#include "unity.h"
#include "util/byte.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_divide_into_byte_pair_ReturnExpectedValueForSpecificInput(void)
{
    uint16_t input = 0x1234;
    uint8_t byte_lr[2] = {0};
    uint8_t expected[] = { 0x34, 0x12 };

    divide_into_byte_pair(input, byte_lr, byte_lr + 1);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, byte_lr, 2);
}

void test_combine_byte_pair_ReturnExpectedValueForSpecificData(void)
{
    uint8_t byte_l = 0x12;
    uint8_t byte_r = 0x34;
    uint16_t expected = 0x3412;

    TEST_ASSERT_EQUAL_HEX16(expected, combine_byte_pair(byte_l, byte_r));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_divide_into_byte_pair_ReturnExpectedValueForSpecificInput);
    RUN_TEST(test_combine_byte_pair_ReturnExpectedValueForSpecificData);
    return UNITY_END();
}
