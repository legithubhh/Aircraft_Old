/**
 *******************************************************************************
 * @file      : IMU.cpp
 * @brief     :
 * @history   :
 *  Version     Date            Author          Note
 *  V0.9.0      2023-05-01     Jerry Gong      First
 *******************************************************************************
 * @attention :
 *******************************************************************************
 *  Copyright (c) 2023 Reborn Team, USTB.
 *  All Rights Reserved.
 *******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/
#include "IMU.h"

/* Private macro -------------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
IMU_Def IMU;
/* Private function prototypes -----------------------------------------------*/

/**
 * @brief
 * @param       *pData:
 *   @arg       None
 * @retval      None
 * @note        None
 */
void IMU_Def::DataPack(uint8_t *pData)
{
    static float Last_Euler = 0.0f;
    static float Now_Euler = 0.0f;

    Accel[0] = ((float)(int16_t)(pData[9] + (pData[10] << 8)));
    Accel[1] = ((float)(int16_t)(pData[11] + (pData[12] << 8)));
    Accel[2] = ((float)(int16_t)(pData[13] + (pData[14] << 8)));

    Gyro[0] = ((float)(int16_t)(pData[16] + (pData[17] << 8)));
    Gyro[1] = ((float)(int16_t)(pData[18] + (pData[19] << 8)));
    Gyro[2] = ((float)(int16_t)(pData[20] + (pData[21] << 8)));

    Euler[0] = ((float)(int16_t)(pData[23] + (pData[24] << 8))) / 100;
    Euler[1] = -((float)(int16_t)(pData[25] + (pData[26] << 8))) / 100;
    Yaw = ((float)(int16_t)(pData[27] + (pData[28] << 8))) / 10;

    Last_Euler = Now_Euler;
    Now_Euler = Yaw;

    if (Now_Euler - Last_Euler > 300.0f) {
        count--;
    } else if (Now_Euler - Last_Euler < -300.0f) {
        count++;
    }

    Euler[2] = Now_Euler + count * 360.0f;
}
