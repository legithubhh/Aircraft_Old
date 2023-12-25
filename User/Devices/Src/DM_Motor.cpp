/**
 *******************************************************************************
 * @file      : DM_Motor.cpp
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
#include "DM_Motor.h"
/* Private macro -------------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
DM_Motor Left_Motor[2];
DM_Motor Right_Motor[2];
/* Private function prototypes -----------------------------------------------*/
static int FloatToUint(float x, float x_min, float x_max, int bits);
static float UintToFloat(int x_int, float x_min, float x_max, int bits);

void DM_Motor::Update(uint8_t can_rx_data[])
{
    static int p_int, v_int, t_int;
    p_int = can_rx_data[1] << 8 | can_rx_data[2];
    v_int = can_rx_data[3] << 4 | can_rx_data[4] >> 4;
    t_int = (can_rx_data[4] & 0xF) << 8 | can_rx_data[5];
    angle = UintToFloat(p_int, P_MIN, P_MAX, 16);
    speed = UintToFloat(v_int, V_MIN, V_MAX, 12);
    torque = UintToFloat(t_int, T_MIN, T_MAX, 12);
}

void DM_Motor::Enable(CAN_HandleTypeDef* hcan, int16_t ID)
{
    CANx_PackProcess_Data(hcan, ID, 0x08, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFC);
}

void DM_Motor::Disable(CAN_HandleTypeDef* hcan, int16_t ID)
{
    CANx_PackProcess_Data(hcan, ID, 0x08, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFD);
}

void DM_Motor::SaveZero(CAN_HandleTypeDef* hcan, int16_t ID)
{
    CANx_PackProcess_Data(hcan, ID, 0x08, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFE);
}

void DM_Motor::Ctrl(CAN_HandleTypeDef* hcan)
{
    uint16_t ang_tmp, spd_tmp, kp_tmp, kd_tmp, tor_tmp;

    ang_tmp = FloatToUint(send_ang, P_MIN, P_MAX, 16);
    spd_tmp = FloatToUint(send_spd, V_MIN, V_MAX, 12);
    kp_tmp = FloatToUint(send_kp, KP_MIN, KP_MAX, 12);
    kd_tmp = FloatToUint(send_kd, KD_MIN, KD_MAX, 12);
    tor_tmp = FloatToUint(send_tor, T_MIN, T_MAX, 12);

    CANx_PackProcess_DM(hcan, motor_id, 0x08, ang_tmp, spd_tmp, kp_tmp, kd_tmp, tor_tmp);
}

void DM_Motor::SetTor(int16_t ID, float _tor)
{
    SetData(ID, 0, 0, _tor, 0, 0);
}

void DM_Motor::SetData(int16_t _id, float _pos, float _vel, float _tor, float _kp, float _kd)
{
    motor_id = _id;
    send_ang = _pos;
    send_spd = _vel;
    send_tor = _tor;
    send_kp = _kp;
    send_kd = _kd;
}

float DM_Motor::GetAngle()
{
    return angle;
}

float DM_Motor::GetSpeed()
{
    return speed;
}

float DM_Motor::GetTorque()
{
    return torque;
}

static int FloatToUint(float x, float x_min, float x_max, int bits)
{
    /// Converts a float to an unsigned int, given range and number of bits///
    float span = x_max - x_min;
    float offset = x_min;
    return (int)((x - offset) * ((float)((1 << bits) - 1)) / span);
}

static float UintToFloat(int x_int, float x_min, float x_max, int bits)
{
    /// converts unsigned int to float, given range and number of bits ///
    float span = x_max - x_min;
    float offset = x_min;
    return ((float)x_int) * span / ((float)((1 << bits) - 1)) + offset;
}