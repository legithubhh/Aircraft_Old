/**
 *******************************************************************************
 * @file      : drv_can.c
 * @brief     :
 * @history   :
 *  Version     Date            Author          Note
 *  V1.0.0      2023-05-01    Jerry Gong        1. <note>
 *******************************************************************************
 * @attention :
 *******************************************************************************
 *  Copyright (c) 2023 Reborn Team, USTB.
 *  All Rights Reserved.
 *******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/
#include "drv_can.h"

/* Private macro -------------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

static void (*pCAN1_RxCpltCallback)(CAN_RxBuffer*);
static void (*pCAN2_RxCpltCallback)(CAN_RxBuffer*);

/* External variables --------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

void CANInit(CAN_HandleTypeDef* hcan, void (*pFunc)(CAN_RxBuffer*))
{
    if (hcan->Instance == CAN1) {
        pCAN1_RxCpltCallback = pFunc;
    } else if (hcan->Instance == CAN2) {
        pCAN2_RxCpltCallback = pFunc;
    }
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan)
{
    static CAN_RxBuffer CAN_Rxbuffer;
    if (hcan->Instance == CAN1) {
        HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &CAN_Rxbuffer.rx_header, CAN_Rxbuffer.rx_data);
        pCAN1_RxCpltCallback(&CAN_Rxbuffer);
    } else if (hcan->Instance == CAN2) {
        HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &CAN_Rxbuffer.rx_header, CAN_Rxbuffer.rx_data);
        pCAN2_RxCpltCallback(&CAN_Rxbuffer);
    }
}

/**
 * @brief
 * @param       hcan:
 * @param       ID:
 * @param       RTR:
 * @param       pData:
 * @param       DLC:
 *   @arg       None
 * @retval      None
 * @note        None
 */
void CANx_Send(CAN_HandleTypeDef* hcan, uint16_t ID, uint16_t RTR, uint8_t* pData, uint16_t DLC)
{
    static CAN_TxHeaderTypeDef tx_header;

    tx_header.StdId = ID;
    tx_header.ExtId = 0;
    tx_header.IDE = CAN_ID_STD;
    tx_header.RTR = RTR;
    tx_header.DLC = DLC;

    if (HAL_CAN_AddTxMessage(hcan, &tx_header, pData, (uint32_t*)CAN_TX_MAILBOX0) != HAL_OK) {
        if (HAL_CAN_AddTxMessage(hcan, &tx_header, pData, (uint32_t*)CAN_TX_MAILBOX1) != HAL_OK) {
            HAL_CAN_AddTxMessage(hcan, &tx_header, pData, (uint32_t*)CAN_TX_MAILBOX2);
        }
    }
}

/**
 * @brief
 * @param       hcan:
 * @param       ID:
 * @param       DLC:
 * @param       data1:
 * @param       data2:
 * @param       data3:
 * @param       data4:
 *   @arg       None
 * @retval      None
 * @note        None
 */
void CANx_PackProcess_Data(CAN_HandleTypeDef* hcan, int16_t ID, int16_t DLC, int16_t data1, int16_t data2, int16_t data3, int16_t data4)
{
    CANx_Message CAN1_Pack;
    CANx_Message CAN2_Pack;

    if (hcan->Instance == CAN1) {
        CAN1_Pack.ID = ID;
        CAN1_Pack.RTR = CAN_RTR_DATA;
        CAN1_Pack.DLC = DLC;
        CAN1_Pack.Data[0] = (unsigned char)(data1 >> 8);
        CAN1_Pack.Data[1] = (unsigned char)data1;
        CAN1_Pack.Data[2] = (unsigned char)(data2 >> 8);
        CAN1_Pack.Data[3] = (unsigned char)data2;
        CAN1_Pack.Data[4] = (unsigned char)(data3 >> 8);
        CAN1_Pack.Data[5] = (unsigned char)data3;
        CAN1_Pack.Data[6] = (unsigned char)(data4 >> 8);
        CAN1_Pack.Data[7] = (unsigned char)data4;

        CANx_Send(&hcan1, ID, CAN1_Pack.RTR, CAN1_Pack.Data, CAN1_Pack.DLC);
    }

    else if (hcan->Instance == CAN2) {
        CAN2_Pack.ID = ID;
        CAN2_Pack.RTR = CAN_RTR_DATA;
        CAN2_Pack.DLC = DLC;
        CAN2_Pack.Data[0] = (unsigned char)(data1 >> 8);
        CAN2_Pack.Data[1] = (unsigned char)data1;
        CAN2_Pack.Data[2] = (unsigned char)(data2 >> 8);
        CAN2_Pack.Data[3] = (unsigned char)data2;
        CAN2_Pack.Data[4] = (unsigned char)(data3 >> 8);
        CAN2_Pack.Data[5] = (unsigned char)data3;
        CAN2_Pack.Data[6] = (unsigned char)(data4 >> 8);
        CAN2_Pack.Data[7] = (unsigned char)data4;

        CANx_Send(&hcan2, ID, CAN2_Pack.RTR, CAN2_Pack.Data, CAN2_Pack.DLC);
    }
}

/**
 * @brief
 * @param       hcan:
 * @param       ID:
 *   @arg       None
 * @retval      None
 * @note        None
 */
void CANx_PackProcess_Remote(CAN_HandleTypeDef* hcan, int16_t ID)
{
    CANx_Message CAN1_Pack;
    CANx_Message CAN2_Pack;

    if (hcan->Instance == CAN1) {
        CAN1_Pack.ID = ID;
        CAN1_Pack.RTR = CAN_RTR_REMOTE;
        CAN1_Pack.DLC = 0x00;
        CANx_Send(&hcan1, ID, CAN1_Pack.RTR, CAN1_Pack.Data, CAN1_Pack.DLC);
    }

    else if (hcan->Instance == CAN2) {
        CAN2_Pack.ID = ID;
        CAN2_Pack.RTR = CAN_RTR_REMOTE;
        CAN2_Pack.DLC = 0x00;
        CANx_Send(&hcan2, ID, CAN2_Pack.RTR, CAN2_Pack.Data, CAN2_Pack.DLC);
    }
}

void CANx_PackProcess_DM(CAN_HandleTypeDef* hcan, int16_t ID, int16_t DLC, int16_t data1, int16_t data2, int16_t data3, int16_t data4, int16_t data5)
{
    CANx_Message CAN1_Pack;
    CANx_Message CAN2_Pack;

    if (hcan->Instance == CAN1) {
        CAN1_Pack.ID = ID;
        CAN1_Pack.RTR = CAN_RTR_DATA;
        CAN1_Pack.DLC = DLC;
        CAN1_Pack.Data[0] = data1 >> 8;
        CAN1_Pack.Data[1] = data1;
        CAN1_Pack.Data[2] = data2 >> 4;
        CAN1_Pack.Data[3] = (data2 & 0xF) << 4 | data3 >> 8;
        CAN1_Pack.Data[4] = data3;
        CAN1_Pack.Data[5] = data4 >> 4;
        CAN1_Pack.Data[6] = (data4 & 0xF) << 4 | data5 >> 8;
        CAN1_Pack.Data[7] = data5;

        CANx_Send(&hcan1, ID, CAN1_Pack.RTR, CAN1_Pack.Data, CAN1_Pack.DLC);
    }

    else if (hcan->Instance == CAN2) {
        CAN2_Pack.ID = ID;
        CAN2_Pack.RTR = CAN_RTR_DATA;
        CAN2_Pack.DLC = DLC;
        CAN1_Pack.Data[0] = data1 >> 8;
        CAN1_Pack.Data[1] = data1;
        CAN1_Pack.Data[2] = data2 >> 4;
        CAN1_Pack.Data[3] = (data2 & 0xF) << 4 | data3 >> 8;
        CAN1_Pack.Data[4] = data3;
        CAN1_Pack.Data[5] = data4 >> 4;
        CAN1_Pack.Data[6] = (data4 & 0xF) << 4 | data5 >> 8;
        CAN1_Pack.Data[7] = data5;

        CANx_Send(&hcan2, ID, CAN2_Pack.RTR, CAN2_Pack.Data, CAN2_Pack.DLC);
    }
}