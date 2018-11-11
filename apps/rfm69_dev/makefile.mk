#name of the target binary
TARGET:=rfm69_dev

mkfile_path := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

#will return these to the calling makefile
APP_FLAGS := -DSMALL_MEMORY -DPORT1_P4_ENABLE_INT
BUILD_SOURCES := $(mkfile_path)/main.cpp
BUILD_INC := $(mkfile_path) 
BUILD_SOURCE_DIRS := $(mkfile_path)
OBJ_DIR = $(mkfile_path)/obj

