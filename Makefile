CC = cc
CDISABLEDFLAGS = -Wno-error=cast-align -Wno-unused-parameter -Wno-unused-variable -Wno-unused-but-set-variable -Wno-sign-conversion
CFLAGS = -O0 -Wall -Wextra -Werror -std=c11 -g -Wpedantic -Wvla -Wcast-align=strict -Wunreachable-code -Wformat=2 -Wstrict-prototypes
CFLAGS += $(CDISABLEDFLAGS)
BUILD_DIR = build
BIN = bin
SRC = main.c allocator.c str.c ds.c gui.c
OBJ = $(addprefix $(BUILD_DIR)/, $(SRC:.c=.o))
LIB = -lm

export PKG_CONFIG_PATH := $(HOME)/Programming/opt/SDL3/lib/pkgconfig:$(PKG_CONFIG_PATH)
CFLAGS += $(shell pkg-config --cflags sdl3)
LDFLAGS += $(shell pkg-config --libs sdl3)

all: $(BIN)/serene

$(BIN)/serene: $(OBJ)
	@mkdir -p $(BIN)
	$(CC) $(OBJ) -o $@ $(LDFLAGS) $(LIB)

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -v $(BUILD_DIR)/*.o $(BIN)/*

run: all
	./bin/serene

.PHONY: all clean run test
