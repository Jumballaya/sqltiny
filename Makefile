CC = cc
FLAGS = -Wall

SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
INSTALL_DIR = /usr/bin

SOURCES = $(wildcard src/*.c)
HEADERS = $(wildcard src/*h)
OBJ = $(patsubst $(SRC_DIR)/%,$(BUILD_DIR)/%,$(SOURCES:.c=.o))
BIN_NAME = sqltiny



$(BIN_DIR)/$(BIN_NAME): $(OBJ)
	@mkdir -p bin
	$(CC) $(FLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS)
	@echo $@
	@echo $<
	@mkdir -p build
	$(CC) -c -o $@ $< $(FLAGS)

install:
	@cp $(BIN_DIR)/$(BIN_NAME) $(INSTALL_DIR)/$(BIN_NAME)


.PHONY: clean build

clean:
	@rm -rf bin
	@rm -rf build
