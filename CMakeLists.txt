# Top-level CMake build configuration for AUTOSAR MCAL on STM32F103 (SPL)
cmake_minimum_required(VERSION 3.10)
project(AUTOSAR_MCAL C ASM)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Platform library (SPL + CMSIS + Startup)
add_subdirectory(platform)

# MCAL Drivers
set(MCAL_SOURCES
    MCAL/Adc/Adc.c
    MCAL/Dio/Dio.c
    MCAL/Port/Port.c
    MCAL/Pwm/Pwm.c
)

set(MCAL_INCLUDES
    MCAL/Adc
    MCAL/Dio
    MCAL/Port
    MCAL/Pwm
    IoHwAb
    Config
)

# Application + Config
set(APP_SOURCES
    main.c
    IoHwAb/IoHwAb.c
    IoHwAb/IoHwAb_Cfg.c
    Config/Adc_Cfg.c
    Config/Dio_Cfg.c
    Config/Port_cfg.c
    Config/Pwm_Lcfg.c
)

# Main executable
add_executable(mcal.elf
    ${APP_SOURCES}
    ${MCAL_SOURCES}
)

target_include_directories(mcal.elf PRIVATE
    ${MCAL_INCLUDES}
)

target_link_libraries(mcal.elf PRIVATE
    platform
)
