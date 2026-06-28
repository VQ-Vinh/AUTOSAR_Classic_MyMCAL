#ifndef IOHWAB_H
#define IOHWAB_H

#include "Std_Types.h"
#include "IoHwAb_Cfg.h"

/**********************************************************
* Initialization
**********************************************************/
void IoHwAb_Init0(const IoHwAb_ConfigType *ConfigPtr);

/**********************************************************
* Version Info
**********************************************************/
void IoHwAb_GetVersionInfo(Std_VersionInfoType *versioninfo);

/**********************************************************
* Main Function (background processing)
**********************************************************/
void IoHwAb_MainFunction(void);

/**********************************************************
* Digital I/O
**********************************************************/
Std_ReturnType IoHwAb_Digital_ReadSignal(IoHwAb_SignalType id, boolean *value);
Std_ReturnType IoHwAb_Digital_WriteSignal(IoHwAb_SignalType id, boolean value);

/**********************************************************
* Analog (ADC)
**********************************************************/
Adc_ValueGroupType IoHwAb_Adc_ReadSignal(Adc_GroupType group, uint8 channel, IoHwAb_StatusType *status);

/**********************************************************
* PWM
**********************************************************/
void IoHwAb_SetPwmDuty(Pwm_ChannelType channel, uint16 duty);
Pwm_PeriodType IoHwAb_Pwm_ConvertToPeriod(Pwm_ChannelType channel, IoHwAb_FrequencyType freq);

/**********************************************************
* Application-Specific (temperature + heater example)
**********************************************************/
uint16 IoHwAb_GetTemperature(IoHwAb_StatusType *status);
Std_ReturnType IoHwAb_SetHeaterPower(uint8 percent);

/**********************************************************
* Callbacks (called by MCAL drivers)
**********************************************************/
void IoHwAb_AdcNotification0(void);
void IoHwAb_AdcNotification1(void);

#endif
