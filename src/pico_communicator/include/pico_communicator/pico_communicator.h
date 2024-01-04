#ifndef _ONE_DYNAMIXEL_PICO_COMMUNICATOR_H
#define _ONE_DYNAMIXEL_PICO_COMMUNICATOR_H

#include "pico.h"
#include "hardware/uart.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief UARTの初期化をする
 *
 * @param[in] *uart_id uartインスタンス
 * @param[in] baud_rate UARTの通信でのボーレート
 * @param[in] data_bits UARTの通信でのデータビット
 * @param[in] stop_bits UARTの通信でのストップビット
 * @param[in] parity UARTの通信でのパリティ
 * @return 実際に設定されたボーレート
*/
uint pico_uart_init(
    uart_inst_t *uart_id,
    uint baud_rate,
    uint data_bits,
    uint stop_bits,
    uart_parity_t parity
);


/**
 * @brief UARTの初期化を解除する
 *
 * @param[in] *uart_id uartインスタンス
*/
void pico_uart_deinit(
    uart_inst_t *uart_id
);


/**
 * @brief ボーレートを設定する
 *
 * @param[in] *uart_id uartインスタンス
 * @param[in] baud_rate UARTの通信でのボーレート
 * @return 実際に設定されたボーレート
*/
uint pico_uart_set_baudrate(
    uart_inst_t *uart_id,
    uint baud_rate
);


/**
 * @brief UARTのFIFOを有効化・無効化する
 *
 * @param[in] *uart_id uartインスタンス
 * @param[in] enabled 有効化を行う場合は、true
*/
void pico_uart_set_fifo_enabled(
    uart_inst_t *uart_id,
    bool enabled
);


/**
 * @brief UARTに書き込みを行う
 *
 * @param[in] *uart_id uartインスタンス
 * @param[in] *src 書き込みを行うデータ
 * @param[in] len データのサイズ
 * @retval 0 書き込みを行った
 * @retval 1 書き込みを行えなかった
*/
int pico_uart_write_blocking(
    uart_inst_t *uart_id,
    const uint8_t *src,
    size_t len
);


/**
 * @brief UARTのFIFOからの読み込みが可能か
 *
 * @param[in] *uart_id uartインスタンス
 * @retval 0 読み込み可能
 * @retval 1 読み込み不可能
*/
int pico_uart_is_readable(
    uart_inst_t *uart_id
);


/**
 * @brief UARTのFIFOからの指定時間以内の読み込みか可能か
 *
 * @param[in] *uart_id uartインスタンス
 * @param[in] us 待機時間[micro second]
 * @retval 0 読み込み可能
 * @retval 1 読み込み不可能
*/
int pico_uart_is_readable_within_us(
    uart_inst_t *uart_id,
    uint us
);


/**
 * @brief UARTのFIFOから1Byte読み込む
 *
 * @param[in] *uart_id uartインスタンス
 * @param[out] *dst 読み込みデータの保存先
 * @retval 0 読み込みを行った
 * @retval 1 読み込みを行えなかった(データがなかった)
*/
int pico_uart_read_raw(
    uart_inst_t *uart_id,
    uint8_t *dst
);


#ifdef __cplusplus
}
#endif

#endif
