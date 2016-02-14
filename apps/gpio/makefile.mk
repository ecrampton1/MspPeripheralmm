TARGET:=gpio

#mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))

#mkfile_path := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

 #name of the target binary
#will pass this into the makefile
BUILD_SOURCES := apps/gpio/main.cpp
BUILD_INC := apps/gpio
OBJ_DIR = apps/gpio/obj

