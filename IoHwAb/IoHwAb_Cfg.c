#include "IoHwAb_Cfg.h"
#include "Port_cfg.h"

extern const Adc_ConfigType AdcDriverConfig;
extern const Pwm_ConfigType PwmDriverConfig;

/**********************************************************
* IoHwAb_Config
* Aggregates existing MCAL configurations
**********************************************************/
const IoHwAb_ConfigType IoHwAb_Config = {
    .portConfig = &PortCfg_Config,
    .adcConfig  = &AdcDriverConfig,
    .pwmConfig  = &PwmDriverConfig,
};
