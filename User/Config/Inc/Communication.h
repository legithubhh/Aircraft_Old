/**
 *******************************************************************************
 * @file      : Communication.h
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
#ifndef __COMMUNICATION_H_
#define __COMMUNICATION_H_

#ifdef __cplusplus

/* Includes ------------------------------------------------------------------*/
#include "CommomInc.h"
#include "drv_can.h"
#include "drv_uart.h"
#include "motortask.h"

/* Exported macro ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/

#endif  // DEBUG

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "gimbal.h"
#include "pcvision.h"
#include "referee.h"
/* Exported macro ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/
void UserCAN1RxCpltCallBack(CAN_RxBuffer* CAN_RxBuffer);
void UserCAN2RxCpltCallBack(CAN_RxBuffer* CAN_RxBuffer);
void UserSystemCommInit();
void RemoteTask();
void RefereeTask();
void VisionReciveTask();
void VisionTransmitTask();

#ifdef __cplusplus
}
#endif

#endif /* __COMMUNICATION_H_ */
