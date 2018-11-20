/**
 * REPL
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "repl.h"

// Print Usage
void print_usage() {
  printf("SQLTiny version 0.0.1\n");
  printf("Enter \".help\" for usage hints.\n");
  printf("Use \".open FILENAME\" to reopen on another file\n");
}

// Print commands (.exit, .help, etc)
void print_commands() {
  printf("  .exit             Exits the REPL\n");
  printf("  .help             Prints this help message\n");
  printf("  .open FILENAME    Close existing db and open the FILENAME\n");
}

// Creates a new Repl
Repl* repl_new(char* prompt) {
  Repl* repl = malloc(sizeof(Repl));
  repl->prompt = prompt;
  repl->input = new_input_buffer();

  return repl;
}

// Prints the prompt
void print_prompt(char* prompt) {
  printf("%s", prompt);
};

// Read input into buffer
void read_input(InputBuffer* buf) {
  ssize_t bytes = getline(&(buf->buffer), &(buf->buffer_len), stdin);

  if (bytes <= 0) {
    printf("error reading input\n");
    exit(EXIT_FAILURE);
  }

  // Ignore trailing newline
  buf->input_len = bytes - 1;
  buf->buffer[bytes - 1] = 0;
}

// Runs a Repl Command
CommandResult repl_command(InputBuffer* buf) {
  if (strcmp(buf->buffer, ".exit") == 0) {
    exit(EXIT_SUCCESS);
  } else if (strcmp(buf->buffer, ".help") == 0) {
    print_usage();
    print_commands();
    return COMMAND_SUCCESS;
  } else if (strcmp(buf->buffer, ".open") == 0) {
    printf(".open not implemented\n");
    return COMMAND_SUCCESS;
  } else {
    return COMMAND_UNRECOGNIZED;
  }
}

// Runs a REPL
void repl_run(Repl* repl) {
  print_usage();

  while (1) {
    print_prompt(repl->prompt);
    read_input(repl->input);

    if (repl->input->buffer[0] == '.') {
      switch (repl_command(repl->input)) {
        case (COMMAND_SUCCESS):
          continue;
        case (COMMAND_UNRECOGNIZED):
          printf("unrecognized command '%s'.\n", repl->input->buffer);
          continue;
      }
    }
  }
}
