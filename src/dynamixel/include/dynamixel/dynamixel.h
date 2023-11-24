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
 * @param[out] dynamixel_id 破棄するdynamixelインスタンス
*/
void dynamixel_destroy(
    dynamixel_t dynamixel_id
);


/**
 * @brief dynamixelにパケットを送る
 *
 * @param[int] dynamixel_id dynamixelインスタンス
 * @param[in] id パケットを送るDynamixelのID
 * @param[in] instruction インストラクション
 * @param[in] parameter_size 追加情報のサイズ
 * @param[in] *parameter 追加情報
 * @retval 0 パケット送信に成功した(正しい応答が返ってきた)
 * @retval 1 パケット送信して応答が返ってきたが、チェックサムが誤っていた
 * @retval 2 パケット送信して応答が返ってきたが、ステータスがエラーだった
 * @retval 3 パケット送信して応答が返ってきたが、応答パケットのデータ量が不十分であった
 * @retval 4 パケット送信して応答が返ってきたが、応答パケットのデータ量が大きすぎてバッファーに入りきらなかった
 * @retval 5 パケット送信したが、時間内に応答が返ってこなかった
*/
int dynamixel_send_packet(
    dynamixel_t dynamixel_id,
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
