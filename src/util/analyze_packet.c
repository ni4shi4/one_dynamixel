#include <stdlib.h>
#include <string.h>
#include "util/crc.h"
#include "util/analyze_packet.h"
#include "util/packet_byte.h"



/**
 * @brief 2バイトデータを1バイトずつに分割する(リトルエンディアン)
 * @param[in] input 分割対象の2バイトデータ
 * @param[out] byte_l 分割後の1バイト目のデータ
 * @param[out] byte_r 分割後の2バイト目のデータ
*/
static void divide_into_byte_pair(
    uint16_t input,
    uint8_t *byte_l,
    uint8_t *byte_r
)
{
    *byte_l = input & 0xff;
    *byte_r = input >> 8;
}

/**
 * @brief 1バイトずつのデータを結合して1つの変数に格納する(リトルエンディアン)
 * @param[in] byte_l 1バイト目のデータ
 * @param[in] byte_r 2バイト目のデータ
*/
static uint16_t combine_byte_pair(
    uint8_t byte_l,
    uint8_t byte_r
)
{
    return (byte_r << 8) | byte_l;
}


int create_uart_packet(
    uint8_t *packet,
    uint8_t id,
    uint8_t instruction,
    const uint8_t *parameter,
    uint16_t parameter_size
)
{
    uint8_t *fixed_parameter;
    uint16_t length, fixed_parameter_size;
    uint16_t checksum;
    int packet_size;


    // ヘッダーの代入
    packet[0] = DYNAMIXEL__HEADER_1;
    packet[1] = DYNAMIXEL__HEADER_2;
    packet[2] = DYNAMIXEL__HEADER_3;
    packet[3] = DYNAMIXEL__HEADER_R;

    // IDの代入
    packet[4] = id;

    // 最大でも3つに1回の頻度で1バイトを追加すれば良いので、サイズは4/3倍あれば十分
    fixed_parameter = (uint8_t *)malloc(parameter_size * 4 / 3);
    // パラメータの修正を行う(ヘッダーと同じ部分が出たらバイトを追加で付与する)
    if (parameter_size >= 3)
    {
        fixed_parameter[0] = parameter[0];
        fixed_parameter[1] = parameter[1];
        fixed_parameter_size = 2;
        for (int i = 2; i < parameter_size; i++)
        {
            fixed_parameter[fixed_parameter_size] = parameter[i];
            if (
                parameter[i - 2] == DYNAMIXEL__HEADER_1
                && parameter[i - 1] == DYNAMIXEL__HEADER_2
                && parameter[i] == DYNAMIXEL__HEADER_3
            )
            {
                // ヘッダーと同じ部分が出たらバイトを追加で付与する
                fixed_parameter_size++;
                fixed_parameter[fixed_parameter_size] = DYNAMIXEL__HEADER_N;
            }

            fixed_parameter_size++;
        }
    }
    else
    {
        fixed_parameter_size = parameter_size;
        // ヘッダーと同じ部分は現れないので、コピーのみ行う
        memcpy(fixed_parameter, parameter, fixed_parameter_size);
    }

    // length、パケットのサイズを計算
    length = fixed_parameter_size + 1 + 2;
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
    memcpy(packet + 8, fixed_parameter, fixed_parameter_size);

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


int parse_uart_packet(
    const uint8_t *packet,
    int packet_size,
    int *header_position,
    uint8_t *id,
    uint8_t *instruction,
    uint8_t *error,
    uint8_t *parameter,
    size_t *parameter_size
)
{
    int i, length, parameter_position;
    size_t parameter_size_l;
    uint16_t packet_crc, compute_crc;

    // ヘッダーの位置を探す
    for (i = 0; i < packet_size - 3; i++)
    {
        if (
            packet[i] == DYNAMIXEL__HEADER_1
            && packet[i + 1] == DYNAMIXEL__HEADER_2
            && packet[i + 2] == DYNAMIXEL__HEADER_3
            && packet[i + 3] == DYNAMIXEL__HEADER_R
        )
        {
            *header_position = i;
            break;
        }
    }

    if (i + 3 < packet_size)
    {
        // ヘッダーが見つかった
        if (i + 4 + 3 < packet_size)
        {
            // 4 : ヘッダー分のオフセット、3 : id + length分のオフセット
            *id = packet[i + 4];
            length = combine_byte_pair(packet[i + 5], packet[i + 6]);
            parameter_size_l = length - 4;

            if (i + 4 + 3 + length <= packet_size)
            {
                // すべてのデータがパケット内に存在
                *instruction = packet[i + 7];
                *error = packet[i + 8];

                //パラメータの修正をする
                if (parameter_size_l > 3)
                {
                    parameter_position = 3;
                    memcpy(parameter, packet + i + 4 + 3 + 2, parameter_position);
                    for (int j = 3; j < parameter_size_l; j++)
                    {
                        if (
                            packet[i + 4 + 3 + 2 + j - 3] != DYNAMIXEL__HEADER_1
                            || packet[i + 4 + 3 + 2 + j - 2] != DYNAMIXEL__HEADER_2
                            || packet[i + 4 + 3 + 2 + j - 1] != DYNAMIXEL__HEADER_3
                        )
                        {
                            /*
                            3個前 ~ 1個前がヘッダーと一致しなければ有効なバイト
                            上記以外は無効なバイト(例外処理で挿入したもの)のため、スキップする
                            */
                            parameter[parameter_position] = packet[i + 4 + 3 + 2 + j];
                            parameter_position++;
                        }
                    }
                    *parameter_size = parameter_position;
                }
                else
                {
                    *parameter_size = parameter_size_l;
                    memcpy(parameter, packet + i + 4 + 3 + 2, parameter_size_l);
                }

                packet_crc = combine_byte_pair(
                    packet[i + 4 + 3 + length - 2],
                    packet[i + 4 + 3 + length - 1]
                );
                // データからCRCを計算する
                compute_crc = crc_16_ibm(packet + i, 4 + 3 + length - 2);

                // CRCを比較してデータに誤りがないかを確認する
                if (compute_crc == packet_crc)
                {
                    return 0;
                }
                else
                {
                    return 1;
                }
            }
            else
            {
                // 一部データがパケットにない
                return 2;
            }
        }
        else
        {
            // length以降のデータがない
            return 2;
        }
    }
    else
    {
        return 3;
    }
}
