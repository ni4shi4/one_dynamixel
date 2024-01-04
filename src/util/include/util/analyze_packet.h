#ifndef _ONE_DYNAMIXEL_ANALYZE_PACKET_H
#define _ONE_DYNAMIXEL_ANALYZE_PACKET_H

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
 * @brief 4バイトデータを1バイトずつに分割する(リトルエンディアン)
 * @param[in] input 分割対象の4バイトデータ
 * @param[out] byte_1 分割後の1バイト目のデータ
 * @param[out] byte_2 分割後の2バイト目のデータ
 * @param[out] byte_3 分割後の3バイト目のデータ
 * @param[out] byte_4 分割後の4バイト目のデータ
*/
void divide_into_4_byte(
    int32_t input,
    uint8_t *byte_1,
    uint8_t *byte_2,
    uint8_t *byte_3,
    uint8_t *byte_4
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

/**
 * @brief 2バイトデータを結合して1つの変数に格納する(リトルエンディアン)
 * @param[in] byte_1 1バイト目のデータ
 * @param[in] byte_2 2バイト目のデータ
*/
int16_t combine_signed_2_byte(
    uint8_t byte_1,
    uint8_t byte_2
);

/**
 * @brief 4バイトデータを結合して1つの変数に格納する(リトルエンディアン)
 * @param[in] byte_1 1バイト目のデータ
 * @param[in] byte_2 2バイト目のデータ
 * @param[in] byte_3 3バイト目のデータ
 * @param[in] byte_4 4バイト目のデータ
*/
int32_t combine_signed_4_byte(
    uint8_t byte_1,
    uint8_t byte_2,
    uint8_t byte_3,
    uint8_t byte_4
);

/***
 * @brief SPI通信で送信するデータ(インストラクションパケット)を作成する
 *
 * 結果を格納する変数を予め用意した上で引数として渡す必要がある
 * @param[out] *packet SPI通信で送信するためのデータ(配列)
 * @param[in] id デバイスID
 * @param[in] instruction インストラクション(0x55固定)
 * @param[in] parameter 追加情報(配列)
 * @param[in] parameter_size parameterのバイト数(=配列長)
 * @return パケットのバイト長
*/
int create_uart_packet(
    uint8_t *packet,
    uint8_t id,
    uint8_t instruction,
    const uint8_t *parameter,
    uint16_t parameter_size
);

/***
 * @brief SPI通信で受信したデータ(ステータスパケット)を解析し、checksumの確認を行う
 *
 * 結果を格納する変数を予め用意した上で引数として渡す必要がある
 * @param[in] *packet SPI通信で受信したデータ(配列)
 * @param[in] packet_size packetのバイト数(=配列長)
 * @param[out] *header_position ヘッダーの位置
 * @param[out] *id デバイスID
 * @param[out] *instruction インストラクション(0x55固定)
 * @param[out] *error エラー番号
 * @param[out] *parameter 追加情報(配列)。事前に十分なサイズを用意する(packet_size以上)
 * @param[out] *parameter_size parameterのバイト数(=配列長)
 * @retval 0 checksum値が正しいデータが見つかった
 * @retval 1 データは見つかったが、checksum値が誤っている(受信データに誤りがある可能性がある)
 * @retval 2 ヘッダーは見つかったが、すべてのデータが見つからなかった(header_positionにヘッダーが代入される)
 * @retval 3 ヘッダーが見つからなかった
*/
int parse_uart_packet(
    const uint8_t *packet,
    int packet_size,
    int *header_position,
    uint8_t *id,
    uint8_t *instruction,
    uint8_t *error,
    uint8_t *parameter,
    size_t *parameter_size
);

#ifdef __cplusplus
}
#endif

#endif
