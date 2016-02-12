SHELL := /bin/bash

# These tools are the host machine's arm cortex a8-target compilers
# They run on HOST and produce binaries that run on arm cortex a8
# The user should take care of putting the tools in the path...
TOOLS_PREFIX := msp430-elf-
TOOLS_PATH   := $(dir $(shell which $(TOOLS_PREFIX)g++))

# Compiler/linker flags to sepcify CPU
MMCU := msp430g2553
CPU_FLAGS   := -mmcu=$(MMCU) -Os -Wall -minrt -pedantic -Wreturn-type -Wunused -s -fdata-sections -ffunction-sections -g -MMD -fwrapv -fomit-frame-pointer -fno-exceptions 

LDSCRIPTS   := $(TOOLS_PATH)../msp430-elf/include/$(MMCU).ld
#SYSINCLUDE  := $(TOOLS_PATH)/include

CXXFLAGS    := -std=c++11 $(CPU_FLAGS) 
CFLAGS      := $(CPU_FLAGS) 
ASFLAGS     := -mmcu=$(MMCU) --x assembler-with-cpp -Wa
LDFLAGS     := $(addprefix -T ,$(LDSCRIPTS))
LDFLAGS     += -mmcu=$(MMCU) -minrt -Wl,--gc-sections,-Map=$(TARGET).map 
#LDFLAGS     += -Wl,--gc-sections


CXX         := $(TOOLS_PREFIX)g++ 
CC          := $(TOOLS_PREFIX)gcc 
CPP         := $(TOOLS_PREFIX)cpp 
ASM         := $(TOOLS_PREFIX)as
AR          := $(TOOLS_PREFIX)ar
LD          := $(TOOLS_PREFIX)ld
STRIP       := $(TOOLS_PREFIX)strip
SIZE       := $(TOOLS_PREFIX)size

# Don't bother with the rest if the tools aren't present
ifneq ($(shell which $(CC)),)
ARCHLONG    := msp430-elf-$(shell $(CXX) -dumpmachine)
ARCH        := msp430


# Add this architecture to the master list
ALL_ARCHES += $(ARCH)

endif

