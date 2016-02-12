TARGET:=gpio

#mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))

mkfile_path := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

 #name of the target binary
#will pass this into the makefile
BUILD_SOURCES := $(mkfile_path)/main.cpp
BUILD_INC := $(mkfile_path)
OBJ_DIR = $(mkfile_path)/obj

