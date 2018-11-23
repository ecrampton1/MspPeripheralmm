SHELL := /bin/bash

# These tools are the host machine's arm cortex a8-target compilers
# They run on HOST and produce binaries that run on arm cortex a8
# The user should take care of putting the tools in the path...
TOOLS_PREFIX := msp430-elf-
#TOOLS_PATH   := $(dir $(shell which $(TOOLS_PREFIX)g++))
TOOLS_PATH   := /usr/local/msp430/bin/
#TOOLS_PATH   := /home/eddie/gcc_msp430_build_2/msp430-gcc-7.3.1.24-source-patches/install/usr/local/bin/


# Compiler/linker flags to sepcify CPU
MMCU := msp430g2553
CPU_FLAGS   := -mmcu=$(MMCU) -Os -Wall -s -Wno-main -pedantic -Wreturn-type -Wunused -fdata-sections -ffunction-sections -fwrapv -fomit-frame-pointer -fno-exceptions 
#CPU_FLAGS   := -mmcu=$(MMCU) -Wall -minrt -pedantic -Wreturn-type -Wunused -g -MMD -fwrapv -fomit-frame-pointer -fno-exceptions


LDSCRIPTS   := $(TOOLS_PATH)../msp430-elf/lib/$(MMCU).ld
#SYSINCLUDE  := $(TOOLS_PATH)/include

CXXFLAGS    := -std=c++11 $(CPU_FLAGS) -lstdc++
CFLAGS      := $(CPU_FLAGS) -lc
ASFLAGS     := -mmcu=$(MMCU) --x assembler-with-cpp -Wa
LDFLAGS     := $(addprefix -T ,$(LDSCRIPTS))
LDFLAGS     += -mmcu=$(MMCU) -Wl,--gc-sections  -lstdc++
#LDFLAGS     += -Wl,--gc-sections


CXX         := $(TOOLS_PATH)$(TOOLS_PREFIX)g++ 
CC          := $(TOOLS_PATH)$(TOOLS_PREFIX)gcc 
CPP         := $(TOOLS_PATH)$(TOOLS_PREFIX)cpp 
ASM         := $(TOOLS_PATH)$(TOOLS_PREFIX)as
AR          := $(TOOLS_PATH)$(TOOLS_PREFIX)ar
LD          := $(TOOLS_PATH)$(TOOLS_PREFIX)ld
STRIP       := $(TOOLS_PATH)$(TOOLS_PREFIX)strip
SIZE       := $(TOOLS_PATH)$(TOOLS_PREFIX)size

# Don't bother with the rest if the tools aren't present
ifneq ($(shell which $(CC)),)
ARCHLONG    := msp430-elf-$(shell $(CXX) -dumpmachine)
ARCH        := msp430


# Add this architecture to the master list
ALL_ARCHES += $(ARCH)

endif

