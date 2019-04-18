PROJECT_NAME      := nrf52832_template
SDK_PATH          := ../nrf5_sdk
PROJECT_PATH      := .
SRC_PATH          := $(PROJECT_PATH)/src
OUTPUT_FILENAME   := nrf52832_template
LINKER_SCRIPT     := generic_gcc_nrf52.ld
GNU_PREFIX        := arm-none-eabi
SDK_CONFIG_FILE   := $(SRC_PATH)/sdk_config.h
CMSIS_CONFIG_TOOL := $(SDK_PATH)/external_tools/cmsisconfig/CMSIS_Configuration_Wizard.jar

export OUTPUT_FILENAME
MAKEFILE_NAME := $(MAKEFILE_LIST)
MAKEFILE_PATH  := $(subst Makefile,,$(abspath $(lastword $(MAKEFILE_LIST))))

# Allow verbose output
ifeq ("$(VERBOSE)","1")
    NO_ECHO :=
else
    NO_ECHO := @
endif

# Define platform-specific details for the build
ifeq ($(OS),Windows_NT)
    GNU_INSTALL_ROOT := C:/Glue/CI/GNU Tools ARM Embedded/7 2017-q4-major
    GNU_VERSION      := 7.2.1
    MK               := md
    RM               := rd /s /q
else
    TOOLS_PATH       := ../../../firmware-tools
    GNU_INSTALL_ROOT := $(TOOLS_PATH)/gcc-arm-none-eabi
    GNU_VERSION      := 7.2.1
    MK               := mkdir
    RM               := rm -rf
endif

# Toolchain commands
AR      := '$(abspath $(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-ar)' -r
AS      := '$(abspath $(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-as)'
CC      := '$(abspath $(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-gcc)'
LD      := '$(abspath $(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-gcc)'
NM      := '$(abspath $(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-nm)'
OBJCOPY := '$(abspath $(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-objcopy)'
OBJDUMP := '$(abspath $(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-objdump)'
SIZE    := '$(abspath $(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-size)'

# Function for removing duplicates in a list
remduplicates = $(strip $(if $1,$(firstword $1) $(call remduplicates,$(filter-out $(firstword $1),$1))))

# C source files
C_SOURCE_FILES += \
		$(SDK_PATH)/external/fprintf/nrf_fprintf_format.c \
		$(SDK_PATH)/external/fprintf/nrf_fprintf.c \
		$(SDK_PATH)/external/segger_rtt/SEGGER_RTT_printf.c \
		$(SDK_PATH)/external/segger_rtt/SEGGER_RTT_Syscalls_GCC.c \
		$(SDK_PATH)/external/segger_rtt/SEGGER_RTT.c \
		$(SDK_PATH)/modules/nrfx/mdk/system_nrf52.c \

C_SOURCE_FILES += \
        $(TOOLS_PATH)/jlink_monitor_mode_debug/JLINK_MONITOR.c \

C_SOURCE_FILES += \
		$(SRC_PATH)/main.c \

# Remove unwanted source files
# C_SOURCE_FILES := $(filter-out $(SDK_PATH)/external/mbedtls/library/asn1write.c, $(C_SOURCE_FILES))
# C_SOURCE_FILES := $(filter-out $(SDK_PATH)/external/mbedtls/library/ssl_srv.c, $(C_SOURCE_FILES))

# Assembly files common to all targets
ASM_SOURCE_FILES += \
        $(SDK_PATH)/modules/nrfx/mdk/gcc_startup_nrf52.S \
        $(TOOLS_PATH)/jlink_monitor_mode_debug/JLINK_MONITOR_ISR_SES.S \

# Includes paths common to all targets
INC_PATHS += \
	-I$(SDK_PATH)/components/libraries/util \
	-I$(SDK_PATH)/components \
	-I$(SDK_PATH)/components/drivers_nrf/nrf_soc_nosd \
	-I$(SDK_PATH)/components/toolchain/cmsis/include \
	-I$(SDK_PATH)/modules/nrfx \
	-I$(SDK_PATH)/modules/nrfx/drivers/include \
	-I$(SDK_PATH)/modules/nrfx/hal \
	-I$(SDK_PATH)/modules/nrfx/mdk \
	-I$(SRC_PATH) \

OBJECT_DIRECTORY = build
LISTING_DIRECTORY = $(OBJECT_DIRECTORY)
OUTPUT_BINARY_DIRECTORY = $(OBJECT_DIRECTORY)

# Sorting removes duplicates
BUILD_DIRECTORIES := $(sort $(OBJECT_DIRECTORY) $(OUTPUT_BINARY_DIRECTORY) $(LISTING_DIRECTORY) )

C_SOURCE_FILE_NAMES = $(notdir $(C_SOURCE_FILES))
C_PATHS = $(call remduplicates, $(dir $(C_SOURCE_FILES) ) )
C_OBJECTS = $(addprefix $(OBJECT_DIRECTORY)/, $(C_SOURCE_FILE_NAMES:.c=.o) )

ASM_SOURCE_FILE_NAMES = $(notdir $(ASM_SOURCE_FILES))
ASM_PATHS = $(call remduplicates, $(dir $(ASM_SOURCE_FILES) ))
ASM_OBJECTS = $(addprefix $(OBJECT_DIRECTORY)/, $(ASM_SOURCE_FILE_NAMES:.S=.o) )

vpath %.c $(C_PATHS)
vpath %.S $(ASM_PATHS)

OBJECTS = $(C_OBJECTS) $(ASM_OBJECTS)

# Compiler flags
CFLAGS += -D__HEAP_SIZE=8192
CFLAGS += -D__STACK_SIZE=8192
CFLAGS += -DBL_SETTINGS_ACCESS_ONLY
CFLAGS += -DBOARD_PCA10040
CFLAGS += -DCONFIG_GPIO_AS_PINRESET
CFLAGS += -DFLOAT_ABI_SOFT
# CFLAGS += -DMBEDTLS_CONFIG_FILE=\"$(MAKEFILE_PATH)src/mbedtls_config.h\"
CFLAGS += -DNRF_CRYPTO_MAX_INSTANCE_COUNT=1
CFLAGS += -DNRF_DFU_SVCI_ENABLED
CFLAGS += -DNRF_DFU_TRANSPORT_BLE=1
CFLAGS += -DNRF_SD_BLE_API_VERSION=6
CFLAGS += -DNRF52
CFLAGS += -DNRF52_PAN_74
CFLAGS += -DNRF52832_XXAA
CFLAGS += -DPB_FIELD_16BIT
CFLAGS += -DS132
CFLAGS += -DSOFTDEVICE_PRESENT
CFLAGS += -DSWI_DISABLE0
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -fno-builtin -fshort-enums
CFLAGS += -mcpu=cortex-m4
CFLAGS += -mfloat-abi=soft
CFLAGS += -mthumb -mabi=aapcs
CFLAGS += -Wall -Wno-unused-variable

# Linker flags
LDFLAGS += --specs=nano.specs
LDFLAGS += -L$(LIBSODIUM_PATH)/lib/cortexm
LDFLAGS += -mcpu=cortex-m4
LDFLAGS += -mfloat-abi=soft
LDFLAGS += -mthumb -mabi=aapcs -L$(SDK_PATH)/modules/nrfx/mdk -T$(LINKER_SCRIPT)
LDFLAGS += -Wl,--gc-sections
LDFLAGS += -Xlinker -Map=$(LISTING_DIRECTORY)/$(OUTPUT_FILENAME).map

# Assembler flags
ASMFLAGS += -D__HEAP_SIZE=8192
ASMFLAGS += -D__STACK_SIZE=8192
ASMFLAGS += -DBL_SETTINGS_ACCESS_ONLY
ASMFLAGS += -DBOARD_PCA10040
ASMFLAGS += -DCONFIG_GPIO_AS_PINRESET
ASMFLAGS += -DFLOAT_ABI_SOFT
ASMFLAGS += -DNRF_CRYPTO_MAX_INSTANCE_COUNT=1
ASMFLAGS += -DNRF_DFU_SVCI_ENABLED
ASMFLAGS += -DNRF_DFU_TRANSPORT_BLE=1
ASMFLAGS += -DNRF_SD_BLE_API_VERSION=6
ASMFLAGS += -DNRF52
ASMFLAGS += -DNRF52_PAN_74
ASMFLAGS += -DNRF52832_XXAA
ASMFLAGS += -DS132
ASMFLAGS += -DSOFTDEVICE_PRESENT
ASMFLAGS += -DSWI_DISABLE0
ASMFLAGS += -g3
ASMFLAGS += -mcpu=cortex-m4
ASMFLAGS += -mfloat-abi=soft
ASMFLAGS += -mthumb -mabi=aapcs

# RULES ***********************************************************************/

# Default build target
.PHONY: all
all: debug

# Clean
.PHONY clean:
ifneq ($(wildcard $(BUILD_DIRECTORIES)),)
clean: 
	$(RM) $(BUILD_DIRECTORIES)
endif

# Debug, defaulting to monitor mode
.PHONY: debug
debug: clean
debug: ASMFLAGS += -DDEBUG -g2 -O0
debug: CFLAGS += -DDEBUG -g2 -O0
debug: CFLAGS += -DDEBUG_NRF=1
debug: CFLAGS += -DNRF_LOG_USES_RTT=1
debug: CFLAGS += -DMMD=1
debug: CFLAGS += -DDEBUG_MONITOR_MODE=1
debug: $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out

# Create build directories
$(BUILD_DIRECTORIES):
	$(MK) $@

# Compile C files
$(OBJECT_DIRECTORY)/%.o: %.c
	@echo COMPILING: $(notdir $<)
	$(NO_ECHO)$(CC) $(CFLAGS) $(INC_PATHS) -c -o $@ $<

# Assemble
$(OBJECT_DIRECTORY)/%.o: %.S
	@echo ASSEMBLING: $(notdir $<)
	$(NO_ECHO)$(CC) $(ASMFLAGS) $(INC_PATHS) -c -o $@ $<

# Link
$(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out: $(BUILD_DIRECTORIES) $(OBJECTS)
	@echo LINKING: $(OUTPUT_FILENAME).out
	$(NO_ECHO)$(LD) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out
	$(NO_ECHO)$(MAKE) -f $(MAKEFILE_NAME) -C $(MAKEFILE_PATH) -e finalise

# Finalise the build
finalise: genbin genhex echosize

# Create .bin file from .out file
genbin:
	@echo GENERATING IMAGE: $(OUTPUT_FILENAME).bin
	$(NO_ECHO)$(OBJCOPY) -O binary $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).bin

# Create .hex file from .out file
genhex:
	@echo GENERATING IMAGE: $(OUTPUT_FILENAME).hex
	$(NO_ECHO)$(OBJCOPY) -O ihex $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).hex

# Print size
echosize:
	-@echo ''
	$(NO_ECHO)$(SIZE) $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out
	-@echo ''

# Run the SDK configuration tool
sdk_config:
	java -jar $(CMSIS_CONFIG_TOOL) $(SDK_CONFIG_FILE)
