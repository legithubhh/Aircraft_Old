/**
 *******************************************************************************
 * @file      : drv_uart.h
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
#ifndef __DRV_UART_H_
#define __DRV_UART_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "usart.h"
/* Exported macro ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define DBUS_BUFLEN 18
#define IMU_BUFLEN 29

/* Exported types ------------------------------------------------------------*/
typedef uint32_t (*UART_CallBack)();

typedef struct
{
    UART_HandleTypeDef *huart;
    uint16_t rx_buffer_size;
    uint8_t rx_buffer[256];
    UART_CallBack Function;
} Uart_RX_Message;

typedef enum {
    UART_TRAMSMIT_NONE = 0,
    UART_TRAMSMIT_BLOCKING,
    UART_TRANSMIT_IT,
    UART_TRAMSMIT_DMA,
} UART_TRANSMIT_MODE;

/* Exported variables --------------------------------------------------------*/
extern Uart_RX_Message UART_Instance[3];

/* Exported function prototypes ----------------------------------------------*/
void UserUARTReceiveIT(UART_HandleTypeDef *huart, uint32_t length, UART_CallBack Function);
void UserUARTSendData(UART_HandleTypeDef *huart, uint8_t *send_buf, uint16_t send_size, UART_TRANSMIT_MODE MODE);

#ifdef __cplusplus
}
#endif

#endif /* __FILE_H_ */