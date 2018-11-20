ifeq ($(OS),Windows_NT)
  ifeq ($(shell uname -s),) # not in a bash-like shell
	CLEANUP = del /F /Q
	MKDIR = mkdir
	CP = copy
	INSTALL_DIR = C:\\windows\\system32\\
  else # in a bash-like shell, like msys
	CLEANUP = rm -rf
	MKDIR = mkdir -p
	CP = cp
	INSTALL_DIR = /usr/bin/
  endif
	TARGET_EXTENSION=.exe
else
	CLEANUP = rm -rf
	MKDIR = mkdir -p
	TARGET_EXTENSION=out
	CP = cp
	INSTALL_DIR = /usr/bin/
endif

SRC_DIR = src/
BUILD_DIR = build/
BIN_DIR = bin/
TEST_DIR = tests/
UNITY_DIR = unity/src/

CC = cc
FLAGS = -Wall

BIN_NAME = sqltiny

SOURCES = $(wildcard src/*.c)
SOURCES_TEST = $(wildcard tests/*.c)
HEADERS = $(wildcard src/*h)
OBJ = $(patsubst $(SRC_DIR)%,$(BUILD_DIR)%,$(SOURCES:.c=.o))

$(BIN_DIR)$(BIN_NAME): $(OBJ)
	$(MKDIR) $(BIN_DIR)
	$(CC) $(FLAGS) -o $@ $^

$(BUILD_DIR)%.o: $(SRC_DIR)%.c $(HEADERS)
	$(MKDIR) $(BUILD_DIR)
	$(CC) -c -o $@ $< $(FLAGS)

install:
	$(CP) $(BIN_DIR)$(BIN_NAME) $(INSTALL_DIR)$(BIN_NAME)

test: $(BIN_DIR)$(BIN_NAME)
	bundle exec rspec

.PHONY: build
.PHONY: clean
.PHONY: test
.PHONY: install

clean:
	$(CLEANUP) $(BUILD_DIR)
	$(CLEANUP) $(BIN_DIR)
