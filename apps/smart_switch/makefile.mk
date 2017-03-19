#name of the target binary
TARGET:=smart_switch

mkfile_path := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

#will return these to the calling makefile
APP_FLAGS := -DSMALL_MEMORY -DPORT2_P3_ENABLE_INT -DPORT1_P3_ENABLE_INT -DPORT1_P4_ENABLE_INT -DTIMER0_ENABLE_INT
BUILD_SOURCES := $(mkfile_path)/main.cpp
BUILD_SOURCES += external_libraries/RF24Network/RF24Network.cpp
BUILD_SOURCES += external_libraries/RF24Mesh/RF24Mesh.cpp
BUILD_SOURCES += external_libraries/PeripheralMessaging/src/rf24_message_handler.cpp
BUILD_SOURCES += external_libraries/PeripheralMessaging/src/message_handler.cpp
BUILD_INC := $(mkfile_path) external_libraries/RF24Mesh/ external_libraries/RF24Network/ external_libraries/PeripheralMessaging/inc
BUILD_SOURCE_DIRS := $(mkfile_path) external_libraries/RF24Network/ external_libraries/RF24Mesh/ external_libraries/PeripheralMessaging/src
OBJ_DIR = $(mkfile_path)/obj

