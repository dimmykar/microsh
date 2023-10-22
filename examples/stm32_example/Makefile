################################################################################
#
# STM32 Example Makefile
# Toolchain: GNU GCC ARM Embedded Tools
#
# Copyright (c) 2022 Dmitry KARASEV
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# This file is part of MicroSH - Shell for Embedded Systems library.
#
# Author:          Dmitry KARASEV <karasevsdmitry@yandex.ru>
# Version:         1.3.1
#
################################################################################

# ------------------------------------------------------------------------------
# Target
# ------------------------------------------------------------------------------
TARGET       = stm32_example
ARCH         = cortex-m4


# ------------------------------------------------------------------------------
# Toolchain
# ------------------------------------------------------------------------------
TC_PREFIX    = arm-none-eabi-
CC           = $(TC_PREFIX)gcc
AS           = $(TC_PREFIX)gcc -x assembler-with-cpp
CP           = $(TC_PREFIX)objcopy
AR           = $(TC_PREFIX)ar
SZ           = $(TC_PREFIX)size
HEX          = $(CP) -O ihex
BIN          = $(CP) -O binary -S


# ------------------------------------------------------------------------------
# Paths
# ------------------------------------------------------------------------------
# Firmware sources path
STM32_SRC_DIR  = src

# MicroSH sources path
MSH_SRC_DIR    = ../../microsh/src/microsh

# MicroSH includes path
MSH_INC_DIR    = ../../microsh/src/include/microsh

# HAL libraries path
HAL_DIR        = $(STM32_SRC_DIR)/st/stm32f4xx

# Third party libraries path
THIRDLIB_DIR   = ../../3rdparty

# Build path
BUILD_DIR      = build


# ------------------------------------------------------------------------------
# Sources
# ------------------------------------------------------------------------------
# Generic C sources
FIRMWARE_SOURCES = \
	../example.c \
	$(STM32_SRC_DIR)/stm32_misc/stm32_misc.c \
	$(MSH_SRC_DIR)/microsh.c

# BSP library sources
BSP_SOURCES = \
	$(HAL_DIR)/stm32f401/cmsis/system_stm32f4xx.c \
	$(HAL_DIR)/ll/stm32f4xx_ll_gpio.c \
	$(HAL_DIR)/ll/stm32f4xx_ll_rcc.c \
	$(HAL_DIR)/ll/stm32f4xx_ll_usart.c

# Third party libraries sources
THIRDLIB_SOURCES = \
	$(THIRDLIB_DIR)/microrl-remaster/src/microrl/microrl.c

# C sources
C_SOURCES = \
	$(FIRMWARE_SOURCES) \
	$(BSP_SOURCES) \
	$(THIRDLIB_SOURCES)


# ASM sources
ASM_SOURCES = \
	$(HAL_DIR)/stm32f401/startup_stm32f401vctx.s


# ------------------------------------------------------------------------------
# Building variables
# ------------------------------------------------------------------------------
# Debugging level: None, all symbols deleted
DBG_LEVEL    = -g0

# Optimization: Optimize for size
OPT          = -O3


# CFLAGS
# CPU
CPU          = -mcpu=$(ARCH)

#Instruction set: Thumb2
INSTR_SET    = -mthumb

# FPU: Single precision
FPU          = -mfpu=fpv4-sp-d16
FLOAT-ABI    = -mfloat-abi=hard

# MCU
MCU          = $(CPU) $(INSTR_SET) $(FPU) $(FLOAT-ABI)

# C standard
STDC         = -std=c99

# GCC Preprocessor
# AS defines
AS_DEFS =

# C defines
C_DEFS = \
	-DSTM32F401xC \
	-DUSE_FULL_LL_DRIVER

#Includes
# AS includes
AS_INCLUDES =

# C includes
C_INCLUDES = \
	-I../ \
	-I$(MSH_INC_DIR) \
	-I$(THIRDLIB_DIR)/microrl-remaster/src/include/microrl \
	-I$(HAL_DIR)/ll \
	-I$(HAL_DIR)/stm32f401 \
	-I$(HAL_DIR)/stm32f401/cmsis


# Compile GCC flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) $(DBG_LEVEL) -Wall -fdata-sections -ffunction-sections

CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) $(DBG_LEVEL) $(STDC) -Wall -fdata-sections -ffunction-sections


# LDFLAGS
# Linker script
LDSCRIPT     = $(HAL_DIR)/stm32f401/ld/STM32F401VCTX_FLASH.ld

# Libraries
# C Runtime Library: reduced C library 'newlib-nano'
STDC_LIB     = --specs=nano.specs

# System calls
STDC_SC      = -lnosys

# Use STD math library
STDMATH_LIB  = -lm

# Libraries flags
LIBS         = $(STDC_LIB) $(STDC_SC) $(STDMATH_LIB)


# Linker GCC flags
LDFLAGS = $(MCU) -T$(LDSCRIPT) $(LIB_DIR) $(LIBS) -static -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections


# ------------------------------------------------------------------------------
# Build the application
# ------------------------------------------------------------------------------
# Default action: Build all Target
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin


# List of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# List of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))


# Tool invocations
$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@

$(BUILD_DIR):
	mkdir $@


# ------------------------------------------------------------------------------
# Cleanup
# ------------------------------------------------------------------------------
# Clean Target
clean:
	-rm -fR $(BUILD_DIR)


# *** EOF ***
