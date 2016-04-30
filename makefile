SHELL := /bin/bash
DIRECTORIES := $(sort $(dir $(wildcard ./apps/*/)))
TARGETS := $(foreach DIR,$(DIRECTORIES),$(lastword $(subst /, , $(DIR))))
empty:=

.PHONY: all $(TARGETS)
all: $(TARGETS)
	echo Done.

$(TARGETS):
	echo $@
	+make -f makefile-main.mk APP=$@	

.PHONY: clean install )

clean: $(addsuffix -clean,$(TARGETS))
$(addsuffix -clean,$(TARGETS)):
	$(eval tapp := $(subst -clean,,$@))
	+make -f makefile-main.mk clean APP=$(tapp)
