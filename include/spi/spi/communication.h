#ifndef _ONE_DYNAMIXEL_COMMUNICATION_H
#define _ONE_DYNAMIXEL_COMMUNICATION_H

#include "pico.h"

#ifdef __cplusplus
extern "C" {
#endif


/***
 * @brief SPI通信で送信するデータ(インストラクションパケット)を作成する
 * 
 * 結果を格納する変数を予め用意した上で引数として渡す必要がある
 * @param[out] *packet SPI通信で送信するためのデータ(配列)
 * @param[in] id デバイスID
 * @param[in] parameter_length parameterのバイト数(=配列長)
 * @param[in] instruction インストラクション(0x55固定)
 * @param[in] parameter 追加情報(配列)
 * @return パケットのバイト長
*/
int create_spi_packet(
    uint8_t *packet,
    uint8_t id,
    uint16_t parameter_length,
    uint8_t instruction,
    const uint8_t *parameter
);

/***
 * @brief SPI通信で受信したデータ(ステータスパケット)を解析し、checksumの確認を行う
 * 
 * 結果を格納する変数を予め用意した上で引数として渡す必要がある
 * @param[in] *packet SPI通信で受信したデータ(配列)
 * @param[out] *id デバイスID
 * @param[out] *length instruction以降のバイト数
 * @param[out] *instruction インストラクション(0x55固定)
 * @param[out] *error エラー番号
 * @param[out] *parameter 追加情報(配列)
 * @retval true checksum値が正しい
 * @retval false checksum値が誤っている(受信データに誤りがある可能性がある)
*/
bool parse_spi_packet(
    const uint8_t *packet,
    uint8_t *id,
    uint16_t *length,
    uint8_t *instruction,
    uint8_t *error,
    uint8_t *parameter
);

#ifdef __cplusplus
}
#endif

#endif
