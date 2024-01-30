/**
 *******************************************************************************
 * @file      : Communication.cpp
 * @brief     :此文件用于CAN，USART通讯（包括遥控器，裁判系统和摄像头）收发任务函数的配置和线程编写
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
uint32_t RefereeCallback();
uint32_t VisionReciveCallback(void);
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
    UserUARTReceiveIT(&huart1, 256, RefereeCallback);
    UserUARTReceiveIT(&huart6, 9, VisionReciveCallback);
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

/**
 * @brief       Referee receive&Transmit function
 *   @arg       None
 * @retval      None
 * @note        None
 */
void RefereeTask()
{
    uint8_t* Referee_Buffer;
    TickType_t xLastWakeTime_t = pdMS_TO_TICKS(10);

    if (xTaskNotifyWait(0x00000000, 0xFFFFFFFF, (uint32_t*)&Referee_Buffer, xLastWakeTime_t) == pdTRUE) {
        RefereePack(Referee_Buffer);
    }
}

/**
 * @brief       Referee call back function
 *   @arg       None
 * @retval      None
 * @note        None
 */
uint32_t RefereeCallback()
{
    static BaseType_t* pxHigherPriorityTaskWoken;
    if (refereeTaskHandle != NULL) {
        xTaskNotifyFromISR(refereeTaskHandle, (uint32_t)UART_Instance[3].rx_buffer, eSetValueWithOverwrite, pxHigherPriorityTaskWoken);
    }
    return 0;
}

/**
 * @brief       Vision receive function
 *   @arg       None
 * @retval      None
 * @note        None
 */
void VisionReciveTask()
{
    uint8_t* Vision_Buffer;
    TickType_t xLastWakeTime_t = pdMS_TO_TICKS(10);

    if (xTaskNotifyWait(0x00000000, 0xFFFFFFFF, (uint32_t*)&Vision_Buffer, xLastWakeTime_t) == pdTRUE) {
        VisionPack(Vision_Buffer);
    }
}

/**
 * @brief       Vision call back function
 *   @arg       None
 * @retval      None
 * @note        None
 */
uint32_t VisionReciveCallback(void)
{
    static BaseType_t* pxHigherPriorityTaskWoken;
    if (visionReciveTaskHandle != NULL) {
        xTaskNotifyFromISR(visionReciveTaskHandle, (uint32_t)UART_Instance[2].rx_buffer, eSetValueWithOverwrite, pxHigherPriorityTaskWoken);
    }
    return 0;
}

/**
 * @brief       Vision transmit function
 *   @arg       None
 * @retval      None
 * @note        None
 */
void VisionTransmitTask()
{
    uint8_t Tx[14];
    int SendAngle[2];

    SendAngle[0] = (int)((INS.Pitch + 180) * 10);  // pitch
    SendAngle[1] = (int)((INS.Yaw + 180) * 10);    // yaw

    Tx[0] = 'R';
    Tx[1] = 'M';
    Tx[2] = 'S';
    Tx[3] = 'L';

    Tx[4] = SendAngle[1] / 1000 + 48;
    Tx[5] = SendAngle[1] % 1000 / 100 + 48;
    Tx[6] = SendAngle[1] % 100 / 10 + 48;
    Tx[7] = SendAngle[1] % 10 + 48;

    Tx[8] = SendAngle[0] / 1000 + 48;
    Tx[9] = SendAngle[0] % 1000 / 100 + 48;
    Tx[10] = SendAngle[0] % 100 / 10 + 48;
    Tx[11] = SendAngle[0] % 10 + 48;

    UserUARTSendData(&huart6, Tx, 13u, UART_TRANSMIT_IT);
    vTaskDelay(10);
}
