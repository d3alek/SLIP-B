TARGET_CHIP := NRF51822_QFAA_CA
BOARD := BOARD_NRF6310

C_SOURCE_FILES += main_tx.c

C_SOURCE_FILES += radio_config.c
C_SOURCE_FILES += slip_ble.c
C_SOURCE_FILES += libalek.c

C_SOURCE_FILES += twi_sw_master.c

C_SOURCE_FILES += ble_advdata.c
C_SOURCE_FILES += ble_stack_handler.c
C_SOURCE_FILES += ble_conn_params.c
C_SOURCE_FILES += ble_ms.c
C_SOURCE_FILES += ble_init.c
C_SOURCE_FILES += ble_characteristics.c
C_SOURCE_FILES += nrf_delay.c
C_SOURCE_FILES += simple_uart.c
C_SOURCE_FILES += libjanek.c

C_SOURCE_FILES += app_timer.c
C_SOURCE_FILES += app_button.c
C_SOURCE_FILES += app_scheduler.c
C_SOURCE_FILES += app_gpiote.c



OUTPUT_FILENAME := radio_rx_example_gcc
SDK_PATH := ../nrf51_sdk/

JLINK_PATH			:= ../opt/JLink_Linux_V474

GNU_INSTALL_ROOT	:= ../opt/arm-2013.05
GNU_VERSION			:= 4.7.2
GNU_PREFIX			:= arm-none-eabi


#Uncomment correct line if you have softdevice programmed on the chip.
DEVICE_VARIANT := xxaa
#DEVICE_VARIANT := xxab

USE_SOFTDEVICE := S110
#USE_SOFTDEVICE := S210

include $(SDK_PATH)Source/templates/gcc/Makefile.common
