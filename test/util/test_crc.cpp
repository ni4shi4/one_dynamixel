#include "CppUTest/TestHarness.h"
#include "util/crc.h"

TEST_GROUP(CRC)
{
    void setup()
    {
    }

    void teardown()
    {
    }
};

TEST(CRC, CRC16IBMFor1ByteData)
{
    int len1 = 1;
    uint8_t data1[] = {
        0x55
    };
    uint16_t expected1 = 0x01fe;
    CHECK_EQUAL(expected1, crc_16_ibm(data1, len1));
}

TEST(CRC, CRC16IBMFor2ByteData)
{
    int len2 = 2;
    uint8_t data2[] = {
        0x55, 0x44,
    };
    uint16_t expected2 = 0x7f9d;
    CHECK_EQUAL(expected2, crc_16_ibm(data2, len2));
}

TEST(CRC, CRC16IBMFor8ByteData)
{
    int len3 = 8;
    uint8_t data3[] = {
        0xff, 0xff, 0xfd, 0x00,
        0x01,
        0x03, 0x00,
        0x01,
    };
    uint16_t expected3 = 0x4e19;
    CHECK_EQUAL(expected3, crc_16_ibm(data3, len3));
}

TEST(CRC, CRC16IBMFor12ByteData)
{
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
    CHECK_EQUAL(expected4, crc_16_ibm(data4, len4));
}

TEST(CRC, CRC16IBMFor23ByteData)
{
    int len5 = 23;
    uint8_t data5[] = {
        0xff, 0xff, 0xfd, 0x00,
        0x01,
        0x12, 0x00,
        0x03,
        0x7a, 0x02, 0xff, 0xff, 0xfd, 0xfd, 0xff, 0xff, 0xfd, 0xfd, 0xff, 0xff, 0xfd, 0xfd, 0xff
    };
    uint16_t expected5 = 0xe2a3;
    CHECK_EQUAL(expected5, crc_16_ibm(data5, len5));
}
