#ifndef _ONE_DYNAMIXEL_CRC_H
#define _ONE_DYNAMIXEL_CRC_H

#include "pico.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief チェックサム値を計算する(CRC-16-IBM)。
 *
 *  Dynamixelと通信する際は、下位8ビットを先に送る(リトルエンディアン)
 *  @param[in] *data チェックサムを計算するデータ
 *  @param[in] len データの長さ
 *  @return 計算したチェックサム値
 */
uint16_t crc_16_ibm(
    const uint8_t *data,
    int len
);


#ifdef __cplusplus
}
#endif

#endif
