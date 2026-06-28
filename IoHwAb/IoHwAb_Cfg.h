#ifndef IOHWAB_CFG_H
#define IOHWAB_CFG_H

#include "Std_Types.h"
#include "Port.h"
#include "Adc_Types.h"
#include "Pwm.h"

/**********************************************************
* IOHWAB Switches
**********************************************************/
#define IOHWAB_VERSION_INFO_API STD_ON
#define IOHWAB_DEV_ERROR_DETECT STD_ON

/**********************************************************
* Module & Vendor Identification
**********************************************************/
#define IOHWAB_MODULE_ID    83U
#define IOHWAB_INSTANCE_ID  0U
#define IOHWAB_VENDOR_ID    1234U

#define IOHWAB_SW_MAJOR_VERSION  1U
#define IOHWAB_SW_MINOR_VERSION  0U
#define IOHWAB_SW_PATCH_VERSION  0U

/**********************************************************
* SID definitions
**********************************************************/
#define IOHWAB_INIT_SID             0x00U
#define IOHWAB_GETVERSIONINFO_SID   0x01U
#define IOHWAB_MAINFUNCTION_SID     0x02U

/**********************************************************
* Error codes
**********************************************************/
#define IOHWAB_E_PARAM_POINTER      0x01U
#define IOHWAB_E_UNINIT             0x02U
#define IOHWAB_E_ALREADY_INIT       0x03U

/**********************************************************
* IoHwAb_SignalType
* Symbolic IDs for digital I/O signals
**********************************************************/
typedef enum
{
    IOHWAB_SIG_BUTTON1,
    IOHWAB_SIG_LED1,
    IOHWAB_SIG_COUNT
} IoHwAb_SignalType;

/**********************************************************
* IoHwAb_StatusType
**********************************************************/
typedef enum
{
    IOHWAB_OK      = 0x00U,
    IOHWAB_BUSY    = 0x01U,
    IOHWAB_TIMEOUT = 0x02U
} IoHwAb_StatusType;

/**********************************************************
* ioHwAb_FrequencyType
**********************************************************/
typedef uint32 IoHwAb_FrequencyType;

/**********************************************************
* IoHwAb_ConfigType
* Aggregates pointers to Port, ADC, and PWM configs
**********************************************************/
typedef struct
{
    const Port_ConfigType       *portConfig;
    const Adc_ConfigType        *adcConfig;
    const Pwm_ConfigType        *pwmConfig;
} IoHwAb_ConfigType;

/**********************************************************
* External configuration data
**********************************************************/
extern const IoHwAb_ConfigType IoHwAb_Config;

#endif
