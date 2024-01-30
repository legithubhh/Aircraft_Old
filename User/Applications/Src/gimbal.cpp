/**
 *******************************************************************************
 * @file      : gimbal.cpp
 * @brief     : 云台总任务及各个子函数任务
 *              s1  1||3   s2  1   遥控器手控模式
 *              s1  2      s2  1   遥控器自瞄模式
 *              s1  x      s2  3   键鼠模式
 *              s1  x      s2  3   持续按R键时   键鼠自瞄模式
 *              s1  x      s2  2   摩擦轮急停模式（双轴可遥控操纵）
 *              ch0
 *              ch1                Pitch轴（遥杆前移低头，后移抬头）
 *              ch2                Yaw轴（遥杆左移云台左转，右移云台右转）
 *              ch3
 *              mouse_x            Yaw轴（鼠标左移云台左转，右移云台右转）
 *              mouse_y            Pitch轴（鼠标前移抬头，后移低头）
 * @history   :
 *  Version     Date            Author          Note
 *  V0.9.0      2024-01-20      <JasonLi>        1. <note>
 *******************************************************************************
 * @attention :
 *******************************************************************************
 *  Copyright (c) 2023 Reborn Team, University of Science and Technology Beijing.
 *  All Rights Reserved.
 *******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/
#include "gimbal.h"
/* Private macro -------------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void CANC6020IdSet();
void PidSetSwitch();
void PidFlagInit(PidsetModeID);
/**
 * @brief       遥控器管理云台所有电机运行任务
 *   @arg       None
 * @retval      None
 * @note        遥控器s1对应左拨杆，s2对应右拨杆，上，中，下分别对应数值1，3,2
 */

void GimbalTask()
{
    MotorOffset();
    // 右拨杆在上，遥控器控制模式
    if (Remote.Pack.s2 == 1) {
        // 左拨杆在上或中，遥控器手控模式 OR 左拨杆在下，遥控器切换到自瞄模式
        if (Remote.Pack.s1 == 1 || Remote.Pack.s1 == 3) {
            PidFlagInit(remote_pid_flag);
            // 当Pitch轴误差达到一定范围时，更改PID参数进行自适应调节，分俯仰角调节以适应重心后偏
            if (pitch_motor_2006.pid_ang.fdb < 0.5) {
                if (pitch_motor_2006.pid_ang.err[0] > 2.5f || pitch_motor_2006.pid_ang.err[0] < -2.5f) {
                    pitchpid_switchflag = base_pid;
                } else if (pitch_motor_2006.pid_ang.err[0] > 1.f || pitch_motor_2006.pid_ang.err[0] < -1.f) {
                    pitchpid_switchflag = pitch1_pid;
                } else {
                    pitchpid_switchflag = pitch2_pid;
                }
            } else {
                if (pitch_motor_2006.pid_ang.err[0] > 3.5f || pitch_motor_2006.pid_ang.err[0] < -3.5f) {
                    pitchpid_switchflag = pitch3_pid;
                } else if (pitch_motor_2006.pid_ang.err[0] > 0.5f || pitch_motor_2006.pid_ang.err[0] < -0.5f) {
                    pitchpid_switchflag = pitch4_pid;
                } else {
                    pitchpid_switchflag = pitch5_pid;
                }
            }

            // 当Yaw轴误差达到一定范围时，更改PID参数进行自适应调节
            if (yaw_motor_6020.pid_ang.err[0] > 15.f || yaw_motor_6020.pid_ang.err[0] < -15.f) {
                yawpid_switchflag = base_pid;
            } else if (yaw_motor_6020.pid_ang.err[0] > 2.5f || yaw_motor_6020.pid_ang.err[0] < -2.5f) {
                yawpid_switchflag = yaw1_pid;
            } else {
                yawpid_switchflag = yaw2_pid;
            }
            PidSetSwitch();
            RemoteControlMode();
        } else {
            PidFlagInit(autoaim_pid_flag);
            PidSetSwitch();
            AutoControlMode();
        }
    }

    // 右拨杆在中，键鼠模式
    if (Remote.Pack.s2 == 3) {
        // 当键盘R键按下期间，键鼠模式切换到自瞄模式，（松开换回键鼠手控模式？）
        if (Remote.Pack.KeyPress[MOUSE_R]) {
            PidFlagInit(autoaim_pid_flag);
            PidSetSwitch();
            AutoControlMode();
        } else {
            PidFlagInit(keymouse_pid_flag);
            if (pitch_motor_2006.pid_ang.fdb < 0.5) {
                if (pitch_motor_2006.pid_ang.err[0] > 2.5f || pitch_motor_2006.pid_ang.err[0] < -2.5f) {
                    pitchpid_switchflag = base_pid;
                } else if (pitch_motor_2006.pid_ang.err[0] > 1.f || pitch_motor_2006.pid_ang.err[0] < -1.f) {
                    pitchpid_switchflag = pitch1_pid;
                } else {
                    pitchpid_switchflag = pitch2_pid;
                }
            } else {
                if (pitch_motor_2006.pid_ang.err[0] > 3.5f || pitch_motor_2006.pid_ang.err[0] < -3.5f) {
                    pitchpid_switchflag = pitch3_pid;
                } else if (pitch_motor_2006.pid_ang.err[0] > 0.5f || pitch_motor_2006.pid_ang.err[0] < -0.5f) {
                    pitchpid_switchflag = pitch4_pid;
                } else {
                    pitchpid_switchflag = pitch5_pid;
                }
            }

            // 当Yaw轴误差达到一定范围时，更改PID参数进行自适应调节
            if (yaw_motor_6020.pid_ang.err[0] > 15.f || yaw_motor_6020.pid_ang.err[0] < -15.f) {
                yawpid_switchflag = base_pid;
            } else if (yaw_motor_6020.pid_ang.err[0] > 1.5f || yaw_motor_6020.pid_ang.err[0] < -1.5f) {
                yawpid_switchflag = yaw1_pid;
            } else {
                yawpid_switchflag = yaw2_pid;
            }
            PidSetSwitch();
            KeymouseControlMode();
        }
    }

    // 右拨杆在下，急停模式
    if (Remote.Pack.s2 == 2) {
        // 左拨杆在上或中，发弹急停 OR 左拨杆在下，切换到全停模式（拨弹盘，摩擦轮目标速度设为0，双轴目标位置设为0度）
        if (Remote.Pack.s1 == 1 || Remote.Pack.s1 == 3) {
            PidFlagInit(remote_pid_flag);
            // 当Pitch轴误差达到一定范围时，更改PID参数进行自适应调节，分俯仰角调节以适应重心后偏
            if (pitch_motor_2006.pid_ang.fdb < 0.5) {
                if (pitch_motor_2006.pid_ang.err[0] > 2.5f || pitch_motor_2006.pid_ang.err[0] < -2.5f) {
                    pitchpid_switchflag = base_pid;
                } else if (pitch_motor_2006.pid_ang.err[0] > 1.f || pitch_motor_2006.pid_ang.err[0] < -1.f) {
                    pitchpid_switchflag = pitch1_pid;
                } else {
                    pitchpid_switchflag = pitch2_pid;
                }
            } else {
                if (pitch_motor_2006.pid_ang.err[0] > 3.5f || pitch_motor_2006.pid_ang.err[0] < -3.5f) {
                    pitchpid_switchflag = pitch3_pid;
                } else if (pitch_motor_2006.pid_ang.err[0] > 0.5f || pitch_motor_2006.pid_ang.err[0] < -0.5f) {
                    pitchpid_switchflag = pitch4_pid;
                } else {
                    pitchpid_switchflag = pitch5_pid;
                }
            }

            // 当Yaw轴误差达到一定范围时，更改PID参数进行自适应调节
            if (yaw_motor_6020.pid_ang.err[0] > 3.5f || yaw_motor_6020.pid_ang.err[0] < -3.5f) {
                yawpid_switchflag = base_pid;
            } else if (yaw_motor_6020.pid_ang.err[0] > 2.5f || yaw_motor_6020.pid_ang.err[0] < -2.5f) {
                yawpid_switchflag = yaw1_pid;
            } else {
                yawpid_switchflag = yaw2_pid;
            }
            PidSetSwitch();
            GimbalStop1ControlMode();
        } else {
            PidFlagInit(remote_pid_flag);
            PidSetSwitch();
            GimbalStop2ControlMode();
        }
    }
    // 集中发送CAN信号
    CANx_PackProcess_Data(&hcan1, CAN_GIMBAL_SEND_ID, 0x08, friction_wheel_3508[0].pid_rpm.output, friction_wheel_3508[1].pid_rpm.output,
                          turn_magazine_2006.pid_rpm.output, pitch_motor_2006.pid_rpm.output);                      // CAN1总线0X200对应电机ID，ID号1-4分别为摩擦轮3508电机1，摩擦轮3508电机2，拨弹盘2006电机，Pitch轴2006电机
    CANx_PackProcess_Data(&hcan1, CAN_GIMBAL_SEND_ELSE_ID, 0x08, yaw_motor_6020.pid_rpm.output, 0000, 0000, 0000);  // CAN1总线0X1FF对应电机ID，ID号1为Yaw轴6020

    // 设置C6020电调进入快速设置ID模式
    // CANC6020IdSet();

    // 以下为测试代码专用

    // TurnMagazineConstently();
}

/**
 * @brief       PID参数标志初始化
 *   @arg       None
 * @retval      None
 * @note        None
 */
void PidFlagInit(PidsetModeID pidsetmode)
{
    controlmode_pidset_flag = pidsetmode;
    pitchpid_switchflag = base_pid;
    yawpid_switchflag = base_pid;
}

/**
 * @brief       不同模式不同任务下PID设置选择判断,更改controlmode_pidset_flag进入不同PID模式，更改xxx_pid_switchflag进行不同任务下不同设置
 *   @arg       None
 * @retval      None
 * @note        None
 */
void PidSetSwitch()
{
    // 遥控器模式参数
    if (controlmode_pidset_flag == remote_pid_flag) {
        PidSetRemote();
        // 遥控模式Pitch轴PID调制
        switch (pitchpid_switchflag) {
            case base_pid:
                break;
            case pitch1_pid:
                RemotePitchPidDemo1();
                break;
            case pitch2_pid:
                RemotePitchPidDemo2();
                break;
            case pitch3_pid:
                RemotePitchPidDemo3();
                break;
            case pitch4_pid:
                RemotePitchPidDemo4();
                break;
            case pitch5_pid:
                RemotePitchPidDemo5();
                break;
        }
        // 遥控模式Yaw轴PID调制
        switch (yawpid_switchflag) {
            case base_pid:
                break;
            case yaw1_pid:
                RemoteYawPidDemo1();
                break;
            case yaw2_pid:
                RemoteYawPidDemo2();
                break;
        }
    }
    // 键鼠模式参数
    else if (controlmode_pidset_flag == keymouse_pid_flag) {
        PidSetRemote();
        // 键鼠模式Pitch轴PID调制
        switch (pitchpid_switchflag) {
            case base_pid:
                break;
            case pitch1_pid:
                RemotePitchPidDemo1();
                break;
            case pitch2_pid:
                RemotePitchPidDemo2();
                break;
            case pitch3_pid:
                RemotePitchPidDemo3();
                break;
            case pitch4_pid:
                RemotePitchPidDemo4();
                break;
            case pitch5_pid:
                RemotePitchPidDemo5();
                break;
        }
        // 键鼠模式Yaw轴PID调制
        switch (yawpid_switchflag) {
            case base_pid:
                break;
            case yaw1_pid:
                RemoteYawPidDemo1();
                break;
            case yaw2_pid:
                RemoteYawPidDemo2();
                break;
        }
    }
    // 自瞄模式参数
    else if (controlmode_pidset_flag == autoaim_pid_flag) {
        PidSetAutoaim();
        switch (pitchpid_switchflag) {
            case base_pid:
                break;
            case pitch1_pid:
                AutoPitchPidDemo1();
                break;
            default:
                break;
        }
    }
}

/**
 * @brief       发送ID为0x700的CAN包,它会设置C6020电调进入快速设置ID
 *   @arg       None
 * @retval      None
 * @note        None
 */
void CANC6020IdSet()
{
    CANx_PackProcess_Data(&hcan1, CAN_MOTOR3508_SET_ID, 0x08, 0000, 0000, 0000, 0000);
}
