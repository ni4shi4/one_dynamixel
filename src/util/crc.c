#include "util/crc.h"

/// @brief CRC多項式
const uint16_t CRC_16_POLY = 0x8005;
/// @brief CRC初期値
const uint16_t CRC_16_INIT_VALUE = 0x0000;

uint16_t crc_16_ibm(
    const uint8_t *data,
    int len
)
{
    uint16_t crc = CRC_16_INIT_VALUE;
    for (int i = 0; i < len; i++) {
        crc ^= (data[i] << 8);
        for (int j = 0; j < 8; j++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ CRC_16_POLY;
            } else {
                crc <<= 1;
            }
        }
    }

    return crc;
}
