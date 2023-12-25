/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"

#include "cmsis_os.h"
#include "main.h"
#include "task.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Communication.h"
#include "INS.h"
#include "gimbal.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
osThreadId gimbalTaskHandle;
/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId remoteTaskHandle;
osThreadId insTaskHandle;
osThreadId legCtrlTaskHandle;
osThreadId chassisTaskHandle;
osThreadId wheelrCtrlTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void StartGimbalTask(void const *argument);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const *argument);
void StartRemoteTask(void const *argument);
void StartINSTask(void const *argument);
void StartLegCtrlTask(void const *argument);
void StartChassisTask(void const *argument);
void StartWheelCtrlTask(void const *argument);

extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize);

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize)
{
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
    *ppxIdleTaskStackBuffer = &xIdleStack[0];
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
    /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
 * @brief  FreeRTOS initialization
 * @param  None
 * @retval None
 */
void MX_FREERTOS_Init(void)
{
    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* USER CODE BEGIN RTOS_MUTEX */
    /* add mutexes, ... */
    /* USER CODE END RTOS_MUTEX */

    /* USER CODE BEGIN RTOS_SEMAPHORES */
    /* add semaphores, ... */
    /* USER CODE END RTOS_SEMAPHORES */

    /* USER CODE BEGIN RTOS_TIMERS */
    /* start timers, add new ones, ... */
    /* USER CODE END RTOS_TIMERS */

    /* USER CODE BEGIN RTOS_QUEUES */
    /* add queues, ... */
    /* USER CODE END RTOS_QUEUES */

    /* Create the thread(s) */
    /* definition and creation of defaultTask */
    osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
    defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

    /* definition and creation of remoteTask */
    osThreadDef(remoteTask, StartRemoteTask, osPriorityNormal, 0, 128);
    remoteTaskHandle = osThreadCreate(osThread(remoteTask), NULL);

    /* definition and creation of insTask */
    osThreadDef(insTask, StartINSTask, osPriorityAboveNormal, 0, 256);
    insTaskHandle = osThreadCreate(osThread(insTask), NULL);

    /* definition and creation of legCtrlTask */
    osThreadDef(legCtrlTask, StartLegCtrlTask, osPriorityHigh, 0, 512);
    legCtrlTaskHandle = osThreadCreate(osThread(legCtrlTask), NULL);

    /* definition and creation of chassisTask */
    osThreadDef(chassisTask, StartChassisTask, osPriorityHigh, 0, 1024);
    chassisTaskHandle = osThreadCreate(osThread(chassisTask), NULL);

    /* definition and creation of wheelrCtrlTask */
    osThreadDef(wheelrCtrlTask, StartWheelCtrlTask, osPriorityNormal, 0, 512);
    wheelrCtrlTaskHandle = osThreadCreate(osThread(wheelrCtrlTask), NULL);

    /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */
    osThreadDef(gimbalTask, StartGimbalTask, osPriorityHigh, 0, 512);
    gimbalTaskHandle = osThreadCreate(osThread(gimbalTask), NULL);
    /* USER CODE END RTOS_THREADS */
}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const *argument)
{
    /* init code for USB_DEVICE */
    MX_USB_DEVICE_Init();
    /* USER CODE BEGIN StartDefaultTask */
    /* Infinite loop */
    for (;;) {
        HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);
        osDelay(500);
        HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);
        HAL_GPIO_TogglePin(LED_G_GPIO_Port, LED_G_Pin);
        osDelay(500);
        HAL_GPIO_TogglePin(LED_G_GPIO_Port, LED_G_Pin);
        HAL_GPIO_TogglePin(LED_B_GPIO_Port, LED_B_Pin);
        osDelay(500);
        HAL_GPIO_TogglePin(LED_B_GPIO_Port, LED_B_Pin);
        osDelay(1);
    }
    /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartRemoteTask */
/**
 * @brief Function implementing the remoteTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartRemoteTask */
void StartRemoteTask(void const *argument)
{
    /* USER CODE BEGIN StartRemoteTask */
    /* Infinite loop */
    for (;;) {
        RemoteTask();
        osDelay(1);
    }
    /* USER CODE END StartRemoteTask */
}

/* USER CODE BEGIN Header_StartINSTask */
/**
 * @brief Function implementing the insTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartINSTask */
void StartINSTask(void const *argument)
{
    /* USER CODE BEGIN StartINSTask */
    INS_Init();
    /* Infinite loop */
    for (;;) {
        INS_Task();
        osDelay(1);
    }
    /* USER CODE END StartINSTask */
}

/* USER CODE BEGIN Header_StartLegCtrlTask */
/**
 * @brief Function implementing the legCtrlTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartLegCtrlTask */
void StartLegCtrlTask(void const *argument)
{
    /* USER CODE BEGIN StartLegCtrlTask */
    /* Infinite loop */
    for (;;) {
        osDelay(1);
    }
    /* USER CODE END StartLegCtrlTask */
}

/* USER CODE BEGIN Header_StartChassisTask */
/**
 * @brief Function implementing the chassisTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartChassisTask */
void StartChassisTask(void const *argument)
{
    /* USER CODE BEGIN StartChassisTask */
    /* Infinite loop */
    for (;;) {
        osDelay(1);
    }
    /* USER CODE END StartChassisTask */
}

/* USER CODE BEGIN Header_StartWheelCtrlTask */
/**
 * @brief Function implementing the wheelrCtrlTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartWheelCtrlTask */
void StartWheelCtrlTask(void const *argument)
{
    /* USER CODE BEGIN StartWheelCtrlTask */
    /* Infinite loop */
    for (;;) {
        osDelay(1);
    }
    /* USER CODE END StartWheelCtrlTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void StartGimbalTask(void const *argument)
{
    /* USER CODE BEGIN StartWheelCtrlTask */
    /* Infinite loop */
    for (;;) {
        GimbalTask();
        osDelay(1);
    }
    /* USER CODE END StartWheelCtrlTask */
}
/* USER CODE END Application */
