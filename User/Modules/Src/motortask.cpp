/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2023-11-25 18:52:36
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2023-12-05 20:16:35
 * @FilePath: \HAL_A_Board_Cpp_Template-master\User\Application\Src\Motor_pos.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
/**
 *******************************************************************************
 * @file      : motortask.cpp
 * @brief     : 此文件包含所有电机的
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
#include "motortask.h"
/* Private macro -------------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private types -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/**
 * 得到CAN1总线0X200对应电机ID返回数据的全局变量，编号0-3分别为摩擦轮3508电机1，摩擦轮3508电机2，拨弹盘2006电机，Pitch轴2006电机
 * 得到CAN1总线0X1FF对应电机ID返回数据的全局变量，编号4为Yaw轴6020电机
 */
Dji_Motor motor_measure[5];                                                                 // 编号0-4分别为摩擦轮3508电机1，摩擦轮3508电机2，拨弹盘2006电机，Pitch轴2006电机，Yaw轴6020电机
MotorControl friction_wheel_3508[2], turn_magazine_2006, pitch_motor_2006, yaw_motor_6020;  // 3508[0]为左摩擦轮，3508[1]为右摩擦轮
GimbalTargetSylloge gimbaltarget;                                                           // 建立云台摩擦轮电机与双轴电机的目标变量，方便统一进行数据读取与修改

PidsetModeID controlmode_pidset_flag = remote_pid_flag;  // 针对遥控器，键鼠与自瞄三种控制模式设置三套相适应枚举类型PID参数

PidSwitchMode pitchpid_switchflag = base_pid, yawpid_switchflag = base_pid;  // 针对不同控制模式，在同一电机执行不同任务时，设置不同的PID参数

/* Private function prototypes -----------------------------------------------*/
void FrictionShoot();
void TurnMagazineConstently();
void YawRotation();
void PitchRotation();
void RemoteAimingTargetSet();
void KeymouseAimingTargetSet();
void AutoAimingTargetSet();
void MotorStart();
void GimbalStopTargetSet();

/**
 * @brief       集中修改双轴电机的初始位置偏移量
 *   @arg       None
 * @retval      None
 * @note        None
 */
void MotorOffset()
{
    motor_measure[3].encode_offest = 4200;
}

/**
 * @brief       初始化模式：遥控模式——基本模式：PID参数集中调制
 *   @arg       None
 * @retval      None
 * @note        None
 */
void PidSetRemote()
{
    /**
     * 左右摩擦轮3508电机的PID参数初始化
     */
    friction_wheel_3508[0].pid_rpm.Set(10.f, 0.1f, 1.f, (1680.f + 168.f) * 4.f, 10.f * 0.f, 168.f * 4.f, 1680.f * 4.f);  // 限定最大值，防止突震，结合调试确定，参考速度闭环输出曲线 -16384-16384
    friction_wheel_3508[1].pid_rpm.Set(10.f, 0.1f, 1.f, (1680.f + 168.f) * 4.f, 10.f * 0.f, 168.f * 4.f, 1680.f * 4.f);

    /**
     * 拨弹盘2006电机的PID参数初始化
     */
    turn_magazine_2006.pid_rpm.Set(10.f, 0.1f, 1.f, (1000.f + 100.f) * 10.f, 10.f * 0.f, 100.f * 1.f, 1000.f * 10.f);  // 限定最大值，防止突震，输出限幅-10000-10000

    /**
     * Pitch轴2006电机的PID参数初始化
     */
    pitch_motor_2006.pid_ang.Set(45.f, 0.f, 0.f, (216.f + 21.6f) * 1.f, 1.f * 1.f, 1.f * 1.f, 216.f * 1.f);           // 依据减速比(36/1*60)=2160*n得n转每秒
    pitch_motor_2006.pid_rpm.Set(25.f, .15f, 3.f, (1000.f + 100.f) * 3.8f, 280.f * 4.f, 100.f * 4.f, 1000.f * 3.8f);  // 限定最大值，防止突震，输出限幅-10000-10000

    /**
     * Yaw轴6020电机的PID参数初始化
     */
    yaw_motor_6020.pid_ang.Set(20.f, 0.f, 10.f, (60.f + 6.f) * 15.f, 1.f * 15.f, 6.f * 15.f, 60.f * 15.f);        // n转每秒
    yaw_motor_6020.pid_rpm.Set(55.f, 0.f, 10.f, (3000.f + 300.f) * 5.f, 500.f * 5.f, 500.f * 5.f, 3000.f * 5.f);  // 限定最大值，防止突震，输出限幅-30000-30000
}

/**
 * @brief       遥控模式——仰角Pitch轴模式1:当Pitch轴为接近目标值时，减小双环KP，减少KD，避免超调；增大KI，进一步弥补静差，提高控制精度。
 *   @arg       None
 * @retval      None
 * @note        None
 */
void RemotePitchPidDemo1()
{
    pitch_motor_2006.pid_ang.Set(35.f, 0.f, 0.f, (216.f + 21.6f) * 1.f, 1.f * 1.f, 1.f * 1.f, 216.f * 1.f);
    pitch_motor_2006.pid_rpm.Set(15.f, .2f, 1.f, (1000.f + 100.f) * 3.5f, 250.f * 4.f, 100.f * 4.f, 1000.f * 3.5);
}

/**
 * @brief       遥控模式——仰角Pitch轴模式2:当Pitch轴更为接近目标值时，增大双环KP，增大，减少KD，增强抗干扰能力；增大KI，最大弥补静差，提高控制精度。
 *   @arg       None
 * @retval      None
 * @note        None
 */
void RemotePitchPidDemo2()
{
    pitch_motor_2006.pid_ang.Set(50.f, 0.f, 0.f, (216.f + 21.6f) * 0.5f, 1.f * 0.5f, 1.f * 0.5f, 216.f * 0.5f);
    pitch_motor_2006.pid_rpm.Set(15.f, 0.25f, 0.5f, (1000.f + 100.f) * 3.5f, 300.f * 4.f, 100.f * 4.f, 1000.f * 3.5f);
}

/**
 * @brief       遥控模式——俯角Pitch轴模式3:为方便调节，Pitch轴调节过程将模式3阶段设置为起始阶段。
 *   @arg       None
 * @retval      None
 * @note        None
 */
void RemotePitchPidDemo3()
{
    pitch_motor_2006.pid_ang.Set(57.5f, 0.f, 0.f, (216.f + 21.6f) * 2.5f, 1.f * 2.f, 1.f * 2.f, 216.f * 2.5f);
    pitch_motor_2006.pid_rpm.Set(40.f, 0.1f, 1.f, (1000.f + 100.f) * 5.f, 150.f * 4.f, 100.f * 4.f, 1000.f * 5.5f);
}

/**
 * @brief       遥控模式——俯角Pitch轴模式4:当Pitch轴更为接近目标值时，增大双环KP，增大，减少KD，增强抗干扰能力；增大KI，最大弥补静差，提高控制精度。
 * @brief       同时输出应该控制上限，避免过冲引发震荡。
 *   @arg       None
 * @retval      None
 * @note        None
 */
void RemotePitchPidDemo4()
{
    pitch_motor_2006.pid_ang.Set(45.f, 0.f, 0.f, (216.f + 21.6f) * 2.5f, 1.f * 2.5f, 1.f * 2.f, 216.f * 2.5f);
    pitch_motor_2006.pid_rpm.Set(20.f, 0.25f, 0.5f, (1000.f + 100.f) * 3.5f, 350.f * 4.f, 100.f * 4.f, 1000.f * 3.f);
}

/**
 * @brief       遥控模式——Yaw轴模式1:当yaw轴为接近目标值时，减小双环KP，减少KD，避免超调。
 *   @arg       None
 * @retval      None
 * @note        None
 */
void RemoteYawPidDemo1()
{
    yaw_motor_6020.pid_ang.Set(15.f, 0.f, 5.f, (60.f + 6.f) * 15.f, 1.f * 15.f, 6.f * 15.f, 60.f * 15.f);
    yaw_motor_6020.pid_rpm.Set(50.f, 0.f, 5.f, (3000.f + 300.f) * 6.f, 500.f * 10.f, 500.f * 10.f, 3000.f * 6.f);
}

/**
 * @brief       遥控模式——Yaw轴模式2:当yaw轴更为接近目标值时，增大双环KP，减少KD，增强抗干扰能力。
 *   @arg       None
 * @retval      None
 * @note        None
 */
void RemoteYawPidDemo2()
{
    yaw_motor_6020.pid_ang.Set(25.f, 0.f, 1.f, (60.f + 6.f) * 15.f, 1.f * 15.f, 6.f * 15.f, 60.f * 15.f);
    yaw_motor_6020.pid_rpm.Set(60.f, 0.f, 1.f, (3000.f + 300.f) * 6.f, 500.f * 10.f, 500.f * 10.f, 3000.f * 6.f);
}

/**
 * @brief       键鼠模式PID参数集中调制
 *   @arg       None
 * @retval      None
 * @note        None
 */
void PidSetKeymouse()
{
}

/**
 * @brief       键鼠模式PID参数集中调制
 *   @arg       None
 * @retval      None
 * @note        None
 */
void KeymPitchPidDemo1()
{
}

/**
 * @brief       自瞄模式PID参数集中调制
 *   @arg       None
 * @retval      None
 * @note        None
 */
void PidSetAutoaim()
{
}

/**
 * @brief       自瞄模式PID参数集中调制
 *   @arg       None
 * @retval      None
 * @note        None
 */
void AutoPitchPidDemo1()
{
}

/**
 * @brief       开启遥控器控制
 *   @arg       None
 * @retval      None
 * @note        None
 */
void RemoteControlMode()
{
    RemoteAimingTargetSet();
    MotorStart();
}

/**
 * @brief       开启键鼠控制
 *   @arg       None
 * @retval      None
 * @note        None
 */
void KeymouseControlMode()
{
    KeymouseAimingTargetSet();
    MotorStart();
}

/**
 * @brief       开启自瞄控制
 *   @arg       None
 * @retval      None
 * @note        None
 */
void AutoControlMode()
{
    AutoAimingTargetSet();
    MotorStart();
}

/**
 * @brief       急停模式
 *   @arg       None
 * @retval      None
 * @note        None
 */
void GimbalStopControlMode()
{
    GimbalStopTargetSet();
    MotorStart();
}

/**
 * @brief       开启云台电机PID输出计算
 *   @arg       None
 * @retval      None
 * @note        None
 */
void MotorStart()
{
    FrictionShoot();
    TurnMagazineConstently();
    YawRotation();
    PitchRotation();
}

/**
 * @brief       转动摩擦轮发射弹丸
 *   @arg       None
 * @retval      None
 * @note        None
 */
void FrictionShoot()
{
    friction_wheel_3508[0].pid_rpm.fdb = motor_measure[0].speed_rpm;
    friction_wheel_3508[1].pid_rpm.fdb = motor_measure[1].speed_rpm;
    friction_wheel_3508[0].pid_rpm.NormalCalc();
    friction_wheel_3508[1].pid_rpm.NormalCalc();
}

/**
 * @brief      连续正向转动弹仓
 *   @arg       None
 * @retval      None
 * @note        None
 */
void TurnMagazineConstently()
{
    turn_magazine_2006.pid_rpm.fdb = motor_measure[2].speed_rpm;
    turn_magazine_2006.pid_rpm.NormalCalc();
}

/**
 * @brief      右手柄上下通道控制Pitch轴转动
 *   @arg       None
 * @retval      None
 * @note        None
 */
void PitchRotation()
{
    pitch_motor_2006.pid_ang.fdb = INS.Pitch;
    pitch_motor_2006.pid_ang.NormalCalc();
    pitch_motor_2006.pid_rpm.ref = -pitch_motor_2006.pid_ang.output;
    pitch_motor_2006.pid_rpm.fdb = motor_measure[3].speed_rpm;
    pitch_motor_2006.pid_rpm.NormalCalc();
}

/**
 * @brief       左手柄左右通道控制YAW轴转动
 *   @arg       None
 * @retval      None
 * @note        None
 */
void YawRotation()
{
    yaw_motor_6020.pid_ang.fdb = INS.Yaw;
    yaw_motor_6020.pid_ang.NormalCalc();
    yaw_motor_6020.pid_rpm.ref = -yaw_motor_6020.pid_ang.output;  // yaw_motor_6020.pid_ang.output
    yaw_motor_6020.pid_rpm.fdb = motor_measure[4].speed_rpm;
    yaw_motor_6020.pid_rpm.NormalCalc();
}

/**
 * @brief      遥控器模式，电机目标值设置
 *   @arg       None
 * @retval      None
 * @note        None
 */
void RemoteAimingTargetSet()
{
    // 摩擦轮目标值设置
    gimbaltarget.friction_wheel_target = 7500;
    friction_wheel_3508[0].pid_rpm.ref = -gimbaltarget.friction_wheel_target;  // 左摩擦轮顺时针转动，右摩擦轮逆时针转动。

    friction_wheel_3508[1].pid_rpm.ref = gimbaltarget.friction_wheel_target;

    // 拨弹盘目标值设置
    gimbaltarget.turn_magazine_target = -1000;
    turn_magazine_2006.pid_rpm.ref = gimbaltarget.turn_magazine_target;

    // Pitch轴目标值设置
    if (Remote.Pack.ch1 < 1.f && Remote.Pack.ch1 > -1.f) {
        gimbaltarget.pitch_target = 0.f;
    } else if (Remote.Pack.ch1 > 1.f) {
        gimbaltarget.pitch_target = Remote.Pack.ch1 / 660.f * 23.f;
    }  // 实测仰角为负，俯角为正————抬头遥杆向后输出负值，低头遥杆向前输出正值
    else {
        gimbaltarget.pitch_target = Remote.Pack.ch1 / 660.f * 14.f;
    }
    VAL_LIMIT(gimbaltarget.pitch_target, -14.0f, 23.0f);  // 遥控器右手柄上下通道控制，抬头最大值角度为14度，低头最大角度为23度
    pitch_motor_2006.pid_ang.ref = gimbaltarget.pitch_target;

    // Yaw轴目标值设置
    gimbaltarget.yaw_target = Remote.Pack.ch2 / 660.f * 30.f;
    VAL_LIMIT(gimbaltarget.yaw_target, -30.0f, 30.0f);  // 遥控器左手柄左右通道控制，最大值为向左向右30度
    yaw_motor_6020.pid_ang.ref = -gimbaltarget.yaw_target;
}

/**
 * @brief      键鼠模式，电机目标值设置
 *   @arg       None
 * @retval      None
 * @note        None
 */
void KeymouseAimingTargetSet()
{
    // 摩擦轮目标值设置
    gimbaltarget.friction_wheel_target = 7500;
    friction_wheel_3508[0].pid_rpm.ref = -gimbaltarget.friction_wheel_target;  // 左摩擦轮顺时针转动，右摩擦轮逆时针转动。

    friction_wheel_3508[1].pid_rpm.ref = gimbaltarget.friction_wheel_target;

    // 拨弹盘目标值设置
    gimbaltarget.turn_magazine_target = -1000;
    turn_magazine_2006.pid_rpm.ref = gimbaltarget.turn_magazine_target;

    // Pitch轴目标值设置
    if (Remote.Pack.ch1 < 1.f && Remote.Pack.ch1 > -1.f) {
        gimbaltarget.pitch_target = 0.f;
    } else if (Remote.Pack.ch1 > 1.f) {
        gimbaltarget.pitch_target = Remote.Pack.ch1 / 660.f * 23.f;
    }  // 实测仰角为负，俯角为正————抬头遥杆向后输出负值，低头遥杆向前输出正值
    else {
        gimbaltarget.pitch_target = Remote.Pack.ch1 / 660.f * 14.f;
    }
    VAL_LIMIT(gimbaltarget.pitch_target, -14.0f, 23.0f);  // 遥控器右手柄上下通道控制，抬头最大值角度为14度，低头最大角度为23度
    pitch_motor_2006.pid_ang.ref = gimbaltarget.pitch_target;

    // Yaw轴目标值设置
    gimbaltarget.yaw_target = Remote.Pack.ch2 / 660.f * 30.f;
    VAL_LIMIT(gimbaltarget.yaw_target, -30.0f, 30.0f);  // 遥控器左手柄左右通道控制，最大值为向左向右45度
    yaw_motor_6020.pid_ang.ref = -gimbaltarget.yaw_target;
}

/**
 * @brief      自瞄模式，电机目标值设置
 *   @arg       None
 * @retval      None
 * @note        None
 */
void AutoAimingTargetSet()
{
    // 摩擦轮目标值设置
    gimbaltarget.friction_wheel_target = 7500;
    friction_wheel_3508[0].pid_rpm.ref = -gimbaltarget.friction_wheel_target;  // 左摩擦轮顺时针转动，右摩擦轮逆时针转动。

    friction_wheel_3508[1].pid_rpm.ref = gimbaltarget.friction_wheel_target;

    // 拨弹盘目标值设置
    gimbaltarget.turn_magazine_target = -1000;
    turn_magazine_2006.pid_rpm.ref = gimbaltarget.turn_magazine_target;

    // Pitch轴目标值设置
    if (Remote.Pack.ch1 < 1.f && Remote.Pack.ch1 > -1.f) {
        gimbaltarget.pitch_target = 0.f;
    } else if (Remote.Pack.ch1 > 1.f) {
        gimbaltarget.pitch_target = Remote.Pack.ch1 / 660.f * 23.f;
    }  // 实测仰角为负，俯角为正————抬头遥杆向后输出负值，低头遥杆向前输出正值
    else {
        gimbaltarget.pitch_target = Remote.Pack.ch1 / 660.f * 14.f;
    }
    VAL_LIMIT(gimbaltarget.pitch_target, -14.0f, 23.0f);  // 遥控器右手柄上下通道控制，抬头最大值角度为14度，低头最大角度为23度
    pitch_motor_2006.pid_ang.ref = gimbaltarget.pitch_target;

    // Yaw轴目标值设置
    gimbaltarget.yaw_target = Remote.Pack.ch2 / 660.f * 30.f;
    VAL_LIMIT(gimbaltarget.yaw_target, -30.0f, 30.0f);  // 遥控器左手柄左右通道控制，最大值为向左向右45度
    yaw_motor_6020.pid_ang.ref = -gimbaltarget.yaw_target;
}

/**
 * @brief      急停模式，电机目标值设置，程序控制摩擦轮速度为0，拨弹盘速度为0，双轴回到0度位置
 *   @arg       None
 * @retval      None
 * @note        None
 */
void GimbalStopTargetSet()
{
    // 摩擦轮目标值设置
    gimbaltarget.friction_wheel_target = 0;
    friction_wheel_3508[0].pid_rpm.ref = -gimbaltarget.friction_wheel_target;

    friction_wheel_3508[1].pid_rpm.ref = gimbaltarget.friction_wheel_target;

    // 拨弹盘目标值设置
    gimbaltarget.turn_magazine_target = 0;
    turn_magazine_2006.pid_rpm.ref = gimbaltarget.turn_magazine_target;

    // Pitch轴目标值设置
    gimbaltarget.pitch_target = 0;
    VAL_LIMIT(gimbaltarget.pitch_target, -14.0f, 23.0f);
    pitch_motor_2006.pid_ang.ref = gimbaltarget.pitch_target;

    // Yaw轴目标值设置
    gimbaltarget.yaw_target = 0;
    VAL_LIMIT(gimbaltarget.yaw_target, -30.0f, 30.0f);
    yaw_motor_6020.pid_ang.ref = -gimbaltarget.yaw_target;
}
