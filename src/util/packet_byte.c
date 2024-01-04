#include "util/packet_byte.h"

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


int get_baud_rate_byte(
    uint baud_rate,
    dynamixel_baud_rate *baud_rate_byte
)
{
    switch (baud_rate)
    {
    case 9600:
        *baud_rate_byte = DYNAMIXEL_BAUD_RATE_9600;
        break;
    case 57600:
        *baud_rate_byte = DYNAMIXEL_BAUD_RATE_57600;
        break;
    case 115200:
        *baud_rate_byte = DYNAMIXEL_BAUD_RATE_115200;
        break;
    case 1000000:
        *baud_rate_byte = DYNAMIXEL_BAUD_RATE_1M;
        break;
    case 2000000:
        *baud_rate_byte = DYNAMIXEL_BAUD_RATE_2M;
        break;
    case 3000000:
        *baud_rate_byte = DYNAMIXEL_BAUD_RATE_3M;
        break;
    case 4000000:
        *baud_rate_byte = DYNAMIXEL_BAUD_RATE_4M;
        break;
    
    default:
        return 1;
        break;
    }

    return 0;
}

uint get_baud_rate(
    dynamixel_baud_rate baud_rate_byte
)
{
    switch (baud_rate_byte)
    {
    case DYNAMIXEL_BAUD_RATE_9600:
        return 9600;
    case DYNAMIXEL_BAUD_RATE_57600:
        return 57600;
    case DYNAMIXEL_BAUD_RATE_115200:
        return 115200;
    case DYNAMIXEL_BAUD_RATE_1M:
        return 1000000;
    case DYNAMIXEL_BAUD_RATE_2M:
        return 2000000;
    case DYNAMIXEL_BAUD_RATE_3M:
        return 3000000;
    case DYNAMIXEL_BAUD_RATE_4M:
        return 4000000;
    
    default:
        return 0;
    }
}
