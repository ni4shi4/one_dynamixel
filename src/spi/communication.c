#include <stdlib.h>
#include <string.h>
#include "util/byte.h"
#include "util/crc.h"
#include "spi/communication.h"
#include "spi/packet_byte.h"


int create_spi_packet(
    uint8_t *packet,
    uint8_t id,
    uint16_t parameter_length,
    uint8_t instruction,
    const uint8_t *parameter
)
{
    uint8_t *fixed_parameter;
    uint16_t length, fixed_parameter_length;
    uint16_t checksum;
    int packet_size;


    // ヘッダーの代入
    packet[0] = HEADER_1;
    packet[1] = HEADER_2;
    packet[2] = HEADER_3;
    packet[3] = HEADER_R;

    // IDの代入
    packet[4] = id;

    // パラメータの修正を行う(ヘッダーと同じ部分が出たらバイトを追加で付与する)
    if (parameter_length >= 3)
    {
        // 最大でも3つに1回の頻度で1バイトを追加すれば良いので、サイズは4/3倍あれば十分
        fixed_parameter = (uint8_t *)malloc(parameter_length * 4 / 3);
        fixed_parameter[0] = parameter[0];
        fixed_parameter[1] = parameter[1];
        fixed_parameter_length = 2;
        for (int i = 2; i < parameter_length; i++)
        {
            fixed_parameter[fixed_parameter_length] = parameter[i];
            if (
                parameter[i - 2] == HEADER_1
                && parameter[i - 1] == HEADER_2
                && parameter[i] == HEADER_3
            )
            {
                // ヘッダーと同じ部分が出たらバイトを追加で付与する
                fixed_parameter_length++;
                fixed_parameter[fixed_parameter_length] = HEADER_N;
            }

            fixed_parameter_length++;
        }
    }
    else
    {
        fixed_parameter_length = parameter_length;
        // ヘッダーと同じ部分は現れないので、コピーのみ行う
        memcpy(fixed_parameter, parameter, fixed_parameter_length);
    }

    // length、パケットのサイズを計算
    length = fixed_parameter_length + 1 + 2;
    packet_size = 4 + 1 + 2 + length;

    // lengthの代入
    divide_into_byte_pair(
        length,
        packet + 5,
        packet + 6
    );

    // インストラクションの代入
    packet[7] = instruction;

    // パラメータの代入
    memcpy(packet + 8, fixed_parameter, fixed_parameter_length);

    free(fixed_parameter);

    // 上で代入したデータに対するchecksumの計算・代入
    checksum = crc_16_ibm(packet, packet_size - 2);
    divide_into_byte_pair(
        checksum,
        packet + packet_size - 2,
        packet + packet_size - 1
    );

    return packet_size;
}


bool parse_spi_packet(
    const uint8_t *packet,
    uint8_t *id,
    uint16_t *length,
    uint8_t *instruction,
    uint8_t *error,
    uint8_t *parameter
)
{
    return true;
}
