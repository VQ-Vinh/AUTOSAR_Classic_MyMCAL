#include "Pwm.h"

const Pwm_ChannelConfigType PwmChannelsConfig[] = {
    /* Channel 0: PA1 - TIM2_CH2 - PWM (10kHz, 50% duty) */
    {
        .TIMx             = TIM2,
        .channel          = 2,
        .classType        = PWM_VARIABLE_PERIOD,
        .defaultPeriod    = 7199,          // 10kHz: 72MHz / (0+1) / (7199+1)
        .defaultDutyCycle = 0x4000,        // Duty 50%
        .polarity         = PWM_HIGH,
        .idleState        = PWM_LOW,
        .NotificationCb   = NULL
    }
};

const Pwm_ConfigType PwmDriverConfig = {
    .Channels    = PwmChannelsConfig,
    .NumChannels = sizeof(PwmChannelsConfig) / sizeof(Pwm_ChannelConfigType)
};
