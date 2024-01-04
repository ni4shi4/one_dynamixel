#ifndef _ONE_DYNAMIXEL_PACKET_BYTE_H
#define _ONE_DYNAMIXEL_PACKET_BYTE_H

#include "pico.h"

#ifdef __cplusplus
extern "C" {
#endif

// ヘッダー用のバイト
extern const uint8_t DYNAMIXEL__HEADER_1;
extern const uint8_t DYNAMIXEL__HEADER_2;
extern const uint8_t DYNAMIXEL__HEADER_3;
extern const uint8_t DYNAMIXEL__HEADER_R;
// パラメータ修正用のバイト
extern const uint8_t DYNAMIXEL__HEADER_N;

// インストラクション用のバイト
extern const uint8_t DYNAMIXEL__INSTRUCTION_PING;
extern const uint8_t DYNAMIXEL__INSTRUCTION_READ;
extern const uint8_t DYNAMIXEL__INSTRUCTION_WRITE;
extern const uint8_t DYNAMIXEL__INSTRUCTION_REG_WRITE;
extern const uint8_t DYNAMIXEL__INSTRUCTION_ACTION;
extern const uint8_t DYNAMIXEL__INSTRUCTION_FACTORY_RESET;
extern const uint8_t DYNAMIXEL__INSTRUCTION_REBOOT;
extern const uint8_t DYNAMIXEL__INSTRUCTION_SYNC_READ;
extern const uint8_t DYNAMIXEL__INSTRUCTION_SYNC_WRITE;
extern const uint8_t DYNAMIXEL__INSTRUCTION_BULK_READ;
extern const uint8_t DYNAMIXEL__INSTRUCTION_BULK_WRITE;
extern const uint8_t DYNAMIXEL__INSTRUCTION_STATUS;

// factory resetのパラメーター
typedef enum {
    DYNAMIXEL_FACTORY_RESET_ID = 0x01,
    DYNAMIXEL_FACTORY_RESET_ID_AND_BAUD_RATE = 0x02,
    DYNAMIXEL_FACTORY_RESET_ALL = 0xff,
} dynamixel_factory_reset_paramerter;

typedef enum {
    DYNAMIXEL_OPERATING_MODE_CURRENT_CONTROL = 0x00,
    DYNAMIXEL_OPERATING_MODE_VELOCITY_CONTROL = 0x01,
    DYNAMIXEL_OPERATING_MODE_POSITION_CONTROL = 0x03,
    DYNAMIXEL_OPERATING_MODE_EXTENDED_POSITION_CONTROL = 0x04,
    DYNAMIXEL_OPERATING_MODE_CURRENT_BASED_POSITION_CONTROL = 0x05,
    DYNAMIXEL_OPERATING_MODE_PWM_CONTROL = 0x10,
} dynamixel_operating_mode;

typedef enum {
    DYNAMIXEL_BAUD_RATE_9600 = 0x00,
    DYNAMIXEL_BAUD_RATE_57600 = 0x01,
    DYNAMIXEL_BAUD_RATE_115200 = 0x02,
    DYNAMIXEL_BAUD_RATE_1M = 0x03,
    DYNAMIXEL_BAUD_RATE_2M = 0x04,
    DYNAMIXEL_BAUD_RATE_3M = 0x05,
    DYNAMIXEL_BAUD_RATE_4M = 0x06,
} dynamixel_baud_rate;


/**
 * @brief ボーレートの値からボーレートを示すバイトを返す
 * @param[in] baud_rate ボーレート
 * @param[out] baud_rate_byte ボーレートを示すバイト
 * @retval 0 ボーレートに対応するバイトが見つかった
 * @retval 1 ボーレートに対応するバイトは見つからなかった

*/
int get_baud_rate_byte(
    uint baud_rate,
    dynamixel_baud_rate *baud_rate_byte
);

/**
 * @brief ボーレートを示すバイトから、実際のボーレートを返す
 * @param[in] baud_rate_byte ボーレートを示すバイト
 * @return ボーレート

*/
uint get_baud_rate(
    dynamixel_baud_rate baud_rate_byte
);

#ifdef __cplusplus
}
#endif

#endif
