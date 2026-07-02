#include "IoHwAb.h"
#include "IoHwAb_Cfg.h"
#include "Port.h"
#include "Dio.h"
#include "Adc.h"
#include "Adc_Cfg.h"
#include "Pwm.h"
#include "stm32f10x_adc.h"

static uint8 IoHwAb_IsInitialized = 0U;

#define ADC_GROUP       ((Adc_GroupType)0U)
#define ADC_CHANNEL     ((uint8)0U)

static Adc_ValueGroupType gAdcBuffer[ADC_MAX_CHANNELS_PER_GROUP];

typedef struct
{
    Dio_ChannelType dioChannel;
    boolean invert;
} IoHwAb_SignalMapping;

static const IoHwAb_SignalMapping gSignalMap[IOHWAB_SIG_COUNT] = {
    /* IOHWAB_SIG_BUTTON1 */ { DIO_CHANNEL_A0, FALSE },
    /* IOHWAB_SIG_LED1    */ { DIO_CHANNEL_C13, TRUE  },
};

void IoHwAb_Init0(const IoHwAb_ConfigType *ConfigPtr)
{
    (void)ConfigPtr;

    if (IoHwAb_IsInitialized != 0U)
    {
        return;
    }

    {
        const IoHwAb_ConfigType *cfg = &IoHwAb_Config;

        if (cfg->portConfig != NULL_PTR)
        {
            Port_Init(cfg->portConfig);
        }

        if (cfg->adcConfig != NULL_PTR)
        {
            Adc_Init(cfg->adcConfig);
        }

        if (cfg->pwmConfig != NULL_PTR)
        {
            Pwm_Init(cfg->pwmConfig);
        }
    }

    IoHwAb_IsInitialized = 1U;
}

void IoHwAb_GetVersionInfo(Std_VersionInfoType *versioninfo)
{
    if (versioninfo == NULL_PTR)
    {
        return;
    }

    versioninfo->vendorID = IOHWAB_VENDOR_ID;
    versioninfo->moduleID = IOHWAB_MODULE_ID;
    versioninfo->sw_major_version = IOHWAB_SW_MAJOR_VERSION;
    versioninfo->sw_minor_version = IOHWAB_SW_MINOR_VERSION;
    versioninfo->sw_patch_version = IOHWAB_SW_PATCH_VERSION;
}

void IoHwAb_MainFunction(void)
{
    if (IoHwAb_IsInitialized == 0U)
    {
        return;
    }
}

Std_ReturnType IoHwAb_Digital_ReadSignal(IoHwAb_SignalType id, boolean *value)
{
    Dio_LevelType level;

    if (IoHwAb_IsInitialized == 0U)
    {
        return E_NOT_OK;
    }

    if (id >= IOHWAB_SIG_COUNT)
    {
        return E_NOT_OK;
    }

    if (value == NULL_PTR)
    {
        return E_NOT_OK;
    }

    level = Dio_ReadChannel(gSignalMap[id].dioChannel);

    if (gSignalMap[id].invert)
    {
        *value = (level == STD_LOW) ? TRUE : FALSE;
    }
    else
    {
        *value = (level == STD_HIGH) ? TRUE : FALSE;
    }

    return E_OK;
}

Std_ReturnType IoHwAb_Digital_WriteSignal(IoHwAb_SignalType id, boolean value)
{
    Dio_LevelType level;

    if (IoHwAb_IsInitialized == 0U)
    {
        return E_NOT_OK;
    }

    if (id >= IOHWAB_SIG_COUNT)
    {
        return E_NOT_OK;
    }

    if (gSignalMap[id].invert)
    {
        level = (value == TRUE) ? STD_LOW : STD_HIGH;
    }
    else
    {
        level = (value == TRUE) ? STD_HIGH : STD_LOW;
    }

    Dio_WriteChannel(gSignalMap[id].dioChannel, level);
    return E_OK;
}

Adc_ValueGroupType IoHwAb_Adc_ReadSignal(Adc_GroupType group, uint8 channel, IoHwAb_StatusType *status)
{
    (void)channel;

    if (status != NULL_PTR)
    {
        *status = IOHWAB_BUSY;
    }

    if (IoHwAb_IsInitialized == 0U)
    {
        return 0U;
    }

    {
        Adc_ValueGroupType buf;
        Adc_SetupResultBuffer(group, &buf);
        Adc_StartGroupConversion(group);

        {
            uint32 timeout = 1000000U;
            while ((Adc_GetGroupStatus(group) != ADC_COMPLETED) && (timeout > 0U))
            {
                timeout--;
            }
        }

        if (Adc_ReadGroup(group, &buf) == E_OK)
        {
            if (status != NULL_PTR)
            {
                *status = IOHWAB_OK;
            }
            return buf;
        }
    }

    if (status != NULL_PTR)
    {
        *status = IOHWAB_TIMEOUT;
    }
    return 0U;
}

void IoHwAb_SetPwmDuty(Pwm_ChannelType channel, uint16 duty)
{
    Pwm_SetDutyCycle(channel, duty);
}

Pwm_PeriodType IoHwAb_Pwm_ConvertToPeriod(Pwm_ChannelType channel, IoHwAb_FrequencyType freq)
{
    const uint32 timerClk = 1000000UL;

    (void)channel;

    if (freq == 0UL)
    {
        return 0U;
    }

    return (Pwm_PeriodType)(timerClk / freq);
}

Std_ReturnType IoHwAb_ReadAdcPercent(uint8 *percent)
{
    Adc_ValueGroupType raw;

    if (IoHwAb_IsInitialized == 0U)
    {
        return E_NOT_OK;
    }

    if (percent == NULL_PTR)
    {
        return E_NOT_OK;
    }

    if (Adc_SetupResultBuffer(ADC_GROUP, &raw) != E_OK)
    {
        return E_NOT_OK;
    }

    Adc_StartGroupConversion(ADC_GROUP);

    {
        uint32 timeout = 1000000U;
        while ((ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET) && (timeout > 0U))
        {
            timeout--;
        }

        if (timeout == 0U)
        {
            return E_NOT_OK;
        }
    }

    if (Adc_ReadGroup(ADC_GROUP, &raw) != E_OK)
    {
        return E_NOT_OK;
    }

    *percent = (uint8)((raw * 100UL) / 4095UL);
    return E_OK;
}

Std_ReturnType IoHwAb_SetFanDuty(uint8 percent)
{
#define FAN_PWM_CHANNEL ((Pwm_ChannelType)0U)

    if (percent > 100U)
    {
        return E_NOT_OK;
    }

    Pwm_SetDutyCycle(FAN_PWM_CHANNEL, (uint16)(((uint32)percent * 0x8000UL) / 100UL));
    return E_OK;
}

void IoHwAb_AdcNotification0(void)
{
}

void IoHwAb_AdcNotification1(void)
{
}
