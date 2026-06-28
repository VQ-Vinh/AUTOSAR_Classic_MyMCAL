/**********************************************************
 * @file    Adc_Types.h
 * @brief   ADC Driver Type Definitions (AUTOSAR)
 * @details Định nghĩa các kiểu dữ liệu và hằng số dùng
 *          bởi ADC Driver theo chuẩn AUTOSAR.
 * @version 2.0
 * @date    2026-03-21
 * @author  HALA Academy
 **********************************************************/

#ifndef ADC_TYPES_H
#define ADC_TYPES_H

#include "Std_Types.h"
#include "stm32f10x.h"

/**********************************************************
 * Kiểu dữ liệu cơ bản
 **********************************************************/
typedef uint8  Adc_GroupType;
typedef uint16 Adc_ValueGroupType;
typedef uint8  Adc_StreamNumSampleType;
typedef uint8  Adc_PowerStateType;

/**********************************************************
 * Trạng thái group ADC
 **********************************************************/
typedef enum
{
    ADC_IDLE             = 0x00U,
    ADC_BUSY             = 0x01U,
    ADC_COMPLETED        = 0x02U,
    ADC_STREAM_COMPLETED = 0x03U
} Adc_StatusType;

/**********************************************************
 * Kiểu kết quả yêu cầu power state
 **********************************************************/
typedef enum
{
    ADC_SERVICE_ACCEPTED    = 0x00U,
    ADC_NOT_INIT            = 0x01U,
    ADC_POWER_STATE_NOT_SUPP = 0x02U,
    ADC_SEQUENCE_ERROR       = 0x03U,
    ADC_TRANSITION_NOT_POSSIBLE = 0x04U
} Adc_PowerStateRequestResultType;

/**********************************************************
 * Hằng số power state
 **********************************************************/
#define ADC_FULL_POWER     ((Adc_PowerStateType)0x00U)
#define ADC_LOW_POWER_STATE ((Adc_PowerStateType)0x01U)

/**********************************************************
 * Hằng số mode chuyển đổi
 **********************************************************/
#define ADC_CONV_MODE_ONESHOT    0x00U
#define ADC_CONV_MODE_CONTINUOUS 0x01U

/**********************************************************
 * Hằng số nguồn trigger
 **********************************************************/
#define ADC_TRIGG_SRC_SW 0x00U
#define ADC_TRIGG_SRC_HW 0x01U

/**********************************************************
 * Hằng số access mode
 **********************************************************/
#define ADC_ACCESS_MODE_SINGLE    0x00U
#define ADC_ACCESS_MODE_STREAMING 0x01U

/**********************************************************
 * Hằng số stream buffer mode
 **********************************************************/
#define ADC_STREAM_BUFFER_LINEAR   0x00U
#define ADC_STREAM_BUFFER_CIRCULAR 0x01U

/**********************************************************
 * @struct Adc_ChannelConfigType
 * @brief  Cấu hình một kênh ADC (channel, rank, sampling time)
 **********************************************************/
typedef struct
{
    uint8  ChannelId;
    uint8  Rank;
    uint32 SamplingTime;
} Adc_ChannelConfigType;

/**********************************************************
 * @struct Adc_DmaConfigType
 * @brief  Cấu hình DMA cho ADC group
 **********************************************************/
typedef struct
{
    DMA_Channel_TypeDef *DmaChannel;
    uint32               Priority;
    uint32               DmaTcFlag;
    uint32               DmaHtFlag;
    uint16               DmaTcItMask;
    uint16               DmaHtItMask;
} Adc_DmaConfigType;

/**********************************************************
 * @struct Adc_GroupConfigType
 * @brief  Cấu hình một group ADC
 **********************************************************/
typedef struct
{
    Adc_GroupType                GroupId;
    ADC_TypeDef                 *AdcInstance;
    const Adc_ChannelConfigType *ChannelList;
    uint8                        NumChannels;
    uint8                        ConversionMode;
    uint8                        TriggerSource;
    uint32                       HwTriggerSource;
    uint8                        AccessMode;
    uint8                        StreamBufferMode;
    Adc_StreamNumSampleType      StreamNumSamples;
    const Adc_DmaConfigType     *DmaConfig;
    void                       (*NotificationCb)(void);
    void                       (*DmaHalfCb)(void);
    void                       (*DmaCompleteCb)(void);
} Adc_GroupConfigType;

/**********************************************************
 * @struct Adc_ConfigType
 * @brief  Cấu hình tổng thể ADC driver
 **********************************************************/
typedef struct
{
    const Adc_GroupConfigType *Groups;
    uint8                      NumGroups;
} Adc_ConfigType;

#endif /* ADC_TYPES_H */
