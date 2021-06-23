#
# Makefile
#
CC ?= gcc
LVGL_DIR ?= ${shell pwd}/../..
LVGL_DIR_NAME ?= lvgl

WARNINGS = -Werror -Wall -Wextra \
           -Wshadow -Wundef -Wmaybe-uninitialized -Wmissing-prototypes -Wpointer-arith -Wuninitialized \
           -Wunreachable-code -Wreturn-type -Wmultichar -Wformat-security -Wdouble-promotion -Wclobbered -Wdeprecated  \
           -Wempty-body -Wshift-negative-value -Wstack-usage=2048  \
           -Wtype-limits -Wsizeof-pointer-memaccess -Wmissing-prototypes -Wno-discarded-qualifiers

WARNINGS += -Wpedantic -pedantic-errors

#-Wno-unused-value -Wno-unused-parameter
OPTIMIZATION ?= -g0

CFLAGS ?= -I$(LVGL_DIR)/ --coverage -Isrc -Iunity $(DEFINES) $(WARNINGS) $(OPTIMIZATION) -I$(LVGL_DIR) -I.

LDFLAGS ?=  -lpng --coverage 
BIN ?= test

include ../lvgl.mk

CSRCS += ${TEST_SRC}
CSRCS += src/lv_test_init.c
CSRCS += src/lv_test_indev.c
CSRCS := $(CSRCS) $(EXTRA_CSRCS)

OBJEXT ?= .o

AOBJS = $(ASRCS:.S=$(OBJEXT))
COBJS = $(CSRCS:.c=$(OBJEXT))

MAINOBJ = $(MAINSRC:.c=$(OBJEXT))

SRCS = $(ASRCS) $(CSRCS) $(MAINSRC)
OBJS = $(AOBJS) $(COBJS)

## MAINOBJ -> OBJFILES

all: default

%.o: %.c
	@$(CC)  $(CFLAGS) -c -o $@ $<
#	@echo "CC $<"	

default: $(AOBJS) $(COBJS) $(MAINOBJ)
	$(CC) -o $(BIN) $(MAINOBJ) $(AOBJS) $(COBJS) $(LDFLAGS)

clean:
	find ../ -type f -name '*.o' -exec rm -f {} +
	find ../ -type f -name '*.gcda' -exec rm -f {} +
	find ../ -type f -name '*.gcno' -exec rm -f {} +
	rm -f $(BIN) 
