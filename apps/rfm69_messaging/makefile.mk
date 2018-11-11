#name of the target binary
TARGET:=rfm69_messaging

mkfile_path := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

#will return these to the calling makefile
APP_FLAGS := -DSMALL_MEMORY
BUILD_SOURCES := $(mkfile_path)/main.cpp
BUILD_SOURCES += external_libraries/PeripheralMessaging/src/message_handler.cpp
BUILD_SOURCES += external_libraries/PeripheralMessaging/src/rfm69_message_handler.cpp
BUILD_INC := $(mkfile_path) external_libraries/PeripheralMessaging/inc
BUILD_SOURCE_DIRS := $(mkfile_path) external_libraries/PeripheralMessaging/src
OBJ_DIR = $(mkfile_path)/obj

