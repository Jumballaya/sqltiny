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

# Sources
DIR_SRC = src/
DIR_BUILD = build/
DIR_BIN = bin/
DIR_TESTS = tests/
DIR_TESTS_RUBY = $(DIR_TESTS)e2e/
DIR_TESTS_C = $(DIR_TESTS)unit/

# Compiler settings
CC = cc
FLAGS = -Wall
INC = -I.
TEST_INC = -I./src

# General info
BIN_NAME = sqltiny

# Files
ENTRY = main.c

SOURCES 			= $(wildcard $(DIR_SRC)*.c)
SOURCES_TEST 	= $(wildcard $(DIR_TESTS_C)Test*.c)
HEADERS 			= $(wildcard $(DIR_SRC)*h)
OBJ 					= $(patsubst $(DIR_SRC)%,$(DIR_BUILD)%,$(SOURCES:.c=.o))

# Create binary
$(DIR_BIN)$(BIN_NAME): $(OBJ)
	$(MKDIR) $(DIR_BIN)
	$(CC) $(FLAGS) -o $@ $(ENTRY) $^

# Create the build files
$(DIR_BUILD)%.o: $(DIR_SRC)%.c $(HEADERS)
	$(MKDIR) $(DIR_BUILD)
	$(CC) -c -o $@ $< $(FLAGS) $(INC)

# Clean
clean:
	$(CLEANUP) $(DIR_BUILD)
	$(CLEANUP) $(DIR_BIN)

# Build C Unit tests
build-tests: $(SOURCES_TEST)
	for test in $(basename $(notdir $(SOURCES_TEST))) ; do \
		echo $$test ; \
		$(CC) $(FLAGS) -o $(DIR_BIN)$$test $(DIR_TESTS_C)$$test.c $(OBJ) -I./src ; \
	done;

# Run tests
test: $(wildcard $(DIR_BIN)Test*)
	@make build-tests
	for t in $^ ; do \
		$$t ; \
	done;
	bundle exec rspec -P $(DIR_TESTS_RUBY)*_spec.rb

# Install the binary
install:
	$(CP) $(DIR_BIN)$(BIN_NAME) $(INSTALL_DIR)$(BIN_NAME)

.PHONY: build
.PHONY: clean
.PHONY: build-tests
.PHONY: test
.PHONY: install
