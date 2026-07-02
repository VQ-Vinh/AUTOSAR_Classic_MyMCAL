# AGENTS.md — AUTOSAR MCAL on STM32F103

## Build & Flash

```bash
make          # incremental build (auto-configures on first run)
make f1       # clean + rebuild
make flash    # build → objcopy → st-flash write at 0x08000000
make clean    # rm -rf build/
```

Cross-compiler: `arm-none-eabi-gcc` via `arm-none-eabi-gcc.cmake` toolchain file.  
Build output goes to `build/` only — root `.o` files are stale.

## Critical Gotchas

- **Port pin `.Speed` field must be set.** Value 0 programs the pin as input regardless of mode. Use `GPIO_Speed_2MHz` or `GPIO_Speed_50MHz`.
- **PC13 is active LOW.** IoHwAb signal mapping uses `invert = TRUE` for `IOHWAB_SIG_LED1` — writing `TRUE` drives the pin LOW (LED on), `FALSE` drives HIGH (LED off).
- **`IoHwAb_Init0(NULL_PTR)`** is the correct init call. The `ConfigPtr` MUST be `NULL_PTR` per AUTOSAR convention; the function reads the global `IoHwAb_Config` internally.
- **`Det_ReportError` is a no-op macro** (expands to nothing). DET error reporting is compiled in but has no backend.
- **No `--gc-sections` in linker flags.** Unused SPL functions in `platform/spl/src/*.c` still consume flash.

## Adding a New I/O Signal

1. Add pin config in `Config/Port_cfg.c` (include `.Speed`!)
2. Add entry to `IoHwAb_SignalType` enum in `IoHwAb/IoHwAb_Cfg.h`
3. Add `Dio_ChannelType` + invert flag to `gSignalMap[]` in `IoHwAb/IoHwAb.c`

## Pin Config

Port pins live in `Config/Port_cfg.c`. Current mapping:

| Pin | Mode | Function |
|-----|------|----------|
| PA0 | ADC | ADC1_IN0 (biến trở 10k) |
| PA1 | AF_PP | TIM2_CH2 (PWM quạt) |
| PC13 | DIO_OUT | LED (active LOW) |

## PWM Duty Range

`Pwm_SetDutyCycle(channel, duty)` — `duty` is `0x0000` (0%) to `0x8000` (100%), **not** 0–100.

## Active Std_Types.h

`platform/include/Std_Types.h` is the canonical AUTOSAR type header.  
The root `./Std_Types.h` is dead code — do not edit.

## ADC

- Group 0: ADC1, channel 0 (PA0), SW-trigger one-shot
- `IoHwAb_ReadAdcPercent` reads raw ADC and scales to 0-100%
