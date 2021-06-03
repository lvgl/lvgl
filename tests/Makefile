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
OPTIMIZATION ?= -O3 -g0

CFLAGS ?= -I$(LVGL_DIR)/ -Iunity $(DEFINES) $(WARNINGS) $(OPTIMIZATION) -I$(LVGL_DIR) -I.

LDFLAGS ?=  -lpng
BIN ?= test

include ../lvgl.mk

CSRCS += ${TEST_SRC}
CSRCS += unity/unity.c
CSRCS += unity/unity_support.c
CSRCS += lv_test_init.c
CSRCS += src/test_fonts/font_1.c
CSRCS += src/test_fonts/font_2.c
CSRCS += src/test_fonts/font_3.c

OBJEXT ?= .o

AOBJS = $(ASRCS:.S=$(OBJEXT))
COBJS = $(CSRCS:.c=$(OBJEXT))

MAINOBJ = $(MAINSRC:.c=$(OBJEXT))

SRCS = $(ASRCS) $(CSRCS) $(MAINSRC)
OBJS = $(AOBJS) $(COBJS)

## MAINOBJ -> OBJFILES

all: default

%.o: %.c
	@$(CC)  $(CFLAGS) -c $< -o $@
#	@echo "CC $<"

	

default: $(AOBJS) $(COBJS) $(MAINOBJ)
	$(CC) -o $(BIN) $(MAINOBJ) $(AOBJS) $(COBJS) $(LDFLAGS)

clean:
	rm -f $(BIN) $(AOBJS) $(COBJS) $(MAINOBJ)
