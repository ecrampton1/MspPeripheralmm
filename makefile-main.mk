SHELL := /bin/bash
ARCH:= msp430
MSPLOAD:= ./mspload

include xcompile.mk

BINDIR := ./msp_bin

#pass in the app to build
ifndef APP
$(error no app specified)
endif

#app to build points to a directory in the apps folder
include apps/$(APP)/makefile.mk

SRCDIRS := ./src ./src/msp430

#EXCLUDE_srcs :=

$(TARGET)_srcs := $(shell find $(SRCDIRS) -iname *.cpp -o -iname *.c | sort -u)
#$(TARGET)_srcs := $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.cpp))
#$(TARGET)_srcs += $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.c))


#$(TARGET)_srcs := $(filter-out $(EXCLUDE_srcs), $($(TARGET)_srcs))

#will pass this into the makefile
$(TARGET)_srcs += $(BUILD_SOURCES)


# Set vpaths to find source files.
# This may become problematic if we ever repeat a source file name anywhere...
vpath %.c   $(SRCDIRS) ./apps/$(TARGET)
vpath %.cpp $(SRCDIRS) ./apps/$(TARGET)


INCDIRS := ./inc ./inc/libraries $(BUILD_INC)
INCDIRS += $(SRCDIRS)


#FLAGS   += -g -O0 -Wall                                         
FLAGS   += $(addprefix -I, $(INCDIRS))
CXXFLAGS += -fmessage-length=0 $(FLAGS)
CFLAGS   += --std=gnu11 $(FLAGS)


$(TARGET)_map:=$(OBJ_DIR)/$(TARGET).map
$(TARGET)_file:=$(OBJ_DIR)/$(TARGET).elf

$(TARGET)_objdir := $(OBJ_DIR)
$(TARGET)_srcs := $(sort $(notdir $(filter %.c,$($(TARGET)_srcs)) $(filter %.cpp,$($(TARGET)_srcs))))
#$(TARGET)_objs := $(shell find $(SRCDIRS) -iname *.cpp -o -iname *.c | sort -u)
$(TARGET)_objs   := $(addprefix $($(TARGET)_objdir)/,$(subst .c,.o,$($(TARGET)_srcs:.cpp=.o)))

$(TARGET)_deps := $(subst .o,.d,$($(TARGET)_objs))

$($(TARGET)_deps) $($(TARGET)_objs) $($(TARGET)_file): | $($(TARGET)_objdir)


$($(TARGET)_objdir)/%.o: %.cpp
	#@mkdir -p $(@D)
	$(CXX) $($(TARGET)_cxxflags) $(CXXFLAGS) -c -o $@ $<

$($(TARGET)_objdir)/%.o: %.c
	#@mkdir -p $(@D)
	$(CC) $($(TARGET)_cflags) $(CFLAGS) -c -o $@ $<

$($(TARGET)_objdir)/%.d: %.c
	#@mkdir -p $(@D)
	echo Generating $@...
	$(CC) -M $< -MT "$(@:.d=.o) $@" $($(TARGET)_cflags) $(CFLAGS) -MF $@

$($(TARGET)_objdir)/%.d: %.cpp
	#@mkdir -p $(@D)
	echo Generating $@...
	$(CXX)  -M $< -MT "$(@:.d=.o) $@" $($(TARGET)_cxxflags) $(CXXFLAGS) -MF $@

.PHONY: $(TARGET)
$(TARGET): $($(TARGET)_file)
$($(TARGET)_file): $($(TARGET)_deps) $($(TARGET)_objs) $(BINDIR)
	@echo Hello World $($(TARGET)_deps)
	$(CXX) -o $@ $($(TARGET)_objs) $($(TARGET)_cxxflags) $(CXXFLAGS) $($(TARGET)_ldflags) $(LDFLAGS)
	$(SIZE) $@
	@cp $@ $(BINDIR)

.DEFAULT_GOAL:=
.PHONY: all clean-all
all: $(TARGET)

.PHONY: clean
clean:
	rm -rf  $($(TARGET)_file) $($(TARGET)_objs) $($(TARGET)_objdir) $(BINDIR)
        
.PHONY: install
install:
	$(MSPLOAD)  $($(TARGET)_file)

all_objdirs:=$(sort $(OBJDIR_BASE) $(OBJDIR) $(all_objdirs))# sort them to get rid of any dupes
all_objdirs += $($(TARGET)_objdir) $(BINDIR)

$(all_objdirs):
	mkdir -p $@

clean-all: cleanall

include $($(TARGET)_deps) 



