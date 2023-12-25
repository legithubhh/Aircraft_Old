/**
 *******************************************************************************
 * @file      : PID.cpp
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
#include <PID.h>

/* Private macro -------------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/**
 * @brief
 *   @arg       None
 * @retval      None
 * @note        None
 */
float PID_Def::NormalCalc()
{
    err[0] = ref - fdb;
    component[0] = kp * err[0];
    component[1] += ki * err[0];
    component[2] = kd * (err[0] - err[1]);

    VAL_LIMIT(component[0], -kp_max, kp_max);
    VAL_LIMIT(component[1], -ki_max, ki_max);
    VAL_LIMIT(component[2], -kd_max, kd_max);

    output = component[0] + component[1] + component[2];

    VAL_LIMIT(output, -output_max, output_max);
    err[1] = err[0];

    return output;
}
