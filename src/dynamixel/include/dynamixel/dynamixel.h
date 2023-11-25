#ifndef _ONE_DYNAMIXEL_DYNAMIXEL_H
#define _ONE_DYNAMIXEL_DYNAMIXEL_H

#include "pico.h"
#include "hardware/uart.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief dynamixelインスタンス
*/
typedef struct dynamixel_struct *dynamixel_t;

/**
 * @brief dynamixelインスタンスを作成する
 *
 * @param[in] *uart_id UARTインスタンス
 * @param[in] gpio_uart_rx UART通信の受信を行うGPIO
 * @param[in] gpio_uart_tx UART通信の送信を行うGPIO
 * @param[in] baud_rate UART通信のボーレート
 * @param[in] buffer_size 受信データのバッファーのサイズ(20未満を指定した場合は、20になる)
 * @param[in] wait_us 送信後の待ち時間[micro sec.]
 * @retval NULL 他のdynamixelインスタンスで使用したGPIOピン、UARTインスタンスを指定したとき
 * @retval dynamixelインスタンス
*/
dynamixel_t dynamixel_create(
    uart_inst_t *uart_id,
    uint gpio_uart_rx,
    uint gpio_uart_tx,
    uint baud_rate,
    size_t buffer_size,
    uint wait_us
);


/**
 * @brief dynamixelインスタンスを破棄する
 *
 * @param[out] self 破棄するdynamixelインスタンス
*/
void dynamixel_destroy(
    dynamixel_t self
);


/**
 * @brief dynamixelにパケットを送る
 *
 * @param[int] self dynamixelインスタンス
 * @param[in] id パケットを送るDynamixelのID
 * @param[in] instruction インストラクション
 * @param[in] parameter_size 追加情報のサイズ
 * @param[in] *parameter 追加情報
 * @retval 0 パケット送信に成功した
 * @retval 1 パケット送信に失敗した
*/
int dynamixel_write_uart_packet(
    dynamixel_t self,
    uint8_t id,
    uint8_t instruction,
    uint16_t parameter_size,
    const uint8_t *parameter
);


/**
 * @brief 応答パケットを読み取ってバッファーに保存する
 *
 * @param[in] self dynamixelインスタンス
 * @param[out] *initial_status_packet_size 読み取り結果をバッファーに保存する際の最初の位置
 * @retval 0 応答パケットの読み取りに成功した
 * @retval 1 応答パケットのデータ量が大きすぎてバッファーに入りきらなかった
*/
int dynamixel_partial_read_uart_packet(
    dynamixel_t self,
    size_t *initial_status_packet_size
);


/**
 * @brief バッファーに保存された応答パケットを解析する
 *
 * @param[in] self dynamixelインスタンス
 * @param[out] *error 応答パケットのエラーステータス
 * @param[out] *status_parameter_size 応答パケットのパラメータのサイズ
 * @param[out] *status_parameter 応答パケットのパラメータ
 * @param[out] *initial_status_packet_size 読み取り結果をバッファーに保存する際の最初の位置
 * @retval 0 応答パケットを解析できた(応答パケットが正しかった)
 * @retval 1 応答パケットを解析できたが、チェックサムが誤っていた
 * @retval 2 応答パケットを解析できたが、ステータスがエラーだった
 * @retval 3 応答パケットを解析できなかった(応答パケットのデータ量が不十分である可能性がある)
*/
int dynamixel_partial_uart_packet(
    dynamixel_t self,
    uint8_t *error,
    size_t *status_parameter_size,
    uint8_t *status_parameter,
    size_t *initial_status_packet_size
);


/**
 * @brief 応答パケットを部分的に読み取って解析する
 *
 * @param[in] self dynamixelインスタンス
 * @param[out] *error 応答パケットのエラーステータス
 * @param[out] *status_parameter_size 応答パケットのパラメータのサイズ
 * @param[out] *status_parameter 応答パケットのパラメータ
 * @param[out] *initial_status_packet_size 読み取り結果をバッファーに保存する際の最初の位置
 * @retval 0 パケット送信に成功した(正しい応答が返ってきた)
 * @retval 1 パケット送信して応答が返ってきたが、チェックサムが誤っていた
 * @retval 2 パケット送信して応答が返ってきたが、ステータスがエラーだった
 * @retval 3 パケット送信して応答が返ってきたが、応答パケットのデータ量が不十分である可能性がある
 * @retval 4 パケット送信して応答が返ってきたが、応答パケットのデータ量が大きすぎてバッファーに入りきらなかった
*/
int dynamixel_partial_read_and_parse_uart_packet(
    dynamixel_t self,
    uint8_t *error,
    size_t *status_parameter_size,
    uint8_t *status_parameter,
    size_t *initial_status_packet_size
);


/**
 * @brief 応答パケットを読み取って解析する
 *
 * @param[int] self dynamixelインスタンス
 * @param[out] *error 応答パケットのエラーステータス
 * @param[out] *status_parameter_size 応答パケットのパラメータのサイズ
 * @param[out] *status_parameter 応答パケットのパラメータ
 * @param[out] *initial_status_packet_size 読み取り結果をバッファーに保存する際の最初の位置
 * @retval 0 パケット送信に成功した(正しい応答が返ってきた)
 * @retval 1 パケット送信して応答が返ってきたが、チェックサムが誤っていた
 * @retval 2 パケット送信して応答が返ってきたが、ステータスがエラーだった
 * @retval 3 パケット送信して応答が返ってきたが、応答パケットのデータ量が不十分であった
 * @retval 4 パケット送信して応答が返ってきたが、応答パケットのデータ量が大きすぎてバッファーに入りきらなかった
 * @retval 5 パケット送信したが、時間内に応答が返ってこなかった
*/
int dynamixel_read_uart_packet(
    dynamixel_t self,
    uint8_t *error,
    size_t *status_parameter_size,
    uint8_t *status_parameter
);


/**
 * @brief dynamixelにパケットを送って、応答パケットを解析する
 *
 * @param[int] self dynamixelインスタンス
 * @param[in] id パケットを送るDynamixelのID
 * @param[in] instruction インストラクション
 * @param[in] parameter_size 追加情報のサイズ
 * @param[in] *parameter 追加情報
 * @param[out] *error 応答パケットのエラーステータス
 * @param[out] *status_parameter_size 応答パケットのパラメータのサイズ
 * @param[out] *status_parameter 応答パケットのパラメータ
 * @retval 0 パケット送信に成功した(正しい応答が返ってきた)
 * @retval 1 パケット送信して応答が返ってきたが、チェックサムが誤っていた
 * @retval 2 パケット送信して応答が返ってきたが、ステータスがエラーだった
 * @retval 3 パケット送信して応答が返ってきたが、応答パケットのデータ量が不十分であった
 * @retval 4 パケット送信して応答が返ってきたが、応答パケットのデータ量が大きすぎてバッファーに入りきらなかった
 * @retval 5 パケット送信したが、時間内に応答が返ってこなかった
*/
int dynamixel_send_packet(
    dynamixel_t self,
    uint8_t id,
    uint8_t instruction,
    uint16_t parameter_size,
    const uint8_t *parameter,
    uint8_t *error,
    size_t *status_parameter_size,
    uint8_t *status_parameter
);


#ifdef __cplusplus
}
#endif

#endif
