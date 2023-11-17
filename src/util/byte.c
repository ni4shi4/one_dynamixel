#include "util/byte.h"


void divide_into_byte_pair(
    uint16_t input,
    uint8_t *byte_l,
    uint8_t *byte_r
)
{
    *byte_l = input & 0xff;
    *byte_r = input >> 8;
}


uint16_t combine_byte_pair(
    uint8_t byte_l,
    uint8_t byte_r
)
{
    return (byte_r << 8) | byte_l;
}
