# This file is part of 34S.
# 
# 34S is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# 34S is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with 34S.  If not, see <http://www.gnu.org/licenses/>.

######################################
# Target
######################################

#TARGET = wp34s

VERSION_NO != git rev-list HEAD --count
export VERSION_NO

######################################
# building variables
######################################
# debug build?
ifdef DEBUG
DEBUG = 1
endif

#######################################
# paths
#######################################
# Build path
SYSTEM := Linux64
OBJECTDIR := build
BUILD_DIR := build

EXE :=


# Path to aux build scripts (including trailing /)
# Leave empty for scripts in PATH
BIN_DIR = bin/

######################################
# System sources
######################################
C_INCLUDES += -Idmcp
C_SOURCES += dmcp/sys/pgm_syscalls.c
ASM_SOURCES = dmcp/startup_pgm.s

#######################################
# Custom section
#######################################

# Includes
C_INCLUDES += -Isrc -Iinc

# C sources

SRCS := keys.c display.c xeq.c prt.c decn.c complex.c stats.c \
		lcd.c int.c date.c consts.c alpha.c charmap.c \
		commands.c string.c storage.c matrix.c \
		font.c xrom.c console.c \
		stopwatch.c \
		printer.c
C_SRCS := $(addprefix src/,$(SRCS))

C_SOURCES += $(C_SRCS)

HEADERS := alpha.h charset7.h complex.h consts.h data.h \
		date.h decn.h display.h features.h int.h keys.h lcd.h \
		stats.h xeq.h xrom.h storage.h matrix.h menu.h menu.c keytran.c \
		main.h \
		stopwatch.h \
		printer.h

# Libraries
#ifeq ($(DEBUG), 1)
#LIBS += lib/gcc111libbid_hard.a
#else
#LIBS += lib/gcc111libbid_hard.a
#endif

# ---


# For decnumber library

DECNUMDIR := src/decNumber
DECNUM_OBJECTDIR := $(DECNUMDIR)/obj_DM42


DECLIB = libdecNumber.a

DECINCS  = decContext.h decDPD.h decNumber.h decPacked.h
DECINCS += decimal128.h decimal32.h decimal64.h
DECINCS := $(addprefix $(DECNUMDIR)/,$(DECINCS))
DECINCS += src/features.h

DECSRCS  = decContext.c decNumber.c decPacked.c
DECSRCS += decimal128.c decimal64.c
DECOBJS = $(DECSRCS:%.c=$(DECNUM_OBJECTDIR)/%.o)

# Consts stuff

CONST_DIR = src/Linux64/consts
CONST_OBJECTDIR = CONST_DIR

CONST_CFILES := $(wildcard $(CONST_DIR)/const_*.c)
CONST_OBJS := $(CONST_CFILES:%.c=%.o)


#######################################
# binaries
#######################################
CC = arm-none-eabi-gcc
CXX = arm-none-eabi-g++
AS = arm-none-eabi-gcc -x assembler-with-cpp
OBJCOPY = arm-none-eabi-objcopy
AR = arm-none-eabi-ar
SIZE = arm-none-eabi-size
HEX = $(OBJCOPY) -O ihex
BIN = $(OBJCOPY) -O binary -S

#######################################
# CFLAGS
#######################################
# macros for gcc

AS_DEFS =
C_DEFS += -D__weak="__attribute__((weak))" -D__packed="__attribute__((__packed__))"
AS_INCLUDES =
CPUFLAGS += -mthumb -march=armv7e-m -mfloat-abi=hard -mfpu=fpv4-sp-d16


# compile gcc flags
ASFLAGS = $(CPUFLAGS) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections
CFLAGS  = $(CPUFLAGS) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections
CFLAGS += -Wno-misleading-indentation
CFLAGS += -DDM42=1 -DC_VERSION=1
DBGFLAGS = -g 

ifeq ($(DEBUG), 1)
CFLAGS += -O0 -DDEBUG
else
CFLAGS += -O2
endif

CFLAGS  += $(DBGFLAGS)
LDFLAGS += $(DBGFLAGS)

# Generate dependency information
CFLAGS += -MD -MP -MF .dep/$(@F).d

#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT = stm32_program.ld
LIBDIR =

all :	top

top :	top_setup clean_some $(BUILD_DIR)/wp34c_top.elf

top_setup:
	$(eval TARGET = wp34c_top)
	$(eval LDFLAGS = --specs=nosys.specs $(CPUFLAGS) -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections -Wl,--wrap=_malloc_r)
	$(shell cat src/main_0.h | sed -e 's/pversion/$(VERSION_NO)/g' -e 's/pname/$(TARGET)/g' > src/main.h)
	$(eval CFLAGS += -DBIGGER_DISPLAY -DTOP_ROW)

normal: normal_setup clean_some $(BUILD_DIR)/wp34c.elf 

normal_setup:
	$(eval TARGET = wp34c)
	$(eval LDFLAGS = --specs=nosys.specs $(CPUFLAGS) -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections -Wl,--wrap=_malloc_r)
	$(shell cat src/main_0.h | sed -e 's/pversion/$(VERSION_NO)/g' -e 's/pname/$(TARGET)/g' > src/main.h)

long :	long_setup clean_some $(BUILD_DIR)/wp34c_long.elf

long_setup:
	$(eval TARGET = wp34c_long)
	$(eval LDFLAGS = --specs=nosys.specs $(CPUFLAGS) -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections -Wl,--wrap=_malloc_r)
	$(shell cat src/main_0.h | sed -e 's/pversion/$(VERSION_NO)/g' -e 's/pname/$(TARGET)/g' > src/main.h)
	$(eval CFLAGS += -DBIGGER_DISPLAY)

#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
OBJECTS += $(BUILD_DIR)/libconsts.a
OBJECTS += $(BUILD_DIR)/libdecNumber.a

# C++ sources
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(CXX_SOURCES:.cc=.o)))
vpath %.cc $(sort $(dir $(CXX_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti

# Build libdecNumber.a:

$(DECNUM_OBJECTDIR):
	mkdir -p $@

$(DECNUM_OBJECTDIR)/%.o: $(DECNUMDIR)/%.c $(DECINCS) Makefile
	$(CC) -c $(CFLAGS) -o $@ $<

$(BUILD_DIR)/$(DECLIB): $(DECOBJS) Makefile
	-rm -f $@
	$(AR) -r $@ $(DECOBJS)
	$(AR) -s $@

# Build libconsts.a:

$(BUILD_DIR)/libconsts.a: $(CONST_OBJS) Makefile
	-rm -f $@
	$(AR) -r $@ $(CONST_OBJS)
	$(AR) -s $@


$(CONST_OBJECTDIR)/%.o: $(CONST_DIR)/%.c Makefile
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(CONST_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

# And the rest ...

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.cc Makefile | $(BUILD_DIR) 
	$(CXX) -c $(CXXFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.cc=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/wp34c.elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(OBJCOPY) --remove-section .qspi -O ihex    $@  $(BUILD_DIR)/$(TARGET)_flash.hex
	$(OBJCOPY) --remove-section .qspi -O binary  $@  $(BUILD_DIR)/$(TARGET)_flash.bin
	$(OBJCOPY) --only-section   .qspi -O ihex    $@  $(BUILD_DIR)/$(TARGET)_qspi.hex
	$(OBJCOPY) --only-section   .qspi -O binary  $@  $(BUILD_DIR)/$(TARGET)_qspi.bin
	$(BIN_DIR)check_qspi_crc $(TARGET) src/qspi_crc.h || ( $(MAKE) clean && false )
	$(BIN_DIR)add_pgm_chsum build/$(TARGET)_flash.bin build/$(TARGET).pgm
	$(SIZE) $@

$(BUILD_DIR)/wp34c_long.elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(OBJCOPY) --remove-section .qspi -O ihex    $@  $(BUILD_DIR)/$(TARGET)_flash.hex
	$(OBJCOPY) --remove-section .qspi -O binary  $@  $(BUILD_DIR)/$(TARGET)_flash.bin
	$(OBJCOPY) --only-section   .qspi -O ihex    $@  $(BUILD_DIR)/$(TARGET)_qspi.hex
	$(OBJCOPY) --only-section   .qspi -O binary  $@  $(BUILD_DIR)/$(TARGET)_qspi.bin
	$(BIN_DIR)check_qspi_crc $(TARGET) src/qspi_crc.h || ( $(MAKE) clean && false )
	$(BIN_DIR)add_pgm_chsum build/$(TARGET)_flash.bin build/$(TARGET).pgm
	$(SIZE) $@

$(BUILD_DIR)/wp34c_top.elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(OBJCOPY) --remove-section .qspi -O ihex    $@  $(BUILD_DIR)/$(TARGET)_flash.hex
	$(OBJCOPY) --remove-section .qspi -O binary  $@  $(BUILD_DIR)/$(TARGET)_flash.bin
	$(OBJCOPY) --only-section   .qspi -O ihex    $@  $(BUILD_DIR)/$(TARGET)_qspi.hex
	$(OBJCOPY) --only-section   .qspi -O binary  $@  $(BUILD_DIR)/$(TARGET)_qspi.bin
	$(BIN_DIR)check_qspi_crc $(TARGET) src/qspi_crc.h || ( $(MAKE) clean && false )
	$(BIN_DIR)add_pgm_chsum build/$(TARGET)_flash.bin build/$(TARGET).pgm
	$(SIZE) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@

$(BUILD_DIR):
	mkdir -p $@

#######################################
# clean up
#######################################
clean: cleantop cleanlibs 

cleantop:
	-rm -fR .dep $(BUILD_DIR)/*.o $(BUILD_DIR)/*.lst
cleanlibs:
	-rm -rf $(DECOBJS)
	-rm $(BUILD_DIR)/$(DECLIB)
	-rm -fR .dep $(CONST_DIR)/*.o $(CONST_DIR)/*.lst
	-rm $(BUILD_DIR)/libconsts.a
clean_some:
	-touch src/keys.c
	-touch src/console.c
	-touch src/lcd.c
	-touch src/display.c

#######################################
# dependencies
#######################################
-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)

source:
		$(MAKE) -C src

.PHONY: clean cleantop cleanlibs clean_some all normal long top source

# *** EOF ***
