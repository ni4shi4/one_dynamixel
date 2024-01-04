#ifndef _ONE_DYNAMIXEL_DYNAMIXEL_H
#define _ONE_DYNAMIXEL_DYNAMIXEL_H

#include "pico.h"
#include "hardware/uart.h"
#include "util/packet_byte.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 応答パケットの解析結果を表す
*/
typedef enum {
    DYNAMIXEL_PARSE_SUCCESS, /*!< 応答パケットを解析できた(応答パケットに以上はなかった) */
    DYNAMIXEL_PARSE_WRONG_CHECKSUM, /*!< パケット送信して応答が返ってきたが、チェックサムが誤っていた */
    DYNAMIXEL_PARSE_STATUS_ERROR, /*!< 応答パケットを解析できたが、ステータスがエラーだった */
    DYNAMIXEL_PARSE_INADEQUATE_DATA, /*!< 応答パケットのデータ量が不十分だった(応答パケットを解析できなかった) */
    DYNAMIXEL_PARSE_HUGE_DATA, /*!< 応答パケットのデータ量が大きすぎてバッファーに入りきらなかった(応答パケットを解析できなかった) */
    DYNAMIXEL_PARSE_WRONG_ID, /*!< 応答パケットのIDが、送信したパケットに期待するものと異なっていた */
    DYNAMIXEL_PARSE_NO_RESPONSE, /*!< 時間内に応答パケットが返ってこなかった */
    DYNAMIXEL_PARSE_WRONG_PARAMETER, /*!< 応答パケットのパラメータが、送信したパケットに期待するものと異なっていた */
    DYNAMIXEL_PARSE_WRONG_WRITE_PARAMETER, /*!< 送信パケットに指定するパラメータに誤りがある */
} dynamixel_parse_result;

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
 * @brief dynamixelに通信設定を書き込む
 *
 * @param[in] self dynamixelインスタンス
 * @param[in] id パケットを送るDynamixelのID
 * @param[in] return_delay_time インストラクションパケットを受け取ってから、ステータスパケットを返すまでのディレイ時間(偶数を指定、奇数を指定した場合は+1される)
 * @param[in] operating_mode 動作モード
 * @param[in] wait_us_multiplier 1以上のときcreate時に設定した応答パケットの待ち時間を一時的に、指定した倍数を掛けた値にする。0のときは、最低0.1s(=100000us)待つように設定される)
 * @param[in] iterative_count 1以上のとき設定処理を指定した回数だけ繰り返す。0のときは、5回だけ繰り返す(デフォルト)
 * @return 応答の結果
*/
dynamixel_parse_result dynamixel_configure(
    dynamixel_t self,
    uint8_t id,
    uint8_t *error,
    uint16_t return_delay_time,
    dynamixel_operating_mode operating_mode,
    uint wait_us_multiplier,
    size_t iterative_count
);


/**
 * @brief dynamixelにpingを送る
 *
 * @param[in] self dynamixelインスタンス
 * @param[in] id パケットを送るDynamixelのID
 * @param[out] *error 応答パケットのエラーステータス
 * @param[out] *dynamixel_model_no 応答したdynamixelのModel No.
 * @param[out] *dynamixel_version_of_firmware 応答したdynamixelのFirmare version
 * @param[in] wait_us_multiplier 1以上のときcreate時に設定した応答パケットの待ち時間を一時的に、指定した倍数を掛けた値にする
 * @return 応答の結果
*/
dynamixel_parse_result dynamixel_send_ping(
    dynamixel_t self,
    uint8_t id,
    uint8_t *error,
    uint16_t *dynamixel_model_no,
    uint8_t *dynamixel_version_of_firmware,
    uint wait_us_multiplier
);


/**
 * @brief dynamixelにreadを送る
 *
 * @param[in] self dynamixelインスタンス
 * @param[in] id パケットを送るDynamixelのID
 * @param[in] start_address コントロールテーブルの開始アドレス
 * @param[in] data_size 読み取りを行うデータサイズ
 * @param[out] *error 応答パケットのエラーステータス
 * @param[out] *data コントロールテーブル上のデータ(サイズは必要量より大きくする)
 * @param[in] wait_us_multiplier 1以上のときcreate時に設定した応答パケットの待ち時間を一時的に、指定した倍数を掛けた値にする
 * @return 応答の結果
*/
dynamixel_parse_result dynamixel_send_read_once(
    dynamixel_t self,
    uint8_t id,
    uint16_t start_address,
    uint16_t data_size,
    uint8_t *error,
    uint8_t *data,
    uint wait_us_multiplier
);

/**
 * @brief dynamixelにreadを送る
 *
 * @param[in] self dynamixelインスタンス
 * @param[in] id パケットを送るDynamixelのID
 * @param[in] start_address コントロールテーブルの開始アドレス
 * @param[in] data_size 読み取りを行うデータサイズ
 * @param[out] *error 応答パケットのエラーステータス
 * @param[out] *data コントロールテーブル上のデータ(サイズは必要量より大きくする)
 * @param[in] wait_us_multiplier 1以上のときcreate時に設定した応答パケットの待ち時間を一時的に、指定した倍数を掛けた値にする
 * @param[in] iterative_count 1以上のとき設定処理を指定した回数だけ繰り返す。0のときは、5回だけ繰り返す(デフォルト)
 * @return 応答の結果
*/
dynamixel_parse_result dynamixel_send_read(
    dynamixel_t self,
    uint8_t id,
    uint16_t start_address,
    uint16_t data_size,
    uint8_t *error,
    uint8_t *data,
    uint wait_us_multiplier,
    size_t iterative_count
);

/**
 * @brief dynamixelからtorque enableを取得する
 *
 * @param[in] self dynamixelインスタンス
 * @param[in] id パケットを送るDynamixelのID
 * @param[out] *error 応答パケットのエラーステータス
 * @param[out] *torque_enable トルクがONかOFFか(false = トルクOFF)
 * @param[in] wait_us_multiplier 1以上のときcreate時に設定した応答パケットの待ち時間を一時的に、指定した倍数を掛けた値にする
 * @param[in] iterative_count 1以上のとき設定処理を指定した回数だけ繰り返す。0のときは、5回だけ繰り返す(デフォルト)
 * @return 応答の結果
*/
dynamixel_parse_result dynamixel_send_read_torque_enable(
    dynamixel_t self,
    uint8_t id,
    uint8_t *error,
    bool *torque_enable,
    uint wait_us_multiplier,
    size_t iterative_count
);

/**
 * @brief dynamixelからpositionを取得する
 *
 * @param[in] self dynamixelインスタンス
 * @param[in] id パケットを送るDynamixelのID
 * @param[out] *error 応答パケットのエラーステータス
 * @param[out] *position dynamixelの角度[degree]
 * @param[in] wait_us_multiplier 1以上のときcreate時に設定した応答パケットの待ち時間を一時的に、指定した倍数を掛けた値にする
 * @param[in] iterative_count 1以上のとき設定処理を指定した回数だけ繰り返す。0のときは、5回だけ繰り返す(デフォルト)
 * @return 応答の結果
*/
dynamixel_parse_result dynamixel_send_read_position(
    dynamixel_t self,
    uint8_t id,
    uint8_t *error,
    float *position,
    uint wait_us_multiplier,
    size_t iterative_count
);

/**
 * @brief dynamixelからvelocityを取得する
 *
 * @param[in] self dynamixelインスタンス
 * @param[in] id パケットを送るDynamixelのID
 * @param[out] *error 応答パケットのエラーステータス
 * @param[out] *velocity dynamixelの角速度[rpm]
 * @param[in] wait_us_multiplier 1以上のときcreate時に設定した応答パケットの待ち時間を一時的に、指定した倍数を掛けた値にする
 * @param[in] iterative_count 1以上のとき設定処理を指定した回数だけ繰り返す。0のときは、5回だけ繰り返す(デフォルト)
 * @return 応答の結果
*/
dynamixel_parse_result dynamixel_send_read_velocity(
    dynamixel_t self,
    uint8_t id,
    uint8_t *error,
    float *velocity,
    uint wait_us_multiplier,
    size_t iterative_count
);

/**
 * @brief dynamixelからcurrentを取得する
 *
 * @param[in] self dynamixelインスタンス
 * @param[in] id パケットを送るDynamixelのID
 * @param[out] *error 応答パケットのエラーステータス
 * @param[out] *current dynamixelの電流[mA]
 * @param[in] wait_us_multiplier 1以上のときcreate時に設定した応答パケットの待ち時間を一時的に、指定した倍数を掛けた値にする
 * @param[in] iterative_count 1以上のとき設定処理を指定した回数だけ繰り返す。0のときは、5回だけ繰り返す(デフォルト)
 * @return 応答の結果
*/
dynamixel_parse_result dynamixel_send_read_current(
    dynamixel_t self,
    uint8_t id,
    uint8_t *error,
    float *current,
    uint wait_us_multiplier,
    size_t iterative_count
);

/**
 * @brief dynamixelから温度を取得する
 *
 * @param[in] self dynamixelインスタンス
 * @param[in] id パケットを送るDynamixelのID
 * @param[out] *error 応答パケットのエラーステータス
 * @param[out] *temperature dynamixelの温度
 * @param[in] wait_us_multiplier 1以上のときcreate時に設定した応答パケットの待ち時間を一時的に、指定した倍数を掛けた値にする
 * @param[in] iterative_count 1以上のとき設定処理を指定した回数だけ繰り返す。0のときは、5回だけ繰り返す(デフォルト)
 * @return 応答の結果
*/
dynamixel_parse_result dynamixel_send_read_temperature(
    dynamixel_t self,
    uint8_t id,
    uint8_t *error,
    uint8_t *temperature,
    uint wait_us_multiplier,
    size_t iterative_count
);

/**
 * @brief dynamixelからreturn delay timeを取得する
 *
 * @param[in] self dynamixelインスタンス
 * @param[in] id パケットを送るDynamixelのID
 * @param[out] *error 応答パケットのエラーステータス
 * @param[out] *return_delay_time インストラクションパケットを受け取ってから、ステータスパケットを返すまでのディレイ時間
 * @param[in] wait_us_multiplier 1以上のときcreate時に設定した応答パケットの待ち時間を一時的に、指定した倍数を掛けた値にする
 * @param[in] iterative_count 1以上のとき設定処理を指定した回数だけ繰り返す。0のときは、5回だけ繰り返す(デフォルト)
 * @return 応答の結果
*/
dynamixel_parse_result dynamixel_send_read_return_delay_time(
    dynamixel_t self,
    uint8_t id,
    uint8_t *error,
    uint16_t *return_delay_time,
    uint wait_us_multiplier,
    size_t iterative_count
);

/**
 * @brief dynamixelからdrive modeを取得する
 * @ref https://emanual.robotis.com/docs/en/dxl/x/xl330-m077/#drive-mode
 *
 * @param[in] self dynamixelインスタンス
 * @param[in] id パケットを送るDynamixelのID
 * @param[out] *error 応答パケットのエラーステータス
 * @param[out] *torque_on_by_goal_update Torque On by Goal Update
 * @param[out] *profile_configuration Profile Configuration
 * @param[out] *normal_reverse_mode Normal/Reverse Mode 
 * @param[in] wait_us_multiplier 1以上のときcreate時に設定した応答パケットの待ち時間を一時的に、指定した倍数を掛けた値にする
 * @param[in] iterative_count 1以上のとき設定処理を指定した回数だけ繰り返す。0のときは、5回だけ繰り返す(デフォルト)
 * @return 応答の結果
*/
dynamixel_parse_result dynamixel_send_read_drive_mode(
    dynamixel_t self,
    uint8_t id,
    uint8_t *error,
    uint8_t *torque_on_by_goal_update,
    uint8_t *profile_configuration,
    uint8_t *normal_reverse_mode,
    uint wait_us_multiplier,
    size_t iterative_count
);

/**
 * @brief dynamixelからoperaing modeを取得する
 *
 * @param[in] self dynamixelインスタンス
 * @param[in] id パケットを送るDynamixelのID
 * @param[out] *error 応答パケットのエラーステータス
 * @param[out] *operating_mode 動作モード
 * @param[in] wait_us_multiplier 1以上のときcreate時に設定した応答パケットの待ち時間を一時的に、指定した倍数を掛けた値にする
 * @param[in] iterative_count 1以上のとき設定処理を指定した回数だけ繰り返す。0のときは、5回だけ繰り返す(デフォルト)
 * @return 応答の結果
*/
dynamixel_parse_result dynamixel_send_read_operating_mode(
    dynamixel_t self,
    uint8_t id,
    uint8_t *error,
    dynamixel_operating_mode *operating_mode,
    uint wait_us_multiplier,
    size_t iterative_count
);

/**
 * @brief dynamixelからボーレートを取得する
 *
 * @param[in] self dynamixelインスタンス
 * @param[in] id パケットを送るDynamixelのID
 * @param[out] *error 応答パケットのエラーステータス
 * @param[out] *operating_mode 動作モード
 * @param[in] wait_us_multiplier 1以上のときcreate時に設定した応答パケットの待ち時間を一時的に、指定した倍数を掛けた値にする
 * @param[in] iterative_count 1以上のとき設定処理を指定した回数だけ繰り返す。0のときは、5回だけ繰り返す(デフォルト)
 * @return 応答の結果
*/
dynamixel_parse_result dynamixel_send_read_baud_rate(
    dynamixel_t self,
    uint8_t id,
    uint8_t *error,
    dynamixel_baud_rate *baud_rate,
    uint wait_us_multiplier,
    size_t iterative_count
);


/**
 * @brief dynamixelにwriteを送る
 *
 * @param[in] self dynamixelインスタンス
 * @param[in] id パケットを送るDynamixelのID
 * @param[in] start_address コントロールテーブルの開始アドレス
 * @param[in] data_size 読み取りを行うデータサイズ
 * @param[in] data 書き込みを行うデータ
 * @param[out] *error 応答パケットのエラーステータス
 * @param[in] wait_us_multiplier 1以上のときcreate時に設定した応答パケットの待ち時間を一時的に、指定した倍数を掛けた値にする
 * @return 応答の結果 (DYNAMIXEL_PARSE_WRONG_PARAMETERを除く)
*/
dynamixel_parse_result dynamixel_send_write_once(
    dynamixel_t self,
    uint8_t id,
    uint16_t start_address,
    uint16_t data_size,
    const uint8_t *data,
    uint8_t *error,
    uint wait_us_multiplier
);

/**
 * @brief dynamixelにwriteを送る
 *
 * @param[in] self dynamixelインスタンス
 * @param[in] id パケットを送るDynamixelのID
 * @param[in] start_address コントロールテーブルの開始アドレス
 * @param[in] data_size 読み取りを行うデータサイズ
 * @param[in] data 書き込みを行うデータ
 * @param[out] *error 応答パケットのエラーステータス
 * @param[in] wait_us_multiplier 1以上のときcreate時に設定した応答パケットの待ち時間を一時的に、指定した倍数を掛けた値にする
 * @return 応答の結果 (DYNAMIXEL_PARSE_WRONG_PARAMETERを除く)
*/
dynamixel_parse_result dynamixel_send_write(
    dynamixel_t self,
    uint8_t id,
    uint16_t start_address,
    uint16_t data_size,
    const uint8_t *data,
    uint8_t *error,
    uint wait_us_multiplier,
    size_t iterative_count
);

/**
 * @brief dynamixelのtorque enableを設定する
 *
 * @param[in] self dynamixelインスタンス
 * @param[in] id パケットを送るDynamixelのID
 * @param[out] *error 応答パケットのエラーステータス
 * @param[in] torque_enable トルクがONかOFFか(false = トルクOFF)
 * @param[in] wait_us_multiplier 1以上のときcreate時に設定した応答パケットの待ち時間を一時的に、指定した倍数を掛けた値にする
 * @return 応答の結果
*/
dynamixel_parse_result dynamixel_send_write_torque_enable(
    dynamixel_t self,
    uint8_t id,
    uint8_t *error,
    bool torque_enable,
    uint wait_us_multiplier,
    size_t iterative_count
);

/**
 * @brief dynamixelの目標positionを設定する
 *
 * @param[in] self dynamixelインスタンス
 * @param[in] id パケットを送るDynamixelのID
 * @param[out] *error 応答パケットのエラーステータス
 * @param[in] goal_position dynamixelの角度[degree]
 * @param[in] wait_us_multiplier 1以上のときcreate時に設定した応答パケットの待ち時間を一時的に、指定した倍数を掛けた値にする
 * @return 応答の結果
*/
dynamixel_parse_result dynamixel_send_write_goal_position(
    dynamixel_t self,
    uint8_t id,
    uint8_t *error,
    float goal_position,
    uint wait_us_multiplier,
    size_t iterative_count
);

/**
 * @brief dynamixelの目標velocityを設定する
 *
 * @param[in] self dynamixelインスタンス
 * @param[in] id パケットを送るDynamixelのID
 * @param[out] *error 応答パケットのエラーステータス
 * @param[in] goal_velocity dynamixelの角速度[rpm]
 * @param[in] wait_us_multiplier 1以上のときcreate時に設定した応答パケットの待ち時間を一時的に、指定した倍数を掛けた値にする
 * @return 応答の結果
*/
dynamixel_parse_result dynamixel_send_write_goal_velocity(
    dynamixel_t self,
    uint8_t id,
    uint8_t *error,
    float goal_velocity,
    uint wait_us_multiplier,
    size_t iterative_count
);

/**
 * @brief dynamixelの目標currentを設定する
 *
 * @param[in] self dynamixelインスタンス
 * @param[in] id パケットを送るDynamixelのID
 * @param[out] *error 応答パケットのエラーステータス
 * @param[in] goal_current dynamixelの電流[mA]
 * @param[in] wait_us_multiplier 1以上のときcreate時に設定した応答パケットの待ち時間を一時的に、指定した倍数を掛けた値にする
 * @return 応答の結果
*/
dynamixel_parse_result dynamixel_send_write_goal_current(
    dynamixel_t self,
    uint8_t id,
    uint8_t *error,
    float goal_current,
    uint wait_us_multiplier,
    size_t iterative_count
);

/**
 * @brief dynamixelのreturn delay timeを設定する
 *
 * @param[in] self dynamixelインスタンス
 * @param[in] id パケットを送るDynamixelのID
 * @param[out] *error 応答パケットのエラーステータス
 * @param[in] return_delay_time インストラクションパケットを受け取ってから、ステータスパケットを返すまでのディレイ時間(偶数を指定、奇数を指定した場合は+1される)
 * @param[in] wait_us_multiplier 1以上のときcreate時に設定した応答パケットの待ち時間を一時的に、指定した倍数を掛けた値にする
 * @return 応答の結果
*/
dynamixel_parse_result dynamixel_send_write_return_delay_time(
    dynamixel_t self,
    uint8_t id,
    uint8_t *error,
    uint16_t return_delay_time,
    uint wait_us_multiplier,
    size_t iterative_count
);

/**
 * @brief dynamixelのdrive modeを設定する
 * @ref https://emanual.robotis.com/docs/en/dxl/x/xl330-m077/#drive-mode
 *
 * @param[in] self dynamixelインスタンス
 * @param[in] id パケットを送るDynamixelのID
 * @param[out] *error 応答パケットのエラーステータス
 * @param[in] torque_on_by_goal_update Torque On by Goal Update
 * @param[in] profile_configuration Profile Configuration
 * @param[in] normal_reverse_mode Normal/Reverse Mode 
 * @param[in] wait_us_multiplier 1以上のときcreate時に設定した応答パケットの待ち時間を一時的に、指定した倍数を掛けた値にする
 * @return 応答の結果
*/
dynamixel_parse_result dynamixel_send_write_drive_mode(
    dynamixel_t self,
    uint8_t id,
    uint8_t *error,
    uint8_t torque_on_by_goal_update,
    uint8_t profile_configuration,
    uint8_t normal_reverse_mode,
    uint wait_us_multiplier,
    size_t iterative_count
);

/**
 * @brief dynamixelのoperating modeを設定する
 *
 * @param[in] self dynamixelインスタンス
 * @param[in] id パケットを送るDynamixelのID
 * @param[out] *error 応答パケットのエラーステータス
 * @param[in] operating_mode 動作モード
 * @param[in] wait_us_multiplier 1以上のときcreate時に設定した応答パケットの待ち時間を一時的に、指定した倍数を掛けた値にする
 * @return 応答の結果
*/
dynamixel_parse_result dynamixel_send_write_operating_mode(
    dynamixel_t self,
    uint8_t id,
    uint8_t *error,
    dynamixel_operating_mode operating_mode,
    uint wait_us_multiplier,
    size_t iterative_count
);

/**
 * @brief dynamixelのボーレートを設定する
 *
 * @param[in] self dynamixelインスタンス
 * @param[in] id パケットを送るDynamixelのID
 * @param[out] *error 応答パケットのエラーステータス
 * @param[in] *operating_mode 動作モード
 * @param[in] wait_us_multiplier 1以上のときcreate時に設定した応答パケットの待ち時間を一時的に、指定した倍数を掛けた値にする
 * @return 応答の結果
*/
dynamixel_parse_result dynamixel_send_write_baud_rate(
    dynamixel_t self,
    uint8_t id,
    uint8_t *error,
    dynamixel_baud_rate baud_rate,
    uint wait_us_multiplier,
    size_t iterative_count
);


/**
 * @brief dynamixelにreg_writeを送る
 *
 * @param[in] self dynamixelインスタンス
 * @param[in] id パケットを送るDynamixelのID
 * @param[in] start_address コントロールテーブルの開始アドレス
 * @param[in] data_size 読み取りを行うデータサイズ
 * @param[in] data 書き込みを行うデータ
 * @param[out] *error 応答パケットのエラーステータス
 * @param[in] wait_us_multiplier 1以上のときcreate時に設定した応答パケットの待ち時間を一時的に、指定した倍数を掛けた値にする
 * @return 応答の結果
*/
dynamixel_parse_result dynamixel_send_reg_write(
    dynamixel_t self,
    uint8_t id,
    uint16_t start_address,
    uint16_t data_size,
    const uint8_t *data,
    uint8_t *error,
    uint wait_us_multiplier
);


/**
 * @brief dynamixelにactionを送る
 *
 * @param[in] self dynamixelインスタンス
 * @param[in] id パケットを送るDynamixelのID
 * @param[out] *error 応答パケットのエラーステータス
 * @param[in] wait_us_multiplier 1以上のときcreate時に設定した応答パケットの待ち時間を一時的に、指定した倍数を掛けた値にする
 * @return 応答の結果
*/
dynamixel_parse_result dynamixel_send_action(
    dynamixel_t self,
    uint8_t id,
    uint8_t *error,
    uint wait_us_multiplier
);


/**
 * @brief dynamixelにfactory_resetを送る
 *
 * @param[in] self dynamixelインスタンス
 * @param[in] id パケットを送るDynamixelのID
 * @param[out] *error 応答パケットのエラーステータス
 * @param[in] wait_us_multiplier 1以上のときcreate時に設定した応答パケットの待ち時間を一時的に、指定した倍数を掛けた値にする
 * @return 応答の結果
*/
dynamixel_parse_result dynamixel_send_factory_reset(
    dynamixel_t self,
    uint8_t id,
    uint8_t *error,
    dynamixel_factory_reset_paramerter factory_reset,
    uint wait_us_multiplier
);


/**
 * @brief dynamixelにrebootを送る
 *
 * @param[in] self dynamixelインスタンス
 * @param[in] id パケットを送るDynamixelのID
 * @param[out] *error 応答パケットのエラーステータス
 * @param[in] wait_us_multiplier 1以上のときcreate時に設定した応答パケットの待ち時間を一時的に、指定した倍数を掛けた値にする
 * @return 応答の結果
*/
dynamixel_parse_result dynamixel_send_reboot(
    dynamixel_t self,
    uint8_t id,
    uint8_t *error,
    uint wait_us_multiplier
);


/**
 * @brief dynamixelにパケットを送って、応答パケットを解析する
 *
 * @param[in] self dynamixelインスタンス
 * @param[in] id パケットを送るDynamixelのID
 * @param[in] instruction インストラクション
 * @param[in] parameter_size 追加情報のサイズ
 * @param[in] *parameter 追加情報
 * @param[out] *error 応答パケットのエラーステータス
 * @param[out] *status_parameter_size 応答パケットのパラメータのサイズ
 * @param[out] *status_parameter 応答パケットのパラメータ
 * @param[in] wait_us_multiplier 1以上のときcreate時に設定した応答パケットの待ち時間を一時的に、指定した倍数を掛けた値にする
 * @retval DYNAMIXEL_PARSE_SUCCESS
 * @retval DYNAMIXEL_PARSE_WRONG_CHECKSUM
 * @retval DYNAMIXEL_PARSE_STATUS_ERROR
 * @retval DYNAMIXEL_PARSE_INADEQUATE_DATA
 * @retval DYNAMIXEL_PARSE_HUGE_DATA
 * @retval DYNAMIXEL_PARSE_WRONG_ID
 * @retval DYNAMIXEL_PARSE_NO_RESPONSE
*/
dynamixel_parse_result dynamixel_send_packet(
    dynamixel_t self,
    uint8_t id,
    uint8_t instruction,
    uint16_t parameter_size,
    const uint8_t *parameter,
    uint8_t *error,
    size_t *status_parameter_size,
    uint8_t *status_parameter,
    uint wait_us_multiplier
);



////////////////
// 以下補助関数 //
////////////////

/**
 * @brief dynamixelにパケットを送る
 *
 * @param[in] self dynamixelインスタンス
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
 * @param[in] id パケットを送るDynamixelのID
 * @param[out] *error 応答パケットのエラーステータス
 * @param[out] *status_parameter_size 応答パケットのパラメータのサイズ
 * @param[out] *status_parameter 応答パケットのパラメータ
 * @param[out] *initial_status_packet_size 読み取り結果をバッファーに保存する際の最初の位置
 * @retval DYNAMIXEL_PARSE_SUCCESS
 * @retval DYNAMIXEL_PARSE_WRONG_CHECKSUM
 * @retval DYNAMIXEL_PARSE_STATUS_ERROR
 * @retval DYNAMIXEL_PARSE_INADEQUATE_DATA
*/
dynamixel_parse_result dynamixel_partial_uart_packet(
    dynamixel_t self,
    uint8_t id,
    uint8_t *error,
    size_t *status_parameter_size,
    uint8_t *status_parameter,
    size_t *initial_status_packet_size
);


/**
 * @brief 応答パケットを部分的に読み取って解析する
 *
 * @param[in] self dynamixelインスタンス
 * @param[in] id パケットを送るDynamixelのID
 * @param[out] *error 応答パケットのエラーステータス
 * @param[out] *status_parameter_size 応答パケットのパラメータのサイズ
 * @param[out] *status_parameter 応答パケットのパラメータ
 * @param[out] *initial_status_packet_size 読み取り結果をバッファーに保存する際の最初の位置
 * @retval DYNAMIXEL_PARSE_SUCCESS
 * @retval DYNAMIXEL_PARSE_WRONG_CHECKSUM
 * @retval DYNAMIXEL_PARSE_STATUS_ERROR
 * @retval DYNAMIXEL_PARSE_INADEQUATE_DATA
 * @retval DYNAMIXEL_PARSE_HUGE_DATA
 * @retval DYNAMIXEL_PARSE_WRONG_ID
*/
dynamixel_parse_result dynamixel_partial_read_and_parse_uart_packet(
    dynamixel_t self,
    uint8_t id,
    uint8_t *error,
    size_t *status_parameter_size,
    uint8_t *status_parameter,
    size_t *initial_status_packet_size
);


/**
 * @brief 応答パケットを読み取って解析する
 *
 * @param[in] self dynamixelインスタンス
 * @param[in] id パケットを送るDynamixelのID
 * @param[out] *error 応答パケットのエラーステータス
 * @param[out] *status_parameter_size 応答パケットのパラメータのサイズ
 * @param[out] *status_parameter 応答パケットのパラメータ
 * @param[out] *initial_status_packet_size 読み取り結果をバッファーに保存する際の最初の位置
 * @param[in] wait_us_multiplier 1以上のときcreate時に設定した応答パケットの待ち時間を一時的に、指定した倍数を掛けた値にする
 * @retval DYNAMIXEL_PARSE_SUCCESS
 * @retval DYNAMIXEL_PARSE_WRONG_CHECKSUM
 * @retval DYNAMIXEL_PARSE_STATUS_ERROR
 * @retval DYNAMIXEL_PARSE_INADEQUATE_DATA
 * @retval DYNAMIXEL_PARSE_HUGE_DATA
 * @retval DYNAMIXEL_PARSE_WRONG_ID
 * @retval DYNAMIXEL_PARSE_NO_RESPONSE
*/
dynamixel_parse_result dynamixel_read_uart_packet(
    dynamixel_t self,
    uint8_t id,
    uint8_t *error,
    size_t *status_parameter_size,
    uint8_t *status_parameter,
    uint wait_us_multiplier
);


#ifdef __cplusplus
}
#endif

#endif
