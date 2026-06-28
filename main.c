/**********************************************************
 * @file    main.c
 * @brief   Application entry point
 * @details Khởi tạo MCAL drivers và vòng lặp chính.
 *          Ví dụ: Port_Init -> Dio_WriteChannel.
 * @version 1.0
 * @date    2024-09-30
 * @author  HALA Academy
 **********************************************************/

#include "Std_Types.h"
#include "Port.h"
#include "Port_cfg.h"
#include "Dio.h"

static void delay(volatile uint32 count)
{
    while (count > 0U)
    {
        count--;
    }
}

int main(void)
{
    Port_Init(&PortCfg_Config);

    while (1)
    {
        Dio_WriteChannel(DIO_CHANNEL_A0, STD_HIGH);
        delay(500000U);
        Dio_WriteChannel(DIO_CHANNEL_A0, STD_LOW);
        delay(500000U);
    }
}
