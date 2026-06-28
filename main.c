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
        IoHwAb_Digital_WriteSignal(IOHWAB_SIG_LED1, TRUE);
        delay(3000000U);
        IoHwAb_Digital_WriteSignal(IOHWAB_SIG_LED1, FALSE);
        delay(3000000U);
    }
}
