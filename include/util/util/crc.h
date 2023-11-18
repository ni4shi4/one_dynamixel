#ifndef _ONE_DYNAMIXEL_CRC_H
#define _ONE_DYNAMIXEL_CRC_H

#include "pico.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 8bitのビット列を反転したものを返す
 * @param[in] input 反転するビット列
 * @return 反転後のビット列
*/
uint8_t bit_reflect_8(
    uint8_t input
);

/**
 * @brief 16bitのビット列を反転したものを返す
 * @param[in] input 反転するビット列
 * @return 反転後のビット列
*/
uint16_t bit_reflect_16(
    uint16_t input
);

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
