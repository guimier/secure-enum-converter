SUB_MAKEFILES = $(wildcard */Makefile)
SUB_DIRS = $(SUB_MAKEFILES:%/Makefile=%)

all: $(SUB_DIRS)
$(SUB_DIRS):
	$(MAKE) -C $@

.PHONY: all $(SUB_DIRS)
