/**
 *******************************************************************************
 * @file      : Dji_Motor.cpp
 * @brief     :
 * @history   :
 *  Version     Date            Author          Note
 *  V0.9.0      yyyy-mm-dd      <author>        1. <note>
 *******************************************************************************
 * @attention :
 *******************************************************************************
 *  Copyright (c) 2023 Reborn Team, USTB.
 *  All Rights Reserved.
 *******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/
#include "Dji_Motor.h"
/* Private macro -------------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/**
 * @brief
 * @param       can_rx_data:
 *   @arg       None
 * @retval      None
 * @note        None
 */
void Dji_Motor::Update(uint8_t can_rx_data[])
{
    encode = can_rx_data[0] << 8 | can_rx_data[1];
    speed_rpm = can_rx_data[2] << 8 | can_rx_data[3];
    torque_current = can_rx_data[4] | can_rx_data[5];
    temp = can_rx_data[6];

    if (encoder_init == 1) {
        if (encode - last_encode > 4096) {
            round_cnt--;
        } else if (encode - last_encode < -4096) {
            round_cnt++;
        }
    } else {
        encode_offest = encode;
        encoder_init = 1;
    }

    last_encode = encode;
    int32_t encode_now = round_cnt * 8192 + encode - encode_offest;
    angle_real = (encode_now / 8192.0f) * 360.0f;
}
