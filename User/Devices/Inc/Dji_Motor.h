/**
 *******************************************************************************
 * @file      : Dji_Motor.h
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
#ifndef __DJI_MOTOR_H_
#define __DJI_MOTOR_H_

#ifdef __cplusplus

/* Includes ------------------------------------------------------------------*/
#include "drv_can.h"
/* Exported macro ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
class Dji_Motor
{
   public:
    int16_t round_cnt;
    int16_t encode;
    int16_t encode_offest;
    int16_t last_encode;
    int16_t speed_rpm;
    int16_t torque_current;
    uint8_t temp;
    uint8_t encoder_init;
    float angle_real;
    void Update(uint8_t can_rx_data[]);

   private:
};

/* Exported variables --------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/
#endif

#endif /* __FILE_H_ */
