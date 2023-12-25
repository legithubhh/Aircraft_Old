/**
 *******************************************************************************
 * @file      : PID.h
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
#ifndef __PID_H_
#define __PID_H_

#ifdef __cplusplus

/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>

/* Exported macro ------------------------------------------------------------*/
#define VAL_LIMIT(val, min, max) \
    if (val <= min) {            \
        val = min;               \
    } else if (val >= max) {     \
        val = max;               \
    }

/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
class PID_Def
{
   public:
    float ref;
    float fdb;
    float output;
    float kp;
    float ki;
    float kd;
    float kp_max;
    float ki_max;
    float kd_max;
    float output_max;

    float component[3];
    float err[6];

    float NormalCalc();
    void Set(float _kp, float _ki, float _kd, float _kp_m, float _ki_m, float _kd_m, float _output_m)
    {
        kp = _kp;
        ki = _ki;
        kd = _kd;
        kp_max = _kp_m;
        ki_max = _ki_m;
        kd_max = _kd_m;
        output_max = _output_m;
    }

   private:
};

/* Exported variables --------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/

#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* __FILE_H_ */
