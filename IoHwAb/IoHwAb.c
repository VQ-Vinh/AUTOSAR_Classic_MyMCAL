#include "IoHwAb.h"
#include "IoHwAb_Cfg.h"
#include "Port.h"
#include "Dio.h"
#include "Adc.h"
#include "Adc_Cfg.h"
#include "Pwm.h"

/**********************************************************
* Static state
**********************************************************/
static uint8 IoHwAb_IsInitialized = 0U;

/* ADC temperature read (background polling) */
#define TEMP_ADC_GROUP     ((Adc_GroupType)0U)
#define TEMP_ADC_CHANNEL   ((uint8)0U)
static Adc_ValueGroupType gTemperatureRaw;
static uint16 gTemperatureC;
static boolean gTempReady;

/* ADC result buffer */
static Adc_ValueGroupType gAdcBuffer[ADC_MAX_CHANNELS_PER_GROUP];

/**********************************************************
* Digital signal-to-pin mapping
**********************************************************/
typedef struct
{
    Dio_ChannelType dioChannel;
    boolean invert; /* for active-low signals */
} IoHwAb_SignalMapping;

static const IoHwAb_SignalMapping gSignalMap[IOHWAB_SIG_COUNT] = {
    /* IOHWAB_SIG_BUTTON1 */ { DIO_CHANNEL_A0, FALSE },
    /* IOHWAB_SIG_LED1    */ { DIO_CHANNEL_C13, TRUE  }, /* PC13: active LOW */
};

/**********************************************************
* Init
**********************************************************/
void IoHwAb_Init0(const IoHwAb_ConfigType *ConfigPtr)
{
    (void)ConfigPtr;

    if (IoHwAb_IsInitialized != 0U)
    {
        return;
    }

    /* AUTOSAR spec: ConfigPtr MUST be NULL_PTR for template instance.
     * Use the globally-defined IoHwAb_Config internally. */
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

    gTempReady = FALSE;
    IoHwAb_IsInitialized = 1U;
}

/**********************************************************
* Version Info
**********************************************************/
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

/**********************************************************
* Main Function
**********************************************************/
void IoHwAb_MainFunction(void)
{
    Adc_StatusType adcStatus;

    if (IoHwAb_IsInitialized == 0U)
    {
        return;
    }

    adcStatus = Adc_GetGroupStatus(TEMP_ADC_GROUP);

    if (adcStatus == ADC_IDLE)
    {
        if (Adc_SetupResultBuffer(TEMP_ADC_GROUP, gAdcBuffer) == E_OK)
        {
            Adc_StartGroupConversion(TEMP_ADC_GROUP);
        }
    }
    else if (adcStatus == ADC_COMPLETED)
    {
        Adc_ValueGroupType raw;

        if (Adc_ReadGroup(TEMP_ADC_GROUP, &raw) == E_OK)
        {
            gTemperatureRaw = raw;
            gTemperatureC = (uint16)((raw * 3300UL / 4095UL) / 10UL);
            gTempReady = TRUE;
        }
    }
    else
    {
    }
}

/**********************************************************
* Digital Read
**********************************************************/
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

/**********************************************************
* Digital Write
**********************************************************/
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

/**********************************************************
* ADC Read Signal
**********************************************************/
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

/**********************************************************
* PWM Duty
**********************************************************/
void IoHwAb_SetPwmDuty(Pwm_ChannelType channel, uint16 duty)
{
    Pwm_SetDutyCycle(channel, duty);
}

/**********************************************************
* PWM Convert To Period
**********************************************************/
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

/**********************************************************
* Get Temperature
**********************************************************/
uint16 IoHwAb_GetTemperature(IoHwAb_StatusType *status)
{
    if (status != NULL_PTR)
    {
        *status = IOHWAB_BUSY;
    }

    if (IoHwAb_IsInitialized == 0U)
    {
        return 0U;
    }

    if (gTempReady)
    {
        if (status != NULL_PTR)
        {
            *status = IOHWAB_OK;
        }
        gTempReady = FALSE;
        return gTemperatureC;
    }

    return 0U;
}

/**********************************************************
* Set Heater Power (0-100% -> PWM duty)
**********************************************************/
Std_ReturnType IoHwAb_SetHeaterPower(uint8 percent)
{
    uint16 duty;

    if (percent > 100U)
    {
        return E_NOT_OK;
    }

    duty = (uint16)(((uint32)percent * 0x8000UL) / 100UL);
    Pwm_SetDutyCycle(1U, duty);
    return E_OK;
}

/**********************************************************
* Notification callbacks (MCAL -> IoHwAb)
**********************************************************/
void IoHwAb_AdcNotification0(void)
{
    Adc_ReadGroup(TEMP_ADC_GROUP, &gTemperatureRaw);
    gTemperatureC = (uint16)((gTemperatureRaw * 3300UL / 4095UL) / 10UL);
    gTempReady = TRUE;
}

void IoHwAb_AdcNotification1(void)
{
}
