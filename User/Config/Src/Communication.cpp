/**
 *******************************************************************************
 * @file      : Communication.cpp
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
#include "Communication.h"

/* Private macro -------------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private types -------------------------------------------------------------*/
uint32_t DR16CallBack();
void UserCAN1DataUpdate(CANx_Message* CANx_RxMsg);
void UserCAN2DataUpdate(CANx_Message* CANx_RxMsg);

/* Private variables ---------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/**
 * @brief
 *   @arg       None
 * @retval      None
 * @note        None
 */
void UserSystemCommInit()
{
    CANInit(&hcan1, UserCAN1RxCpltCallBack);
    CANInit(&hcan2, UserCAN2RxCpltCallBack);
    UserUARTReceiveIT(&huart3, 18, DR16CallBack);
}

/**
 * @brief       CAN1 call back function
 * @param       CAN_RxBuffer:
 *   @arg       None
 * @retval      None
 * @note        None
 */
void UserCAN1RxCpltCallBack(CAN_RxBuffer* CAN_RxBuffer)
{
    static CANx_Message CAN1_RxMessage;
    CAN1_RxMessage.ID = CAN_RxBuffer->rx_header.StdId;
    CAN1_RxMessage.DLC = CAN_RxBuffer->rx_header.DLC;
    memcpy(CAN1_RxMessage.Data, CAN_RxBuffer->rx_data, CAN1_RxMessage.DLC);

    UserCAN1DataUpdate(&CAN1_RxMessage);
}

/**
 * @brief       CAN2 call back function
 * @param       CAN_RxBuffer:
 *   @arg       None
 * @retval      None
 * @note        None
 */
void UserCAN2RxCpltCallBack(CAN_RxBuffer* CAN_RxBuffer)
{
    static CANx_Message CAN2_RxMessage;
    CAN2_RxMessage.ID = CAN_RxBuffer->rx_header.StdId;
    CAN2_RxMessage.DLC = CAN_RxBuffer->rx_header.DLC;
    memcpy(CAN2_RxMessage.Data, CAN_RxBuffer->rx_data, CAN2_RxMessage.DLC);

    UserCAN2DataUpdate(&CAN2_RxMessage);
}

/**
 * @brief       将CAN1总线数据更新到云台的两个3508和两个2006电机
 * @param       CANx_RxMsg:
 *   @arg       None
 * @retval      None
 * @note        None
 */
void UserCAN1DataUpdate(CANx_Message* CANx_RxMsg)
{
    switch (CANx_RxMsg->ID) {
        case CAN_FRICTION1_3508_ID:
        case CAN_FRICTION2_3508_ID:
        case CAN_TRIGGER_2006_ID:
        case CAN_PITCH_2006_ID:
        case CAN_YAW_6020_ID: {
            static uint8_t i = 0;
            // get motor id
            i = CANx_RxMsg->ID - CAN_FRICTION1_3508_ID;

            /**
             * 得到CAN1总线对应电机ID返回数据的全局变量，编号0-3分别为摩擦轮3508电机1，摩擦轮3508电机2，拨弹盘2006电机，Pitch轴2006电机
             */
            motor_measure[i].Update(CANx_RxMsg->Data);
            break;
        }

        default: {
            break;
        }
    }
}

/**
 * @brief       更新CAN2总线数据
 * @param       CANx_RxMsg:
 *   @arg       None
 * @retval      None
 * @note        None
 */
void UserCAN2DataUpdate(CANx_Message* CANx_RxMsg)
{
}

/**
 * @brief       Remote call back function
 *   @arg       None
 * @retval      None
 * @note        None
 */
uint32_t DR16CallBack()
{
    if (remoteTaskHandle != NULL) {
        osSignalSet(remoteTaskHandle, (uint32_t)UART_Instance[1].rx_buffer);
    }
    return 0;
}

/**
 * @brief       Remote receive function
 *   @arg       None
 * @retval      None
 * @note        None
 */
void RemoteTask()
{
    static uint8_t* Dbus_Buffer;
    static TickType_t xLastWakeTime_t = pdMS_TO_TICKS(10);

    if (xTaskNotifyWait(0x00000000, 0xFFFFFFFF, (uint32_t*)&Dbus_Buffer, xLastWakeTime_t) == pdTRUE) {
        Remote.SbusToRc(Dbus_Buffer);
    }
}
