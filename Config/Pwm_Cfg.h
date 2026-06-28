/**********************************************************
 * @file    Pwm_Cfg.h
 * @brief   PWM Driver Compile-Time Configuration Header
 * @details File này chứa các macro cấu hình compile-time
 *          cho PWM Driver theo chuẩn AUTOSAR.
 * @version 1.0
 * @date    2024-06-27
 * @author  HALA Academy
 **********************************************************/

#ifndef PWM_CFG_H
#define PWM_CFG_H

#include "Std_Types.h"

/**********************************************************
 * Module & Vendor Identification
 **********************************************************/
#define PWM_MODULE_ID    82U
#define PWM_INSTANCE_ID  0U
#define PWM_VENDOR_ID    1234U

#define PWM_SW_MAJOR_VERSION  1U
#define PWM_SW_MINOR_VERSION  0U
#define PWM_SW_PATCH_VERSION  0U

/**********************************************************
 * Compile-time switches (STD_ON / STD_OFF)
 **********************************************************/
#define PWM_DEV_ERROR_DETECT               STD_ON
#define PWM_DEINIT_API                     STD_ON
#define PWM_VERSION_INFO_API               STD_ON
#define PWM_LOW_POWER_STATES_SUPPORT       STD_ON

#endif /* PWM_CFG_H */
