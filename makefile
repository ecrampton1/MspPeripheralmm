#
# Makefile for msp430
#
# 'make' builds everything
# 'make clean' deletes everything except source files and Makefile
# 'make gdb' starts the gdb  connection to port 2000
# You need to set TARGET, MCU and SOURCES for your project.
# TARGET is the name of the executable file to be produced 
# $(TARGET).elf $(TARGET).hex and $(TARGET).txt nad $(TARGET).map are all generated.
# The TXT file is used for BSL loading, the ELF can be used for JTAG use
# 
#TARGET     = rf24_temp_msp
TARGET     = msp_test
MCU        = msp430g2553
GDB        = msp430-gdb
# List all the source files here
# eg if you have a source file foo.c then list it here
SOURCES = 

ifdef RF_TEMP_MAIN
	SOURCES += main_rf_temp.c ./adc_driver/adc.c
else
	SOURCES += ./test/main.cpp  ./src/msp_sys.cpp ./src/mcu_sys.cpp
endif
# Include are located in the Include directory
INCLUDES = -I./inc
# Add or subtract whatever MSPGCC flags you want. There are plenty more
#######################################################################################
CFLAGS   = -mmcu=$(MCU) -Os -Wall -Wreturn-type -Wunused -s -fdata-sections -ffunction-sections -fno-exceptions -std=c++0x $(INCLUDES)   
ASFLAGS  = -mmcu=$(MCU) -x assembler-with-cpp -Wa
#LDFLAGS  = -mmcu=$(MCU) --no-keep-memory -Wl,-gc-sections, -Map=$(TARGET).map
#TODO OPTIMIZE BUILD--no-keep-memory
#CFLAGS   = -mmcu=$(MCU) -Os -Wall -Wunused -s -fdata-sections -ffunction-sections $(INCLUDES)   
#ASFLAGS  = -mmcu=$(MCU) -x assembler-with-cpp -Wa 
LDFLAGS  = -mmcu=$(MCU) --no-keep-memory -Wl,--gc-sections,-Map=$(TARGET).map
########################################################################################
CC       = msp430-gcc
CXX		 = msp430-g++
LD       = msp430-ld
AR       = msp430-ar
AS       = msp430-gcc
GASP     = msp430-gasp
NM       = msp430-nm
OBJCOPY  = msp430-objcopy
RANLIB   = msp430-ranlib
STRIP    = msp430-strip
SIZE     = msp430-size
READELF  = msp430-readelf
CP       = cp -p
RM       = rm -f
MV       = mv
########################################################################################
# the file which will include dependencies
DEPEND = $(SOURCES:.cpp=.d)
# all the object files
OBJECTS = $(SOURCES:.cpp=.o)
all: $(TARGET).elf  
$(TARGET).elf: $(OBJECTS)
	echo "Linking $(OBJECTS)"
	$(CC) $(OBJECTS) $(LDFLAGS) $(LIBS) -o $@
	echo
	echo ">>>> Size of Firmware <<<<"
	$(SIZE) $(TARGET).elf
	echo
%.o: %.cpp
	echo "Compiling $<"
	$(CC) -c $(CFLAGS) -o $@ $<
# rule for making assembler source listing, to see the code
%.lst: %.cpp
	echo "Source listing $<"
	$(CC) -c $(ASFLAGS) -Wa,-anlhd $< > $@
# include the dependencies unless we're going to clean, then forget about them.
ifneq ($(MAKECMDGOALS), clean)
-include $(DEPEND)
endif
# dependencies file
# includes also considered, since some of these are our own
# (otherwise use -MM instead of -M)
%.d: %.cpp
	echo "Generating dependencies $@ from $<"
	$(CC) -M ${CFLAGS} $< >$@
.SILENT:
.PHONY:	clean
clean:
	-$(RM) $(OBJECTS)
	-$(RM) $(TARGET).*
	#-$(RM) $(SOURCES:.c=.lst)
	#-$(RM) $(DEPEND)
.PHONY:	gdb
gdb:
	$(GDB) --eval-command "target remote localhost:2000" $(TARGET).elf
	
.PHONY:	upload
upload:
	mspdebug rf2500 "prog $(TARGET).elf"

