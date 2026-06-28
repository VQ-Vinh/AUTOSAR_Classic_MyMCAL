/**********************************************************
 * @file    Port_cfg.h
 * @brief   Port Driver Configuration Header
 * @details File này khai báo các biến cấu hình toàn cục
 *          cho Port Driver, được định nghĩa trong Port_cfg.c
 * @version 1.0
 * @date    2024-06-19
 * @author  HALA Academy
 **********************************************************/

#ifndef PORT_CFG_H
#define PORT_CFG_H

#include "Std_Types.h"
#include "Port.h"

extern const Port_PinConfigType PortCfg_Pins[];
extern const uint32 PortCfg_PinsCount;
extern const Port_ConfigType PortCfg_Config;

#endif /* PORT_CFG_H */
