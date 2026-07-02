# AUTOSAR MCAL — Mini Project: Fan Speed Control via ADC + PWM (STM32F103)

## Overview

Demo project sử dụng AUTOSAR MCAL drivers để đọc **biến trở** qua **ADC** và điều khiển tốc độ **quạt** bằng **PWM** trên STM32F103.

## Hardware

| Pin | Mode | Chức năng |
|-----|------|-----------|
| PA0 | ADC1_IN0 | Biến trở 10k (0–3.3V → 0–100%) |
| PA1 | TIM2_CH2 | PWM → BJT/MOSFET → quạt |
| PC13 | GPIO Out | LED onboard (active LOW) |

## Logic

```c
if (adcPercent > 50) {
    fanDuty = adcPercent;   // ADC 60% → fan 60%
    LED = ON;
} else {
    fanDuty = 0;            // quạt tắt
    LED = OFF;
}
```

## Build

```bash
make          # build
make f1       # clean + rebuild
make flash    # build + flash STM32
make clean    # xóa build/
```

## Project Structure

```
My_MCAL/
├── Config/           # Cấu hình (Port_cfg, Pwm_Lcfg, Adc_Cfg, ...)
├── IoHwAb/           # Lớp trừu tượng I/O Hardware Abstraction
├── MCAL/             # AUTOSAR MCAL drivers (Port, Dio, Adc, Pwm)
├── platform/         # SPL, startup, linker script
├── main.c            # Logic ứng dụng
└── AGENTS.md         # Gotchas cho AI
```

## Dependencies

- Toolchain: `arm-none-eabi-gcc` (MinGW)
- Flasher: `st-flash` (ST-Link)
- Build: CMake + MinGW Makefiles
