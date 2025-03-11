CC = cc
CDISABLEDFLAGS = -Wno-error=cast-align -Wno-unused-parameter -Wno-unused-variable
CFLAGS = -O0 -march=native -Wall -Wextra -Werror -std=c11 -g -Wpedantic -Wvla -Wcast-align=strict -Wshadow -Wconversion -Wunreachable-code -Wformat=2 -Wstrict-prototypes $(CDISABLEDFLAGS)
BUILD_DIR = build
BIN = bin
SRC = main.c allocator.c str.c ds.c
OBJ = $(SRC:.c=.o)
OBJ_PATHS = $(addprefix $(BUILD_DIR)/, $(OBJ))

LIBS = -lwayland-client

all: $(BIN)/serene

$(BIN)/serene: $(OBJ_PATHS)
	@mkdir -p $(BIN)
	$(CC) $(OBJ_PATHS) -o $@ $(LIBS)

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -v $(BUILD_DIR)/*.o $(BIN)/*

.PHONY: all clean
