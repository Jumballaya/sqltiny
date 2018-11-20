ifeq ($(OS),Windows_NT)
  ifeq ($(shell uname -s),) # not in a bash-like shell
	CLEANUP = del /F /Q
	MKDIR = mkdir
	CP = copy
	INSTALL_DIR = C:\\windows\\system32\\
  else # in a bash-like shell, like msys
	CLEANUP = rm -f
	MKDIR = mkdir -p
	CP = cp
	INSTALL_DIR = /usr/bin/
  endif
	TARGET_EXTENSION=.exe
else
	CLEANUP = rm -f
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
FLAGS_TEST = -Wall -I$(UNITY_DIR)

BIN_NAME = sqltiny

SOURCES = $(wildcard src/*.c)
SOURCES_TEST = $(wildcard tests/*.c)
HEADERS = $(wildcard src/*h)
OBJ = $(patsubst $(SRC_DIR)%,$(BUILD_DIR)%,$(SOURCES:.c=.o))

RESULTS = $(patsubst $(TEST_DIR)Test%,$(BUILD_DIR)%,$(SOURCES_TEST:.c=.o))
PASSED = `grep -s PASS $(PATHR)*.txt`
FAIL = `grep -s FAIL $(PATHR)*.txt`
IGNORE = `grep -s IGNORE $(PATHR)*.txt`

$(BIN_DIR)$(BIN_NAME): $(OBJ)
	$(MKDIR) $(BIN_DIR)
	$(CC) $(FLAGS) -o $@ $^

$(BUILD_DIR)%.o: $(SRC_DIR)%.c $(HEADERS)
	$(MKDIR) $(BUILD_DIR)
	$(CC) -c -o $@ $< $(FLAGS)

$(BUILD_DIR)%.o: $(TEST_DIR)Test%.c $(HEADERS)
	$(MKDIR) $(BUILD_DIR)
	$(CC) -c -o $@ $< $(FLAGS_TEST)

install:
	$(CP) $(BIN_DIR)$(BIN_NAME) $(INSTALL_DIR)$(BIN_NAME)

test: $(OBJS) $(RESULTS)
	@echo "-----------------------\nIGNORES:\n-----------------------"
	@echo "$(IGNORE)"
	@echo "-----------------------\nFAILURES:\n-----------------------"
	@echo "$(FAIL)"
	@echo "-----------------------\nPASSED:\n-----------------------"
	@echo "$(PASSED)"
	@echo "\nDONE"

.PHONY: build clean
.PHONY: test

clean:
	@rm -rf bin
	@rm -rf build
