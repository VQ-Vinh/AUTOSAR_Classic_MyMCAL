/**********************************************************
 * @file    Adc_Cfg.h
 * @brief   ADC Driver Compile-Time Configuration Header
 * @details File này chứa các macro cấu hình compile-time
 *          cho ADC Driver theo chuẩn AUTOSAR.
 * @version 2.0
 * @date    2026-03-21
 * @author  HALA Academy
 **********************************************************/

#ifndef ADC_CFG_H
#define ADC_CFG_H

#include "Std_Types.h"

/**********************************************************
 * Compile-time switches (STD_ON / STD_OFF)
 **********************************************************/
#define ADC_DEV_ERROR_DETECT                STD_ON
#define ADC_DEINIT_API                      STD_ON
#define ADC_READ_GROUP_API                  STD_ON
#define ADC_HW_TRIGGER_API                  STD_ON
#define ADC_GRP_NOTIF_CAPABILITY            STD_ON
#define ADC_ENABLE_START_STOP_GROUP_API     STD_ON
#define ADC_VERSION_INFO_API                STD_ON
#define ADC_LOW_POWER_STATES_SUPPORT        STD_ON
#define ADC_POWER_STATE_ASYNCH_TRANSITION_MODE STD_OFF

/**********************************************************
 * Giới hạn tài nguyên
 **********************************************************/
#define ADC_MAX_GROUPS               2U
#define ADC_MAX_CHANNELS_PER_GROUP   4U
#define ADC_CONFIGURED_GROUPS        2U

/**********************************************************
 * Module & Vendor Identification
 **********************************************************/
#define ADC_MODULE_ID    80U
#define ADC_INSTANCE_ID  0U
#define ADC_VENDOR_ID    1234U

#define ADC_SW_MAJOR_VERSION  2U
#define ADC_SW_MINOR_VERSION  0U
#define ADC_SW_PATCH_VERSION  0U

/**********************************************************
 * API Service IDs (SID)
 **********************************************************/
#define ADC_INIT_SID                     0x00U
#define ADC_SETUPRESULTBUFFER_SID        0x01U
#define ADC_DEINIT_SID                   0x02U
#define ADC_STARTGROUPCONVERSION_SID     0x03U
#define ADC_STOPGROUPCONVERSION_SID      0x04U
#define ADC_READGROUP_SID                0x05U
#define ADC_ENABLEHARDWARETRIGGER_SID    0x06U
#define ADC_DISABLEHARDWARETRIGGER_SID   0x07U
#define ADC_ENABLEGROUPNOTIFICATION_SID  0x08U
#define ADC_DISABLEGROUPNOTIFICATION_SID 0x09U
#define ADC_GETSTREAMLASTPOINTER_SID     0x0AU
#define ADC_GETVERSIONINFO_SID           0x0BU
#define ADC_PREPAREPOWERSTATE_SID        0x0CU
#define ADC_SETPOWERSTATE_SID            0x0DU

/**********************************************************
 * Error Codes (ADC_E_*)
 **********************************************************/
#define ADC_E_PARAM_POINTER               0x01U
#define ADC_E_ALREADY_INITIALIZED         0x02U
#define ADC_E_UNINIT                      0x03U
#define ADC_E_PARAM_GROUP                 0x04U
#define ADC_E_BUSY                        0x05U
#define ADC_E_IDLE                        0x06U
#define ADC_E_NOT_DISENGAGED              0x07U
#define ADC_E_WRONG_TRIGG_SRC             0x08U
#define ADC_E_WRONG_CONV_MODE             0x09U
#define ADC_E_BUFFER_UNINIT               0x0AU
#define ADC_E_NOTIF_CAPABILITY            0x0BU
#define ADC_E_POWER_STATE_NOT_SUPPORTED   0x0CU
#define ADC_E_PERIPHERAL_NOT_PREPARED     0x0DU
#define ADC_E_TRANSITION_NOT_POSSIBLE     0x0EU

#endif /* ADC_CFG_H */
