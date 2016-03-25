TARGET:=timer

#mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))

mkfile_path := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

 #name of the target binary
#will pass this into the makefile
#APP_FLAGS := -DUARTA0_ENABLE_INT
BUILD_SOURCES := $(mkfile_path)/main.cpp
BUILD_INC := $(mkfile_path)
OBJ_DIR = $(mkfile_path)/obj

