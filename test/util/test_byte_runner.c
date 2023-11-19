#include "unity_fixture.h"

TEST_GROUP_RUNNER(BYTE)
{
    RUN_TEST_CASE(BYTE, test_divide_into_byte_pair_ReturnExpectedValueForSpecificInput);
    RUN_TEST_CASE(BYTE, test_combine_byte_pair_ReturnExpectedValueForSpecificData);
}
