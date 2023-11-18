#ifndef _ONE_DYNAMIXEL_BYTE_H
#define _ONE_DYNAMIXEL_BYTE_H

#include "pico.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 2バイトデータを1バイトずつに分割する(リトルエンディアン)
 * @param[in] input 分割対象の2バイトデータ
 * @param[out] byte_l 分割後の1バイト目のデータ
 * @param[out] byte_r 分割後の2バイト目のデータ
*/
void divide_into_byte_pair(
    uint16_t input,
    uint8_t *byte_l,
    uint8_t *byte_r
);


/**
 * @brief 1バイトずつのデータを結合して1つの変数に格納する(リトルエンディアン)
 * @param[in] byte_l 1バイト目のデータ
 * @param[in] byte_r 2バイト目のデータ
*/
uint16_t combine_byte_pair(
    uint8_t byte_l,
    uint8_t byte_r
);


#ifdef __cplusplus
}
#endif

#endif
