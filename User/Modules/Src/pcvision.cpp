/*
 * @Author: Jerry Gong
 * @Date: 2023-02-15 20:15:56
 * @LastEditTime: 2023-07-27 11:05:52
 * @FilePath: \A_Board\User\Devices\Src\PC_Vision.c
 * @Description: 视觉数据收发以及编解码处理
 *
 * Copyright (c) 2023 by USTB Reborn Robot Team, All Rights Reserved.
 */
#include "pcvision.h"

VisionRx vision_rxpack;
uint16_t Vision_lost = 0x00;
int aim_flag = 1;

void VisionPack(uint8_t* pData)
{
    static float last_vision[2];
    static int16_t count;

    if (pData == NULL) {
        return;
    }

    if (pData[0] == 'R' && pData[1] == 'M') {
        if (pData[2] != 'A' && pData[3] != 'A') {
            aim_flag = 1;
            Vision_lost = 0;
            vision_rxpack.yaw_angle = (float)(int16_t)(pData[3] << 8 | pData[2]) / 100.0f;    // yaw
            vision_rxpack.pitch_angle = (float)(int16_t)(pData[5] << 8 | pData[4]) / 100.0f;  // pitch

            last_vision[0] = vision_rxpack.yaw_angle;
            last_vision[1] = vision_rxpack.pitch_angle;
        }

        else {
            Vision_lost++;
        }

        if (Vision_lost == 14) {
            aim_flag = 0;
            vision_rxpack.yaw_angle = 0.0f;
            vision_rxpack.pitch_angle = last_vision[1];
        }
    }
}
