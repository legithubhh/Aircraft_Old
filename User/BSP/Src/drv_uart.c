/**
 *******************************************************************************
 * @file      : drv_uart.c
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
#include "drv_uart.h"

/* Private macro -------------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
Uart_RX_Message UART_Instance[3];

/* Private function prototypes -----------------------------------------------*/
/**
 * @brief
 * @param       *huart:
 * @param       length:
 * @param       Function:
 *   @arg       None
 * @retval      None
 * @note        None
 */
void UserUARTReceiveIT(UART_HandleTypeDef *huart, uint32_t length, UART_CallBack Function)
{
    if (huart->Instance == USART1) {
        UART_Instance[0].huart = huart;
        UART_Instance[0].rx_buffer_size = length;
        UART_Instance[0].Function = Function;
        HAL_UARTEx_ReceiveToIdle_DMA(huart, UART_Instance[0].rx_buffer, length);
        __HAL_DMA_DISABLE_IT(UART_Instance[0].huart->hdmarx, DMA_IT_HT);

    }

    else if (huart->Instance == USART3) {
        UART_Instance[1].huart = huart;
        UART_Instance[1].rx_buffer_size = length;
        UART_Instance[1].Function = Function;
        HAL_UARTEx_ReceiveToIdle_DMA(UART_Instance[1].huart, UART_Instance[1].rx_buffer, UART_Instance[1].rx_buffer_size);
        __HAL_DMA_DISABLE_IT(UART_Instance[1].huart->hdmarx, DMA_IT_HT);

    }

    else if (huart->Instance == USART6) {
        UART_Instance[2].huart = huart;
        UART_Instance[2].rx_buffer_size = length;
        UART_Instance[2].Function = Function;
        HAL_UARTEx_ReceiveToIdle_DMA(UART_Instance[2].huart, UART_Instance[2].rx_buffer, UART_Instance[2].rx_buffer_size);
        __HAL_DMA_DISABLE_IT(UART_Instance[2].huart->hdmarx, DMA_IT_HT);
    }
}

/**
 * @brief
 * @param       *huart:
 * @param       Size:
 *   @arg       None
 * @retval      None
 * @note        None
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    for (uint8_t i = 0; i < 4; ++i) {
        if (huart == UART_Instance[i].huart) {
            if (UART_Instance[i].Function != NULL) {
                UART_Instance[i].Function();
            }
            HAL_UARTEx_ReceiveToIdle_DMA(UART_Instance[i].huart, UART_Instance[i].rx_buffer, UART_Instance[i].rx_buffer_size);
            __HAL_DMA_DISABLE_IT(UART_Instance[i].huart->hdmarx, DMA_IT_HT);
            return;
        }
    }
}

/**
 * @brief
 * @param       *huart:
 * @param       *send_buf:
 * @param       send_size:
 * @param       MODE:
 *   @arg       None
 * @retval      None
 * @note        None
 */
void UserUARTSendData(UART_HandleTypeDef *huart, uint8_t *send_buf, uint16_t send_size, UART_TRANSMIT_MODE MODE)
{
    if (MODE == UART_TRAMSMIT_BLOCKING) {
        HAL_UART_Transmit(huart, send_buf, send_size, 10);
    } else if (MODE == UART_TRANSMIT_IT) {
        HAL_UART_Transmit_IT(huart, send_buf, send_size);
    } else if (MODE == UART_TRAMSMIT_DMA) {
        HAL_UART_Transmit_DMA(huart, send_buf, send_size);
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    for (uint8_t i = 0; i < 4; ++i) {
        if (huart == UART_Instance[i].huart) {
            HAL_UARTEx_ReceiveToIdle_DMA(UART_Instance[i].huart, UART_Instance[i].rx_buffer, UART_Instance[i].rx_buffer_size);
            __HAL_DMA_DISABLE_IT(UART_Instance[i].huart->hdmarx, DMA_IT_HT);
            return;
        }
    }
}
