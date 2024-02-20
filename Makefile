CFLAGS := -std=c11 -O3 -g -Wall -Wextra -Wpedantic -Wstrict-aliasing
CFLAGS += -Wno-pointer-arith -Wno-unused-parameter -Wno-unused-parameter

# uncomment for debugging purposes
# CFLAGS := -ggdb3 -g

LDFLAGS := -lcjson

SRC := $(wildcard src/**/*.c) $(wildcard src/*.c) $(wildcard src/**/**/*.c) $(wildcard src/**/**/**/*.c)
OBJ := $(SRC:.c=.o)
BIN := bin

APPNAME := hoshi

.PHONY: all clean

all: configuration dirs app

configuration:
	@echo "Showing configuration for building:"
	@echo "  * LDFLAGS:   $(LDFLAGS)"
	@echo "  * CFLAGS:    $(CFLAGS)"
	@echo "---------------------------------------------------------------------------"

dirs:
	@mkdir -p $(BIN)

run: all
	$(BIN)/$(APPNAME)

app: $(OBJ)
	@echo "BIN   $(BIN)/$(APPNAME)"
	@$(CC) -o $(BIN)/$(APPNAME) $^ $(LDFLAGS)

%.o: %.c
	@echo "CC    $<"
	@$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -rf $(BIN) $(OBJ)
