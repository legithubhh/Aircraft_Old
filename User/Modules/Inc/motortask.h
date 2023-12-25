/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2023-11-15 20:37:27
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2023-12-05 19:57:25
 * @FilePath: \HAL_A_Board_Cpp_Template-master\User\Algorithm\Inc\PID.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
/**
 *******************************************************************************
 * @file      : motortask.h
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
#ifndef __MOTOR_TASK_H_
#define __MOTOR_TASK_H_

#ifdef __cplusplus

/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>

#include "DR16.h"
#include "Dji_Motor.h"
#include "INS.h"
#include "PID.h"
#include "drv_can.h"
#include "drv_uart.h"
#include "stm32f4xx_hal.h"
#include "tim.h"

/* Exported macro ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

/**
 * 创建电机位置与角度PID结构体类型,方便统一设置
 */
typedef struct
{
    PID_Def pid_ang, pid_rpm;
} MotorControl;

/**
 * 创建摩擦轮电机与双轴电机的目标值变量，方便修改。
 */
typedef struct
{
    float pitch_target;
    float yaw_target;
    float friction_wheel_target;
    float turn_magazine_target;
} GimbalTargetSylloge;

/**
 * 创建电机CAN通信ID枚举类型，方便辨别，由使用情况具体定义名称
 */
typedef enum {
    /**
     * 0x200对应CAN1发送控制两个3508和两个2006，0x1FF对应CAN1发送控制6020
     */
    CAN_GIMBAL_SEND_ID = 0x200,
    CAN_GIMBAL_SEND_ELSE_ID = 0x1FF,

    /**
     * 分别对应两个3508和两个2006报文ID，和一个6020报文ID
     */
    CAN_FRICTION1_3508_ID = 0x201,
    CAN_FRICTION2_3508_ID = 0x202,

    CAN_TRIGGER_2006_ID = 0x203,
    CAN_PITCH_2006_ID = 0x204,

    CAN_YAW_6020_ID = 0x205,

    /**
     * 3508电调ID软件快速设置发送ID
     */
    CAN_MOTOR3508_SET_ID = 0x700

} CanMsgMotorId;

/**
 * 针对遥控器，键鼠与自瞄三种控制模式设置三套相适应枚举类型PID参数，以下为提供判断条件的对应的标志编码
 */
typedef enum {
    /**
     * 遥控模式与急停模式
     */
    remote_pid_flag = 0,

    /**
     * 键鼠模式
     */
    keymouse_pid_flag = 1,

    /**
     * 自瞄模式
     */
    autoaim_pid_flag = 2,

} PidsetModeID;

/**
 * 同一电机在执行不同任务时可能需要用到不同PID参数，进行PID参数的切换，以下为提供判断条件的对应的标志编码
 */
typedef enum {
    /**
     * 基础模式
     */
    base_pid = 0,

    /**
     * Pitch模式1
     */
    pitch1_pid = 1,

    /**
     * Pitch模式2
     */
    pitch2_pid = 2,

    /**
     * Pitch模式3
     */
    pitch3_pid = 3,

    /**
     * Pitch模式4
     */
    pitch4_pid = 4,

    /**
     * Yaw模式1
     */

    yaw1_pid = 5,

    /**
     * Yaw模式2
     */
    yaw2_pid = 6

} PidSwitchMode;
/* Exported variables -------------------------------------------------------*/
extern Dji_Motor motor_measure[5];                                                                 // 编号0-4分别为摩擦轮3508电机1，摩擦轮3508电机2，拨弹盘2006电机，Pitch轴2006电机，Yaw轴6020电机
extern MotorControl friction_wheel_3508[2], turn_magazine_2006, pitch_motor_2006, yaw_motor_6020;  // 3508[0]为左摩擦轮，3508[1]为右摩擦轮

extern PidsetModeID controlmode_pidset_flag;                  // 针对遥控器，键鼠与自瞄三种控制模式设置三套相适应枚举类型PID参数
extern PidSwitchMode pitchpid_switchflag, yawpid_switchflag;  // 针对不同控制模式，在同一电机执行不同任务时，设置不同的PID参数
/* Exported function prototypes ----------------------------------------------*/
void PidSetRemote();
void RemotePitchPidDemo1();
void RemotePitchPidDemo2();
void RemotePitchPidDemo3();
void RemotePitchPidDemo4();
void RemoteYawPidDemo1();
void RemoteYawPidDemo2();
void PidSetKeymouse();
void PidSetAutoaim();
void KeymPitchPidDemo1();
void AutoPitchPidDemo1();

void MotorOffset();

void RemoteControlMode();
void KeymouseControlMode();
void AutoControlMode();
void GimbalStopControlMode();

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

#endif /* __MOTOR_TASK_H_ */
