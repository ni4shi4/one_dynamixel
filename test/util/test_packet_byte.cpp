#include "CppUTest/TestHarness.h"
#include "util/packet_byte.h"


TEST_GROUP(PACKET_BYTE)
{
    void setup()
    {
    }

    void teardown()
    {
    }
};

TEST(PACKET_BYTE, GetCorrectBaudRateByte)
{
    dynamixel_baud_rate baud_rate;
    int result = get_baud_rate_byte(3000000, &baud_rate);
    LONGS_EQUAL(0, result);
    LONGS_EQUAL(DYNAMIXEL_BAUD_RATE_3M, baud_rate);
}

TEST(PACKET_BYTE, GetWrongBaudRateByte)
{
    dynamixel_baud_rate baud_rate;
    int result = get_baud_rate_byte(3000001, &baud_rate);
    LONGS_EQUAL(1, result);
}

TEST(PACKET_BYTE, GetCorrectBaudRate)
{
    uint baud_rate = get_baud_rate(DYNAMIXEL_BAUD_RATE_4M);
    LONGS_EQUAL(4000000, baud_rate);
}
