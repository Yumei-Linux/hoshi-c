UNAME_S = $(shell uname -s)

CFLAGS := -std=c11 -O3 -g -Wall -Wextra -Wpedantic -Wstrict-aliasing
CFLAGS += -Wno-pointer-arith -Wno-unused-parameter -Wno-newline-eof -Wno-unused-parameter -Wno-gnu-statement-expression
CFLAGS += -Wno-gnu-compound-literal-initializer -Wno-gnu-zero-variadic-macro-arguments
CFLAGS := -ggdb3 -g

LDFLAGS :=

ifeq ($(UNAME_S), Linux)
	LDFLAGS += -lm
endif

SRC := $(wildcard src/**/*.c) $(wildcard src/*.c) $(wildcard src/**/**/*.c) $(wildcard src/**/**/**/*.c)
OBJ := $(SRC:.c=.o)
BIN := bin

APPNAME := hoshi

.PHONY: all clean

all: dirs app

dirs:
	mkdir -p $(BIN)

run: all
	$(BIN)/$(APPNAME)

app: $(OBJ)
	$(CC) -o $(BIN)/$(APPNAME) $^ $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -rf $(BIN) $(OBJ)
