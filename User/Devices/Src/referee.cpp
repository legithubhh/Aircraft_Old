#include "referee.h"

referee_info_t referee_info;

void RefereePack(uint8_t* pData)
{
    uint16_t judge_length;  // 统计一帧数据长度
    //	referee_info.CmdID = 0; //数据命令码解析
    // 空数据包，则不作任何处理
    if (pData == NULL)
        return;

    // 写入帧头数据(5-byte),用于判断是否开始存储裁判数据
    memcpy(&referee_info.FrameHeader, pData, LEN_HEADER);

    // 判断帧头数据(0)是否为0xA5
    if (pData[SOF] == JUDGE_FRAME_HEADER) {
        // 帧头CRC8校验
        if (Verify_CRC8_Check_Sum(pData, LEN_HEADER) == TRUE) {
            // 统计一帧数据长度(byte),用于CR16校验
            judge_length = pData[DATA_LENGTH] + LEN_HEADER + LEN_CMDID + LEN_TAIL;
            // 帧尾CRC16校验
            if (Verify_CRC16_Check_Sum(pData, judge_length) == TRUE) {
                // 2个8位拼成16位int
                referee_info.CmdID = (pData[6] << 8 | pData[5]);
                // 解析数据命令码,将数据拷贝到相应结构体中(注意拷贝数据的长度)
                // 第8个字节开始才是数据 data=7
                switch (referee_info.CmdID) {
                    case ID_game_state:  // 0x0001
                        memcpy(&referee_info.GameState, (pData + DATA), LEN_game_state);
                        break;

                    case ID_game_result:  // 0x0002
                        memcpy(&referee_info.GameResult, (pData + DATA), LEN_game_result);
                        break;

                    case ID_game_robot_survivors:  // 0x0003
                        memcpy(&referee_info.GameRobotHP, (pData + DATA), LEN_game_robot_HP);
                        break;

                    case ID_event_data:  // 0x0101
                        memcpy(&referee_info.EventData, (pData + DATA), LEN_event_data);
                        break;

                    case ID_supply_projectile_action:  // 0x0102
                        memcpy(&referee_info.SupplyProjectileAction, (pData + DATA), LEN_supply_projectile_action);
                        break;

                    case ID_game_robot_state:  // 0x0201
                        memcpy(&referee_info.GameRobotStat, (pData + DATA), LEN_game_robot_state);
                        break;
                    case ID_power_heat_data:  // 0x0202
                        memcpy(&referee_info.PowerHeatData, (pData + DATA), LEN_power_heat_data);
                        break;

                    case ID_game_robot_pos:  // 0x0203
                        memcpy(&referee_info.GameRobotPos, (pData + DATA), LEN_game_robot_pos);
                        break;

                    case ID_buff_musk:  // 0x0204
                        memcpy(&referee_info.BuffMusk, (pData + DATA), LEN_buff_musk);
                        break;

                    case ID_aerial_robot_energy:  // 0x0205
                        memcpy(&referee_info.AerialRobotEnergy, (pData + DATA), LEN_aerial_robot_energy);
                        break;

                    case ID_robot_hurt:  // 0x0206
                        memcpy(&referee_info.RobotHurt, (pData + DATA), LEN_robot_hurt);
                        break;

                    case ID_shoot_data:  // 0x0207
                        memcpy(&referee_info.ShootData, (pData + DATA), LEN_shoot_data);
                        // JUDGE_ShootNumCount();//发弹量统计
                        break;

                    case ID_aerial_bullet:  // 0x0208
                        memcpy(&referee_info.BulletData, (pData + DATA), LEN_bullet_data);
                        break;

                    case ID_command_data:  // 0x0304
                        memcpy(&referee_info.CommaData, (pData + DATA), LEN_command_data);
                        break;
                }
            }
        }
        // 首地址加帧长度,指向CRC16下一字节,用来判断是否为0xA5,从而判断一个数据包是否有多帧数据
        if (*(pData + sizeof(xFrameHeader) + LEN_CMDID + referee_info.FrameHeader.DataLength + LEN_TAIL) == 0xA5) {
            // 如果一个数据包出现了多帧数据,则再次调用解析函数,直到所有数据包解析完毕
            RefereePack(pData + sizeof(xFrameHeader) + LEN_CMDID + referee_info.FrameHeader.DataLength + LEN_TAIL);
        }
    }
}

void KeyProcessReferee()  // 图传链路键鼠解算代码，要用直接调用至RefereePack函数最后面
{
    for (short i = 0; i < 16; i++) {
        if (referee_info.CommaData.keyboard_value & (0x01 << i)) {
            Remote.Pack.KeyPress[i] = 1;
        } else {
            Remote.Pack.KeyPress[i] = 0;
        }
    }

    if (referee_info.CommaData.left_button_down == 0x01) {
        Remote.Pack.KeyPress[MOUSE_L] = 1;
    } else {
        Remote.Pack.KeyPress[MOUSE_L] = 0;
    }
    if (referee_info.CommaData.right_button_down == 0x01) {
        Remote.Pack.KeyPress[MOUSE_R] = 1;
    } else {
        Remote.Pack.KeyPress[MOUSE_R] = 0;
    }
}
