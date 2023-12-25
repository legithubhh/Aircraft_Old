/**
 *******************************************************************************
 * @file      : IMU.h
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __IMU_H_
#define __IMU_H_

#ifdef __cplusplus

/* Includes ------------------------------------------------------------------*/
#include "cmsis_os.h"
#include "drv_uart.h"

/* Exported macro ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
class IMU_Def
{
   public:
    float Accel[3];
    float Gyro[3];
    float Euler[3];
    float Yaw;
    int count;
    void DataPack(uint8_t *pData);
};

/* Exported variables --------------------------------------------------------*/
extern IMU_Def IMU;

/* Exported function prototypes ----------------------------------------------*/

#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* __FILE_H_ */
