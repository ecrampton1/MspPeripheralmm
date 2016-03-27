#name of the target binary
TARGET:=uart


#will return these to the calling makefile
APP_FLAGS := -DUARTA0_ENABLE_INT
BUILD_SOURCES := apps/$(TARGET)/main.cpp
BUILD_INC := apps/$(TARGET)
OBJ_DIR = apps/$(TARGET)/obj

