#include "Std_Types.h"
#include "IoHwAb.h"
#include "IoHwAb_Cfg.h"

static void delay(volatile uint32 count)
{
    while (count > 0U)
    {
        count--;
    }
}

int main(void)
{
    IoHwAb_Init0(NULL_PTR);

    while (1)
    {
        uint8 pct;
        uint8 duty;

        if (IoHwAb_ReadAdcPercent(&pct) == E_OK)
        {
            if (pct > 50U)
            {
                duty = pct;
                IoHwAb_SetFanDuty(duty);
                IoHwAb_Digital_WriteSignal(IOHWAB_SIG_LED1, TRUE);
            }
            else
            {
                IoHwAb_SetFanDuty(0U);
                IoHwAb_Digital_WriteSignal(IOHWAB_SIG_LED1, FALSE);
            }
        }

        delay(100000U);
    }
}
