################################################################################
# Daiichi AVAS Ford Project Makefile
#
# Project Specifications:
# MCU: FS32K116LAT0MLFT
# Toolchain: gcc-arm-none-eabi v10.3.1
################################################################################


######################################
# Target Definitions
######################################
PROJ_NAME:=ford-avas-application


######################################
# Application Source Directories
######################################
SOURCE_DIR:=source/
SOURCE_DIR+=source/driver/
SOURCE_DIR+=source/driver/flash/
SOURCE_DIR+=source/driver/flash/spiflash/
SOURCE_DIR+=source/driver/amplifier/
SOURCE_DIR+=source/util/
SOURCE_DIR+=source/util/log/
SOURCE_DIR+=source/bsp/
SOURCE_DIR+=source/app/appman
SOURCE_DIR+=source/autosar/canif/private
SOURCE_DIR+=source/autosar/canif/public
SOURCE_DIR+=source/autosar/canif
SOURCE_DIR+=source/autosar/
SOURCE_DIR+=source/api/
SOURCE_DIR+=source/api/api_data
SOURCE_DIR+=source/app/app_main
SOURCE_DIR+=source/system
SOURCE_DIR+=source/app/app_avas
SOURCE_DIR+=source/api/api_audio


######################################
# SDK Source Directories
######################################
BASEDIR_SDK:=lib/nxp-sdk/
SOURCE_DIR_SDK:=platform/
SOURCE_DIR_SDK+=platform/drivers/src/
SOURCE_DIR_SDK+=platform/drivers/src/clock/
SOURCE_DIR_SDK+=platform/drivers/src/interrupt/
SOURCE_DIR_SDK+=platform/drivers/src/clock/S32K1xx/
SOURCE_DIR_SDK+=platform/devices/
SOURCE_DIR_SDK+=platform/devices/S32K116/
SOURCE_DIR_SDK+=platform/devices/S32K116/startup/
SOURCE_DIR_SDK+=platform/devices/S32K116/startup/gcc/
SOURCE_DIR_SDK+=platform/devices/S32K116/include/
SOURCE_DIR_SDK+=platform/devices/common
SOURCE_DIR_SDK+=platform/drivers/inc/
SOURCE_DIR_SDK+=platform/drivers/src/adc/
SOURCE_DIR_SDK+=platform/drivers/src/flash/
SOURCE_DIR_SDK+=platform/drivers/src/lpuart/
SOURCE_DIR_SDK+=platform/drivers/src/lpi2c/
SOURCE_DIR_SDK+=platform/drivers/src/pins/
SOURCE_DIR_SDK+=platform/drivers/src/edma/
SOURCE_DIR_SDK+=platform/drivers/src/lptmr/
SOURCE_DIR_SDK+=platform/drivers/src/flexio/
SOURCE_DIR_SDK+=platform/drivers/src/interrupt/
SOURCE_DIR_SDK+=platform/drivers/src/flexcan/
SOURCE_DIR_SDK+=platform/drivers/src/lpspi/
SOURCE_DIR_SDK+=platform/drivers/src/pdb/
SOURCE_DIR_SDK+=platform/drivers/src/wdog/
SOURCE_DIR_SDK+=platform/drivers/src/power/
SOURCE_DIR_SDK+=platform/drivers/src/power/S32K1xx
SOURCE_DIR_SDK+=rtos/osif/


######################################
# MCU Sources
######################################
C_SOURCES:=system_S32K116.c
C_SOURCES+=startup.c
C_SOURCES+=osif_baremetal.c
ASMSOURCES:=startup_S32K116.S

C_SOURCES+=adc_driver.c

C_SOURCES+=clock_S32K1xx.c

C_SOURCES+=edma_driver.c
C_SOURCES+=edma_hw_access.c
C_SOURCES+=edma_irq.c

C_SOURCES+=flash_driver.c

C_SOURCES+=flexcan_driver.c
C_SOURCES+=flexcan_hw_access.c
C_SOURCES+=flexcan_irq.c

C_SOURCES+=flexio_common.c
C_SOURCES+=flexio_i2c_driver.c
C_SOURCES+=flexio_i2s_driver.c
#C_SOURCES+=flexio_spi_driver.c
#C_SOURCES+=flexio_uart_driver.c

C_SOURCES+=interrupt_manager.c

C_SOURCES+=lpi2c_driver.c
C_SOURCES+=lpi2c_hw_access.c
C_SOURCES+=lpi2c_irq.c

C_SOURCES+=lpspi_master_driver.c
C_SOURCES+=lpspi_hw_access.c
C_SOURCES+=lpspi_irq.c
C_SOURCES+=lpspi_shared_function.c
C_SOURCES+=lpspi_slave_driver.c

C_SOURCES+=lptmr_driver.c
C_SOURCES+=lptmr_hw_access.c

C_SOURCES+=lpuart_driver.c
C_SOURCES+=lpuart_hw_access.c
C_SOURCES+=lpuart_irq.c

C_SOURCES+=pdb_driver.c
C_SOURCES+=pdb_hw_access.c

C_SOURCES+=pins_driver.c
C_SOURCES+=pins_port_hw_access.c

C_SOURCES+=power_manager_S32K1xx.c

C_SOURCES+=wdog_driver.c
C_SOURCES+=wdog_hw_access.c


######################################
# Application sources
######################################
C_SOURCES+=main.c
C_SOURCES+=log.c
C_SOURCES+=drv_amplifier.c
C_SOURCES+=drv_flash.c
C_SOURCES+=spiflash.c
C_SOURCES+=bsp_timer.c
C_SOURCES+=bsp_can.c
C_SOURCES+=bsp_i2c.c
C_SOURCES+=bsp_spi.c
C_SOURCES+=bsp_mcu.c
C_SOURCES+=bsp_uart.c
C_SOURCES+=bsp_isr.c
C_SOURCES+=bsp_wdg.c
C_SOURCES+=appman.c
C_SOURCES+=bsp_gpio.c
C_SOURCES+=canif.c
C_SOURCES+=canif_transceiver.c
C_SOURCES+=canif_cfg.c
C_SOURCES+=api_data.c
C_SOURCES+=system_config.c
C_SOURCES+=bsp_eeprom.c
C_SOURCES+=bsp_i2s.c
C_SOURCES+=bsp_dma.c
C_SOURCES+=app_main.c
C_SOURCES+=bsp_adc.c
C_SOURCES+=app_avas.c
C_SOURCES+=api_audio.c

######################################
# Test and Debug Only
######################################
 #SOURCE_DIR+=tools\AVAS_Audio_Calibration_Tool\Audio-File-Calibration-Tool-master\test-bin-files\sine_1000Hz_16KHz_32bitPCM_-6dBFS_stereo_01.1_short

 #C_SOURCES+=sine_1000Hz_16KHz_32bitPCM_-6dBFS_stereo_01.1_short.c



######################################
# SDK Sources
######################################
# C_SOURCES+=pins_driver.c


#######################################
# File Extensions
#######################################
ASM_EXT:=S


#######################################
# Paths
#######################################
BUILD_DIR:=build
OBJECT_DIR:=objects

ifeq ($(OS),Windows_NT) 
    GCC_PATH:=$(CURDIR)/ford-avas-toolchain/10.3.1/bin
else
    GCC_PATH = /usr/bin
endif



######################################
# Output Files
######################################
MAP_FILE:=$(BUILD_DIR)/$(PROJ_NAME).map
ELF_FILE:=$(BUILD_DIR)/$(PROJ_NAME).elf
BIN_FILE:=$(BUILD_DIR)/$(PROJ_NAME).bin
HEX_FILE:=$(BUILD_DIR)/$(PROJ_NAME).hex


#######################################
# Toolchain Executables
#######################################
CC:=$(GCC_PATH)/arm-none-eabi-gcc
AS:=$(GCC_PATH)/arm-none-eabi-gcc
LD:=$(GCC_PATH)/arm-none-eabi-gcc
CP:=$(GCC_PATH)/arm-none-eabi-objcopy
CCSIZE:=$(GCC_PATH)/arm-none-eabi-size
RM:=rm -f


#######################################
# Compiler Options
#######################################
CFLAGS:=-mcpu=cortex-m0plus -mthumb
CFLAGS+=-Wall -Wextra -Wstrict-prototypes -Wno-pedantic -Wunused -Wsign-compare -Wconversion -Wno-missing-field-initializers -Wno-missing-braces
CFLAGS+=-O1
CFLAGS+=-funsigned-char -funsigned-bitfields  -fshort-enums
CFLAGS+=-ffunction-sections -fdata-sections -fno-jump-tables
CFLAGS+=-std=c99
CFLAGS+=-g
CFLAGS+=-DCPU_S32K116
CFLAGS+=-DDEV_ERROR_DETECT
CFLAGS+=-mfloat-abi=soft


#######################################
# Assembler Options
#######################################
ASFLAGS:=-mcpu=cortex-m0plus -mthumb
ASFLAGS+=-Wall -Wextra -Wstrict-prototypes -pedantic -Werror
ASFLAGS+=-x assembler-with-cpp -DSTART_FROM_FLASH
ASFLAGS+=-mfloat-abi=soft


#######################################
# Linker Options
#######################################
LDFLAGS:=-mcpu=cortex-m0plus -mthumb
LDFLAGS+=-Wl,-Map=$(MAP_FILE)
LDFLAGS+=-T "$(BASEDIR_SDK)/platform/devices/S32K116/linker/gcc/S32K116_17_flash.ld"
LDFLAGS+=-nostartfiles -nodefaultlibs -nostdlib --entry=Reset_Handler -Wl,-gc-sections
LDFLAGS+=-lgcc -lm -lc -specs=nano.specs


#######################################
# Includes
#######################################
include Makefile-arm
 