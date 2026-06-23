# LVGL9 Native Build - Cross-Platform Makefile for Linux & macOS
# Place this Makefile at the LVGL repository root alongside main.c and lv_conf.h.

# ------------------------------------------------------------------------------
# Platform Detection
# ------------------------------------------------------------------------------
UNAME_S := $(shell uname -s)
UNAME_M := $(shell uname -m)

# ------------------------------------------------------------------------------
# Compiler & Toolchain Configuration
# ------------------------------------------------------------------------------
ifeq ($(UNAME_S),Darwin)
    # macOS (Apple Silicon or Intel)
    CC := clang
    ARCH_FLAG := -arch $(UNAME_M)
    # macOS frameworks are typically handled by sdl2-config; no extra sys libs needed
    SYS_LIBS :=
else ifeq ($(UNAME_S),Linux)
    # Linux (x86_64, aarch64, etc.)
    CC := gcc
    ARCH_FLAG :=
    # Older Linux distros may need -ldl and -lrt; modern glibc usually includes them,
    # but adding them is harmless on most systems.
    SYS_LIBS := -ldl -lrt
else
    $(error Unsupported OS: $(UNAME_S))
endif

# ------------------------------------------------------------------------------
# SDL2 Configuration (requires sdl2-config in PATH)
# ------------------------------------------------------------------------------
SDL_CFLAGS := $(shell sdl2-config --cflags 2>/dev/null)
SDL_LIBS   := $(shell sdl2-config --libs 2>/dev/null)

ifeq ($(SDL_CFLAGS),)
    $(error "sdl2-config not found. Please install SDL2 development libraries.")
endif

# ------------------------------------------------------------------------------
# Build Flags
# ------------------------------------------------------------------------------
CFLAGS  := -O3 -flto -g -Wall -Wextra -Wshadow -I. -Isrc -march=armv8-a+sve2
CFLAGS  += -Wno-unused-variable -Wno-unused-parameter
CFLAGS  += $(SDL_CFLAGS) $(ARCH_FLAG)

LDFLAGS := $(SDL_LIBS) $(SYS_LIBS) -lm -lpthread -flto

# ------------------------------------------------------------------------------
# Source Files
# ------------------------------------------------------------------------------
# LVGL core library sources
LVGL_SRC := $(shell find src -type f -name "*.c")

# Benchmark demo sources (including assets)
DEMO_SRC := $(shell find demos/benchmark -type f -name "*.c")
DEMO_SRC += $(shell find demos/widgets -type f -name "*.c")

# Optional: common demos entry point if present
ifneq (,$(wildcard demos/lv_demos.c))
    DEMO_SRC += demos/lv_demos.c
endif

# User application entry point
USER_SRC := main.c

SRC := $(LVGL_SRC) $(DEMO_SRC) $(USER_SRC)
OBJ := $(SRC:.c=.o)

# ------------------------------------------------------------------------------
# Targets
# ------------------------------------------------------------------------------
TARGET := lvgl_benchmark

.PHONY: all clean run info

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(ARCH_FLAG) -o $@ $^ $(LDFLAGS)

# Pattern rule for object compilation
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJ) $(TARGET)

run: $(TARGET)
	./$(TARGET)

# Print build environment for debugging
info:
	@echo "OS:         $(UNAME_S)"
	@echo "Machine:    $(UNAME_M)"
	@echo "Compiler:   $(CC)"
	@echo "SDL CFLAGS: $(SDL_CFLAGS)"
	@echo "SDL LIBS:   $(SDL_LIBS)"