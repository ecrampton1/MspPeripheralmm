#name of the target binary
TARGET:=uart


#will return these to the calling makefile
#APP_FLAGS := -DUARTA0_ENABLE_INT
mkfile_path := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

#will return these to the calling makefile
#APP_FLAGS := -DUARTA0_ENABLE_INT -DSPIB0_ENABLE_INT
BUILD_SOURCES := $(mkfile_path)/main.cpp
BUILD_INC := $(mkfile_path)
OBJ_DIR = $(mkfile_path)/obj
BUILD_SOURCE_DIRS := $(mkfile_path)

