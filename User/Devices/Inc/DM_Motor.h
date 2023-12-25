/**
 *******************************************************************************
 * @file      : DM_Motor.h
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
#ifndef __DM_MOTOR_H_
#define __DM_MOTOR_H_

#ifdef __cplusplus

/* Includes ------------------------------------------------------------------*/
#include "drv_can.h"
/* Exported macro ------------------------------------------------------------*/
#define P_MIN -12.5   // 位置最小值
#define P_MAX 12.5    // 位置最大值
#define V_MIN -45     // 速度最小值
#define V_MAX 45      // 速度最大值
#define KP_MIN 0.0    // Kp最小值
#define KP_MAX 500.0  // Kp最大值
#define KD_MIN 0.0    // Kd最小值
#define KD_MAX 5.0    // Kd最大值
#define T_MIN -18     // 转矩最大值
#define T_MAX 18      // 转矩最小值

/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
class DM_Motor
{
   public:
    uint16_t motor_id;
    float angle, speed, torque;
    float send_ang, send_spd, send_kp, send_kd, send_tor;
    void Update(uint8_t can_rx_data[]);
    void Enable(CAN_HandleTypeDef* hcan, int16_t ID);
    void Disable(CAN_HandleTypeDef* hcan, int16_t ID);
    void SaveZero(CAN_HandleTypeDef* hcan, int16_t ID);
    void Ctrl(CAN_HandleTypeDef* hcan);
    //  void SetAngele(CAN_HandleTypeDef* hcan, int16_t ID, float _pos);
    void SetTor(int16_t ID, float _tor);
    void SetData(int16_t _id, float _pos, float _vel, float _tor, float _kp, float _kd);
    float GetAngle();
    float GetSpeed();
    float GetTorque();

   private:
};
/* Exported variables --------------------------------------------------------*/
extern DM_Motor Left_Motor[2];
extern DM_Motor Right_Motor[2];
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

#endif /* __DM_MOTOR_H_ */
