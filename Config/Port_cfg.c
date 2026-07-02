#include "Port_cfg.h"

const Port_PinConfigType PortCfg_Pins[] = {
    /* PA0 - ADC1_IN0 - Analog Input (biến trở 10k) */
    {
        .PortNum              = PORT_ID_A,
        .PinNum               = 0,
        .Mode                 = PORT_PIN_MODE_ADC,
        .Direction            = PORT_PIN_IN,
        .Speed                = 0,
        .DirectionChangeable  = 0,
        .Level                = 0,
        .Pull                 = PORT_PIN_PULL_NONE,
        .ModeChangeable       = 0
    },
    /* PA1 - TIM2_CH2 - PWM Output */
    {
        .PortNum              = PORT_ID_A,
        .PinNum               = 1,
        .Mode                 = PORT_PIN_MODE_AF_PP,
        .Direction            = PORT_PIN_OUT,
        .Speed                = GPIO_Speed_50MHz,
        .DirectionChangeable  = 0,
        .Level                = PORT_PIN_LEVEL_LOW,
        .Pull                 = PORT_PIN_PULL_NONE,
        .ModeChangeable       = 0
    },
    /* PC13 - LED onboard (active LOW) */
    {
        .PortNum              = PORT_ID_C,
        .PinNum               = 13,
        .Mode                 = PORT_PIN_MODE_DIO,
        .Direction            = PORT_PIN_OUT,
        .Speed                = GPIO_Speed_2MHz,
        .DirectionChangeable  = 1,
        .Level                = PORT_PIN_LEVEL_HIGH,
        .Pull                 = PORT_PIN_PULL_NONE,
        .ModeChangeable       = 0
    }
};

/* Tổng số chân cấu hình */
const uint32 PortCfg_PinsCount = sizeof(PortCfg_Pins) / sizeof(Port_PinConfigType);

/* Cấu hình tổng Port driver */
const Port_ConfigType PortCfg_Config = {
    .PinConfigs = PortCfg_Pins,
    .PinCount   = PortCfg_PinsCount,
};
