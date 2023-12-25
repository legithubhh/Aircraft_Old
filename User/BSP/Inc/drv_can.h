/**
 *******************************************************************************
 * @file      : drv_can.h
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
#ifndef __DRV_CAN_H_
#define __DRV_CAN_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "can.h"
/* Exported macro ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
typedef struct
{
    CAN_RxHeaderTypeDef rx_header;
    uint8_t rx_data[8];
} CAN_RxBuffer;

typedef struct
{
    uint16_t ID;
    uint8_t DLC;
    uint16_t RTR;
    uint8_t Data[8];
} CANx_Message;

/* Exported variables --------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/
void CANx_PackProcess_Data(CAN_HandleTypeDef* hcan, int16_t ID, int16_t DLC, int16_t data1, int16_t data2, int16_t data3, int16_t data4);
void CANx_PackProcess_Remote(CAN_HandleTypeDef* hcan, int16_t ID);
void CANx_PackProcess_DM(CAN_HandleTypeDef* hcan, int16_t ID, int16_t DLC, int16_t data1, int16_t data2, int16_t data3, int16_t data4, int16_t data5);
void CANInit(CAN_HandleTypeDef* hcan, void (*pFunc)(CAN_RxBuffer*));

#ifdef __cplusplus
}
#endif

#endif /* __FILE_H_ */
