CC = clang
CFLAGS = -g -O0 -Wall -Werror -fPIC
LDFLAGS = 
DYLIB = libutils.dylib

SRC_DIR = .
BIN_DIR = bin
LIB_DIR = lib

SOURCES = $(filter-out $(SRC_DIR)/utils.c, $(wildcard $(SRC_DIR)/*.c))
EXECUTABLES = $(patsubst $(SRC_DIR)/%.c, $(BIN_DIR)/%, $(SOURCES))

default: all

all: $(LIB_DIR)/$(DYLIB) $(EXECUTABLES)

$(LIB_DIR)/$(DYLIB): utils.c utils.h
	[ -d "$(LIB_DIR)" ] || mkdir -p $(LIB_DIR)
	$(CC) $(CFLAGS) -dynamiclib utils.c -o $@

$(BIN_DIR)/%: $(SRC_DIR)/%.c $(LIB_DIR)/$(DYLIB)
	[ "$$(uname -sm)" = "Darwin arm64" ]
	[ -d "$(BIN_DIR)" ] || mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $< -L$(LIB_DIR) -lutils -o $@

clean:
	rm -rf $(BIN_DIR) $(LIB_DIR)

run-%: bin/%
	DYLD_LIBRARY_PATH=$(LIB_DIR) ./bin/$*

.PHONY: clean all default run-%
