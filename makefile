SHELL := /bin/bash
#DIRS:=syk_hal syk_comm periph rf2_instruments wc_control_logic
#MAKEFILE_DIR:=../makefiles/
DIRECTORIES := $(sort $(dir $(wildcard ./apps/*/)))
#TARGETS := $(lastword $(subst /, , $(DIRECTORIES)))
TARGETS := $(foreach DIR,$(DIRECTORIES),$(lastword $(subst /, , $(DIR))))
empty:=

.PHONY: all $(TARGETS)
all: $(TARGETS)
	echo Done.

$(TARGETS):
	echo $@
	+make -f makefile-main.mk APP=$@	
	

wc_control_logic: syk_hal syk_comm periph rf2_instruments

.PHONY: clean install $(addsuffix _clean,$(DIRS))

install: wc_control_logic
	make -C ../$^ -f $(MAKEFILE_DIR)Makefile-${^}.mk install

clean: $(addsuffix -clean,$(TARGETS))
$(addsuffix -clean,$(TARGETS)):
	$(eval tapp := $(subst -clean,,$@))
	+make -f makefile-main.mk clean APP=$(tapp)
