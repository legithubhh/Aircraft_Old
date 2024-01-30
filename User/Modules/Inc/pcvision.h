/**
 *******************************************************************************
 * @file      : pcvision.h
 * @brief     :
 * @history   :
 *  Version     Date            Author          Note
 *  V0.9.0      yyyy-mm-dd      <author>        1. <note>
 *******************************************************************************
 * @attention :
 *******************************************************************************
 *  Copyright (c) 2024 Reborn Team, University of Science and Technology Beijing.
 *  All Rights Reserved.
 *******************************************************************************
 */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PCVISION_H__
#define __PCVISION_H__

#ifdef __cplusplus

/* Includes ------------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/

#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#include "IMU.h"
#include "cmsis_os.h"
#include "drv_uart.h"
#include "usart.h"

/* Exported macro ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

typedef struct
{
    /* data */
    float yaw_angle;
    float pitch_angle;
    float distance;
} VisionRx;

/* Exported variables --------------------------------------------------------*/
extern VisionRx vision_rxpack;
extern int aim_flag;
/* Exported function prototypes ----------------------------------------------*/
void VisionPack(uint8_t *pData);

#ifdef __cplusplus
}
#endif

#endif /* __PCVISION_H__ */
