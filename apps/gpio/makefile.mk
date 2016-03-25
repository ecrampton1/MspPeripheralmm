TARGET:=gpio

#mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))

#mkfile_path := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

 #name of the target binary
#will pass this into the makefile
#APP_FLAGS := -DUARTA0_ENABLE_INT
BUILD_SOURCES := apps/gpio/main.cpp
BUILD_INC := apps/gpio
OBJ_DIR = apps/gpio/obj

