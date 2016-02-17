#name of the target binary
TARGET:=uart


#will return these to the calling makefile
BUILD_SOURCES := apps/$(TARGET)/main.cpp
BUILD_INC := apps/$(TARGET)
OBJ_DIR = apps/$(TARGET)/obj

