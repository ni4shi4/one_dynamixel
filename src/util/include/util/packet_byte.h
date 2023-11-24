#ifndef _ONE_DYNAMIXEL_PACKET_BYTE_H
#define _ONE_DYNAMIXEL_PACKET_BYTE_H

#include "pico.h"

#ifdef __cplusplus
extern "C" {
#endif

// ヘッダー用のバイト
const uint8_t DYNAMIXEL__HEADER_1 = 0xff;
const uint8_t DYNAMIXEL__HEADER_2 = 0xff;
const uint8_t DYNAMIXEL__HEADER_3 = 0xfd;
const uint8_t DYNAMIXEL__HEADER_R = 0x00;
// パラメータ修正用のバイト
const uint8_t DYNAMIXEL__HEADER_N = 0xfd;

// インストラクション用のバイト
const uint8_t DYNAMIXEL__INSTRUCTION_PING = 0x01;
const uint8_t DYNAMIXEL__INSTRUCTION_READ = 0x02;
const uint8_t DYNAMIXEL__INSTRUCTION_WRITE = 0x03;
const uint8_t DYNAMIXEL__INSTRUCTION_REG_WRITE = 0x04;
const uint8_t DYNAMIXEL__INSTRUCTION_ACTION = 0x05;
const uint8_t DYNAMIXEL__INSTRUCTION_FACTORY_RESET = 0x06;
const uint8_t DYNAMIXEL__INSTRUCTION_REBOOT = 0x08;
const uint8_t DYNAMIXEL__INSTRUCTION_SYNC_READ = 0x82;
const uint8_t DYNAMIXEL__INSTRUCTION_SYNC_WRITE = 0x83;
const uint8_t DYNAMIXEL__INSTRUCTION_BULK_READ = 0x92;
const uint8_t DYNAMIXEL__INSTRUCTION_BULK_WRITE = 0x93;
const uint8_t DYNAMIXEL__INSTRUCTION_STATUS = 0x55;

#ifdef __cplusplus
}
#endif

#endif
