#
# TinySQL Build System
#
# Commands
# 	* build - (defaults to make) build the TinySQL CLI binary
# 	* clean - clean the build and bin folders
# 	* test  - build the test binaries, run them and then run the rspec end-to-end tests (runs make build)
#
#	Variables
#		* Utilities
#		* Directories
#		* Compiler settings
#		* General Information
#		* Files/Dependencies
#

# Utilities
CLEANUP = rm -rf
MKDIR = mkdir -p
CP = cp


# Sources
DIR_SRC = src/

DIR_BUILD = build/
DIR_BUILD_OBJECTS = $(DIR_BUILD)objects/
DIR_BUILD_TESTS = $(DIR_BUILD)tests/

DIR_INSTALL = /usr/bin/

DIR_BIN = bin/

DIR_TESTS = tests/
DIR_TESTS_RUBY = $(DIR_TESTS)e2e/
DIR_TESTS_C = $(DIR_TESTS)unit/

DIR_INC = include/
DIR_INC_TESTS = $(DIR_TESTS)$(DIR_INC)


# Compiler settings
CC = gcc
FLAGS = -Wall
INC = -I $(DIR_INC)
INC_TESTS = -I $(DIR_INC) -I $(DIR_INC_TESTS)

# General info
BIN_NAME = sqltiny
TARGET_EXTENSION = out

# Files
ENTRY = main.c

SOURCES 			= $(wildcard $(DIR_SRC)*.c)
SOURCES_TESTS 	= $(wildcard $(DIR_TESTS_C)Test*.c)
HEADERS 			= $(wildcard $(DIR_SRC)*h)
HEADERS_TESTS  = $(wildcard $(DIR_TESTS)*h)
OBJ 					= $(patsubst $(DIR_SRC)%,$(DIR_BUILD_OBJECTS)%,$(SOURCES:.c=.o))

# Create binary
$(DIR_BIN)$(BIN_NAME): $(OBJ)
	$(MKDIR) $(DIR_BIN)
	$(CC) $(FLAGS) -o $@ $(ENTRY) $^

# Create the build files
$(DIR_BUILD_OBJECTS)%.o: $(DIR_SRC)%.c $(HEADERS) $(DIR_BUILD_OBJECTS)
	$(MKDIR) $(DIR_BUILD_OBJECTS)
	$(CC) -c -o $@ $< $(FLAGS) $(INC)

$(DIR_BUILD_OBJECTS):
	$(MKDIR) $(DIR_BUILD_OBJECTS)

$(DIR_BUILD_TESTS):
	$(MKDIR) $(DIR_BUILD_TESTS)

$(DIR_BUILD):
	$(MKDIR) $(DIR_BUILD)

# Clean
clean:
	$(CLEANUP) $(DIR_BIN)
	$(CLEANUP) $(DIR_BUILD)

# Build C Unit tests
build-tests: $(SOURCES_TESTS) $(HEADERS_TESTS)
	$(MKDIR) $(DIR_BUILD_TESTS)
	for test in $(basename $(notdir $(SOURCES_TESTS))) ; do \
		echo $$test ; \
		$(CC) $(FLAGS) -o $(DIR_BUILD_TESTS)$$test $(DIR_TESTS_C)$$test.c $(OBJ) $(INC_TESTS) ; \
	done;

# Run tests
test: $(wildcard $(DIR_BUILD_TESTS)Test*)
	@make
	@make build-tests
	for t in $^ ; do \
		$$t ; \
	done;
	bundle exec rspec -P $(DIR_TESTS_RUBY)*_spec.rb

# Install the binary
install:
	$(CP) $(DIR_BIN)$(BIN_NAME) $(DIR_INSTALL)$(BIN_NAME)

.PHONY: build
.PHONY: clean
.PHONY: build-tests
.PHONY: test
.PHONY: install
