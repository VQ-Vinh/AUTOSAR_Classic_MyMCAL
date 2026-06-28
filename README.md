# Mini-Project: Hệ Thống Điều Khiển Quạt Làm Mát Theo Nhiệt Độ

## Mô tả

Hệ thống sử dụng **STM32F103C8T6** và các driver **AUTOSAR MCAL** để điều khiển quạt DC 12V dựa trên nhiệt độ môi trường đọc từ cảm biến analog (LM35/NTC).

| Điều kiện | Hành động |
|-----------|-----------|
| Nhiệt độ < 30°C | Quạt tắt, LED tắt |
| 30°C ≤ Nhiệt độ < 40°C | Quạt 50% duty, LED bật |
| Nhiệt độ ≥ 40°C | Quạt 100% duty, LED bật |

## Kiến trúc phần mềm

```
Application (main.c)
      |
  IoHwAb Layer          ← ECU Abstraction, API duy nhất cho ứng dụng
      |
  MCAL Layer
  ┌──────┬──────┬──────┬──────┐
  │ Adc  │ Dio  │ Port │ Pwm  │
  └──┬───┴──┬───┴──┬───┴──┬───┘
     │      │      │      │
     └─── SPL (Standard Peripheral Library) ───┘
                      |
                 CMSIS + Startup
```

## Yêu cầu phần cứng

| Linh kiện | Chức năng | Kết nối STM32 |
|-----------|-----------|---------------|
| STM32F103C8T6 | Vi điều khiển | — |
| LM35 hoặc NTC | Cảm biến nhiệt | PA0 (ADC1_IN0) |
| IRLZ44N MOSFET | Điều khiển quạt PWM | PA8 (TIM1_CH1) |
| Quạt DC 12V | Làm mát | MOSFET drain |
| LED + 220Ω | Báo trạng thái quạt | PC13 (active LOW) |

## Cấu trúc thư mục

```
My_MCAL/
├── main.c                  # Ứng dụng: gọi IoHwAb API
├── Makefile                # Targets: all, f1, flash, clean
├── arm-none-eabi-gcc.cmake # Toolchain file
├── README.md
│
├── IoHwAb/                 # ECU Abstraction Layer
│   ├── IoHwAb.h
│   ├── IoHwAb.c
│   ├── IoHwAb_Cfg.h
│   └── IoHwAb_Cfg.c
│
├── MCAL/                   # MCAL Drivers
│   ├── Adc/                #   ADC driver
│   ├── Dio/                #   DIO driver
│   ├── Port/               #   Port driver
│   └── Pwm/                #   PWM driver
│
├── Config/                 # Cấu hình MCAL
│   ├── Adc_Cfg.c / .h
│   ├── Dio_Cfg.c
│   ├── Port_cfg.c / .h
│   └── Pwm_Lcfg.c
│   └── Pwm_Cfg.h
│
└── platform/               # Platform (SPL + CMSIS + startup)
    ├── CMakeLists.txt
    ├── include/
    ├── bsp/                #   CMSIS, linker script, startup
    ├── spl/                #   STM32 SPL
    └── debug/              #   (local, không track git)
```

## Build & Flash

```bash
make          # Build (auto-configure nếu chưa có build/)
make f1       # Clean + build lại từ đầu
make flash    # Build + nạp xuống STM32F1 qua ST-Link
make clean    # Xoá thư mục build/
```

## Trạng thái hiện tại

| Module | Trạng thái | Ghi chú |
|--------|-----------|---------|
| Port Driver | ✅ Hoàn thành | PA0(ADC), PA8(PWM), PC13(LED) |
| Dio Driver | ✅ Hoàn thành | Đọc/ghi từng chân, port, group |
| Adc Driver | ✅ Hoàn thành | 2 groups: SW one-shot + HW streaming DMA |
| Pwm Driver | ✅ Hoàn thành | TIM1_CH1(PA8), TIM3_CH2(PA7) |
| IoHwAb | ✅ Hoàn thành | Init, Digital R/W, ADC, PWM, callbacks |
| main.c | ✅ Blink PC13 | Đang test LED, chưa có logic quạt |
| Platform | ✅ Hoàn thành | SPL, CMSIS, startup, linker script |

## Kế hoạch thực hiện

### Bước 1: Cấu hình Port Driver
- [x] PA0: Input analog (ADC1_IN0)
- [ ] PA8: PWM output (TIM1_CH1) — cần thêm vào `Port_cfg.c` và cập nhật `Pwm_Lcfg.c`
- [x] PC13: GPIO output (LED status, active LOW)

### Bước 2: Cấu hình ADC
- [x] ADC1, single channel, SW trigger, one-shot
- [ ] Thêm group mới cho cảm biến nhiệt (hoặc dùng group 0 có sẵn)
- [ ] Công thức scale: LM35 (10mV/°C, Vref=3.3V, 12-bit) → `temp = raw * 3300 / 4095 / 10`

### Bước 3: Cấu hình PWM
- [ ] Thêm kênh TIM1_CH1 (PA8) vào `Pwm_Lcfg.c`
- [ ] Tần số 10 kHz, duty cycle điều chỉnh 0–100%

### Bước 4: Cập nhật IoHwAb
- [ ] Thêm API `IoHwAb_ReadTemperature(uint8 *temp)` — đọc ADC → scale → °C
- [ ] Thêm API `IoHwAb_SetFanDuty(uint8 percent)` — gọi `Pwm_SetDutyCycle`
- [ ] Cập nhật `IoHwAb_Cfg.h` nếu cần mapping mới

### Bước 5: Viết ứng dụng (main.c)
- [ ] Vòng lặp chính: đọc nhiệt độ → so sánh ngưỡng → điều khiển quạt + LED
- [ ] Chỉ gọi API IoHwAb, không gọi trực tiếp MCAL

### Bước 6: Kiểm thử
- [ ] Build sạch, zero warning
- [ ] Flash và kiểm tra trên board
- [ ] Debug qua OpenOCD nếu cần

## Tài liệu tham khảo

- `docs/` — Hướng dẫn API từng module MCAL, lý thuyết IoHwAb, đề bài mini-project
