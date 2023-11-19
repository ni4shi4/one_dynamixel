#include "unity_fixture.h"

TEST_GROUP_RUNNER(COMMUNICATION)
{
    RUN_TEST_CASE(COMMUNICATION, test_create_uart_packet_CreateExpectedOutputForSpecificInput);
    RUN_TEST_CASE(COMMUNICATION, test_parse_uart_packet_CreateExpectedOutputForSpecificInput);
}
