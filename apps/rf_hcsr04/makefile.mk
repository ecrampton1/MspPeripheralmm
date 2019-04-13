#name of the target binary
TARGET:=rf_hcsr04

mkfile_path := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

#will return these to the calling makefile
APP_FLAGS := -DSMALL_MEMORY -DTIMER1_ENABLE_INT 
BUILD_SOURCES := $(mkfile_path)/main.cpp
BUILD_SOURCES += external_libraries/PeripheralMessaging/src/message_handler.cpp
BUILD_SOURCES += external_libraries/PeripheralMessaging/src/rfm69_message_handler.cpp
BUILD_INC := $(mkfile_path) external_libraries/PeripheralMessaging/inc external_libraries/RFM69/inc
BUILD_SOURCE_DIRS := $(mkfile_path) external_libraries/PeripheralMessaging/src
OBJ_DIR = $(mkfile_path)/obj

