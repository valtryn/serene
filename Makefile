CC = cc
CDISABLEDFLAGS = -Wno-error=cast-align -Wno-unused-parameter -Wno-unused-variable -Wno-unused-but-set-variable -Wno-sign-conversion
COPTIMIZATION = -O0
CFLAGS = -Wall -Wextra -Werror -std=c11 -g -Wpedantic -Wvla -Wcast-align=strict -Wunreachable-code -Wformat=2 -Wstrict-prototypes
CFLAGS += $(CDISABLEDFLAGS)
CFLAGS += $(COPTIMIZATION)
CFLAGS += -I. -I./core -I./ui -I./test
BUILD_DIR = build
BIN_DIR = bin

CORE_SRC = base/main.c base/allocator.c base/str.c base/ds.c
UI_SRC   = ui/core.c ui/gfx.c ui/color.c ui/components.c
SRC = $(CORE_SRC) $(UI_SRC)

OBJ = $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRC))

LIB = -lm -lX11 -lXext
# export PKG_CONFIG_PATH := $(HOME)/Programming/opt/SDL3/lib/pkgconfig:$(PKG_CONFIG_PATH)
# CFLAGS += $(shell pkg-config --cflags sdl3)
# LDFLAGS += $(shell pkg-config --libs sdl3)

all: $(BIN_DIR)/serene

$(BIN_DIR)/serene: $(OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) $(OBJ) -o $@ $(LDFLAGS) $(LIB)

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -v -rf $(BUILD_DIR)/* $(BIN_DIR)/*

run: all
	./bin/serene

.PHONY: all clean run test
