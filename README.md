# AUTOSAR Classic MCAL — STM32F103

Dự án **Microcontroller Abstraction Layer (MCAL)** theo chuẩn **AUTOSAR Classic** dành cho vi điều khiển **STM32F103 (Cortex-M3)**, sử dụng **Standard Peripheral Library (SPL)** của STMicroelectronics.

## Kiến trúc phần mềm

```
Application Layer (main.c)
       |
IoHwAb Layer (ECU Abstraction)  ← chưa triển khai
       |
   MCAL Layer
  ┌─────┬─────┬──────┬──────┐
  │ Adc │ Dio │ Port │ Pwm │
  └──┬──┴──┬──┴──┬───┴──┬───┘
     │     │     │      │
     └── SPL (Standard Peripheral Library) ──┘
                    │
               CMSIS + Startup
```

## Cấu trúc thư mục

```
My_MCAL/
├── main.c                  # Entry point (rỗng)
├── CMakeLists.txt          # Build gốc (rỗng)
├── README.md
│
├── Config/                 # Cấu hình post-build / link-time
│   ├── Adc_Cfg.c           #   2 groups: SW one-shot + HW streaming DMA
│   ├── Dio_Cfg.c           #   (rỗng)
│   ├── Port_cfg.c          #   PA0 (TIM2_CH1), PA7 (TIM3_CH2)
│   └── Pwm_Lcfg.c          #   TIM2_CH1 + TIM3_CH2
│
├── MCAL/                   # MCAL Drivers
│   ├── Adc/                #   ADC driver (~1417 dòng)
│   ├── Dio/                #   DIO driver (~259 dòng)
│   ├── Port/               #   Port driver (~196 dòng)
│   └── Pwm/                #   PWM driver (~465 dòng)
│
├── platform/               # Platform layer (build static lib)
│   ├── CMakeLists.txt      #   Build platform library
│   ├── include/Std_Types.h #   Kiểu AUTOSAR chuẩn
│   ├── bsp/
│   │   ├── cmsis/          #   CMSIS (core_cm3.h)
│   │   ├── linker/         #   Linker script (stm32f103.ld)
│   │   └── startup_stm32f10x_md.s
│   ├── debug/              #   Semihost, SVD, syscalls
│   └── spl/                #   STM32 SPL (24 .c + 26 .h)
│
├── docs/                   # Tài liệu hướng dẫn
│   ├── [Tài liệu] Hướng dẫn sử dụng API module ADC (MCAL).md
│   ├── [Tài liệu] Hướng dẫn sử dụng API module DIO (MCAL).md
│   ├── [Tài liệu] Hướng dẫn sử dụng API module PORT (MCAL).md
│   ├── [Tài liệu] Hướng dẫn sử dụng API module PWM (MCAL).md
│   ├── [Mini-Project 1] Hệ thống điều khiển quạt làm mát động cơ.md
│   └── I_O Hardware Abstraction - IoHwAb) trong AUTOSAR.md
│
└── images/
```

## MCAL Drivers

### ADC Driver (`MCAL/Adc/`)
- API: Init, DeInit, Start/StopGroupConversion, ReadGroup, SetupResultBuffer, Enable/DisableHardwareTrigger, Enable/DisableGroupNotification, GetGroupStatus, GetStreamLastPointer, GetVersionInfo, Power State APIs
- Hỗ trợ Single / Streaming mode, DMA (circular / linear), SW / HW trigger
- ISR handler cho EOC và DMA HT/TC
- Config: Group 0 (SW trigger, one-shot, single access), Group 1 (HW trigger, continuous, streaming + DMA circular)

### DIO Driver (`MCAL/Dio/`)
- API: ReadChannel, WriteChannel, ReadPort, WritePort, ReadChannelGroup, WriteChannelGroup, FlipChannel, GetVersionInfo
- Macro mapping `DIO_CHANNEL(Port, Pin)` cho GPIOA-D (0-63)

### Port Driver (`MCAL/Port/`)
- API: Init, SetPinDirection, RefreshPortDirection, SetPinMode, GetVersionInfo
- Cấu hình: mode (DIO/ADC/PWM/SPI/CAN/LIN), direction, speed, pull, level, changeable flags

### PWM Driver (`MCAL/Pwm/`)
- API: Init, DeInit, SetDutyCycle, SetPeriodAndDuty, SetOutputToIdle, GetOutputState, Enable/DisableNotification, GetVersionInfo, Power State APIs
- Hỗ trợ TIM1-4, variable / fixed period, notification edge

## Hardware Target

| Thành phần | Mô tả |
|-----------|-------|
| MCU | STM32F103C8T6 (Cortex-M3, 72 MHz) |
| Flash | 64 KB |
| RAM | 20 KB |
| SPL | STM32F10x Standard Peripheral Library |
| CMSIS | core_cm3 |

## Yêu cầu

- **Toolchain**: ARM GCC (`arm-none-eabi-gcc`)
- **Build**: CMake >= 3.10
- **Debug**: OpenOCD + ST-Link (hoặc semihosting)

## Build

```bash
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=../cmake/arm-none-eabi.cmake
make
```

> **Ghi chú**: `CMakeLists.txt` gốc ở thư mục đang rỗng — cần bổ sung cấu hình build.

## Vấn đề cần xử lý

| # | Vấn đề | File | Mức độ |
|---|--------|------|--------|
| 1 | Thiếu `Adc_Types.h` | `MCAL/Adc/Adc.h:17` | Cao |
| 2 | Thiếu `Adc_Cfg.h` | `MCAL/Adc/Adc.c:13`, `Config/Adc_Cfg.c:11` | Cao |
| 3 | Thiếu `Port_cfg.h` | `MCAL/Port/Port.c:17` | Cao |
| 4 | Thiếu `Pwm_Cfg.h` | `MCAL/Pwm/Pwm.c:13` | Cao |
| 5 | Thiếu define `PORT_PIN_MODE_AF_PP` | `Config/Port_cfg.c:23,33` | Cao |
| 6 | Lỗi cú pháp thiếu `;` | `MCAL/Port/Port.c:83` | Cao |
| 7 | `main.c` rỗng | `main.c` | Trung bình |
| 8 | `CMakeLists.txt` rỗng | `CMakeLists.txt` | Trung bình |

## Tài liệu tham khảo

Xem thêm tại thư mục [`docs/`](docs/) bao gồm:
- Hướng dẫn API từng module MCAL (ADC, DIO, Port, PWM)
- Mini-project: Hệ thống điều khiển quạt làm mát động cơ
- Lý thuyết về IoHwAb (I/O Hardware Abstraction) trong AUTOSAR
