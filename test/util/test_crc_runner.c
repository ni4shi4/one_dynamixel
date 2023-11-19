#include "unity_fixture.h"

TEST_GROUP_RUNNER(CRC)
{
    RUN_TEST_CASE(CRC, test_bit_reflect_8_ReturnExpectedValueForSpecificInput);
    RUN_TEST_CASE(CRC, test_bit_reflect_16_ReturnExpectedValueForSpecificInput);
    RUN_TEST_CASE(CRC, test_crc_16_ibm_ReturnExpectedValueForSpecificData);
}
