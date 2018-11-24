/**
 * REPL
 *
 *  @TODO: Better control of the input, like back-arrows maybe multi-line.
 *      The ability to change the database settings with SQL or commands.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "repl.h"
#include "util.h"

// Adds n amount of spaces as an indent
void indent(uint32_t level) {
  for (uint32_t i = 0; i < level; i++) {
    printf("  ");
  }
}

// Print Constants
void print_constants(Table* t) {
  printf("  ROW_SIZE: %d\n", t->row_size);
  printf("  COMMON_NODE_HEADER_SIZE: %d\n", COMMON_NODE_HEADER_SIZE);
  printf("  LEAF_NODE_HEADER_SIZE: %d\n", LEAF_NODE_HEADER_SIZE);
  printf("  LEAF_NODE_CELL_SIZE: %d\n", LEAF_NODE_CELL_SIZE);
  printf("  LEAF_NODE_SPACE_FOR_CELLS: %d\n", LEAF_NODE_SPACE_FOR_CELLS);
  printf("  LEAF_NODE_MAX_CELLS: %d\n", LEAF_NODE_MAX_CELLS);
}

// Print Btree
void print_tree(Pager* pager, uint32_t page_num, uint32_t indent_level) {
  void* node = db_get_page(pager, page_num);
  uint32_t num_keys, child;

  switch (btree_get_node_type(node)) {
    case (NODE_LEAF):
      num_keys = *btree_leaf_node_num_cells(node);
      indent(indent_level);
      printf("- leaf (size %d)\n", num_keys);
      for (uint32_t i = 0; i < num_keys; i++) {
        indent(indent_level + 1);
        printf("- %d\n", *btree_leaf_node_key(node, i));
      }
      break;
    case (NODE_INTERNAL):
      num_keys = *btree_internal_node_num_keys(node);
      indent(indent_level);
      printf("- internal (size %d)\n", num_keys);
      for (uint32_t i = 0; i < num_keys; i++) {
        child = *btree_internal_node_child(node, i);
        print_tree(pager, child, indent_level + 1);
        indent(indent_level);
        printf("- key %d\n", *btree_internal_node_key(node, i));
      }
      child = *btree_internal_node_right_child(node);
      print_tree(pager, child, indent_level + 1);
      break;
  }
}

// Print Btree
void print_leaf_node(void* node) {
  uint32_t num_cells = *btree_leaf_node_num_cells(node);
  printf("- leaf (size %d)\n", num_cells);
  for (uint32_t i = 0; i < num_cells; i++) {
    uint32_t key = *btree_leaf_node_key(node, i);
    printf("  - %d : %d\n", i, key);
  }
}

// Print Usage
void print_usage() {
  printf("SQLTiny version %s\n", APP_VERSION);
  printf("Enter \".help\" for usage hints.\n");
  printf("Use \".open FILENAME\" to reopen on another file\n");
}

// Print commands (.exit, .help, etc)
void print_commands() {
  printf("  .exit             Exits the REPL\n");
  printf("  .help             Prints this help message\n");
  printf("  .constants        Prints database constants\n");
  printf("  .btree            Prints the database btree\n");
  printf("  .open FILENAME    Close existing db and open the FILENAME\n");
}

// Creates a new Repl
Repl* repl_new(char* prompt, CLI* cli) {
  Repl* repl = malloc(sizeof(Repl));
  repl->cli = cli;
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
CommandResult repl_command(InputBuffer* buf, Table* table) {
  if (strcmp(buf->buffer, ".exit") == 0) {
    db_close(table);
    exit(EXIT_SUCCESS);
  } else if (strcmp(buf->buffer, ".constants") == 0) {
    printf("Constants:\n");
    print_constants(table);
    return COMMAND_SUCCESS;
  } else if (strcmp(buf->buffer, ".btree") == 0) {
    printf("BTree:\n");
    print_leaf_node(db_get_page(table->pager, 0));
    return COMMAND_SUCCESS;
  } else if (strcmp(buf->buffer, ".help") == 0) {
    print_usage();
    print_commands();
    return COMMAND_SUCCESS;
  } else if (strncmp(buf->buffer, ".open", 5) == 0) {
    char* fname = string_chop(buf->buffer, ".open ");
    *table = *(db_open(fname));
    printf("initializing database: %s\n\n", fname);
    return COMMAND_SUCCESS;
  } else {
    return COMMAND_UNRECOGNIZED;
  }
}

// Runs a REPL
void repl_run(Repl* repl) {
  /*print_usage();*/

  Table* table = db_open(repl->cli->file);

  while (1) {
    print_prompt(repl->prompt);
    read_input(repl->input);

    if (repl->input->buffer[0] == '.') {
      switch (repl_command(repl->input, table)) {
        case (COMMAND_SUCCESS):
          continue;
        case (COMMAND_UNRECOGNIZED):
          printf("unrecognized command '%s'.\n", repl->input->buffer);
          continue;
      }
    }

    Statement stmt;
    switch (sql_prepare_statement(repl->input, &stmt)) {
      case (PREPARE_SUCCESS):
        break;
      case (PREPARE_NEGATIVE_ID):
        printf("error: ID must be positive\n");
        continue;
      case (PREPARE_STRING_TOO_LONG):
        printf("error: string is too long\n");
        continue;
      case (PREPARE_SYNTAX_ERROR):
        printf("syntax error. Could not parse statement:\n");
        printf("  %s\n", repl->input->buffer);
        continue;
      case (PREPARE_UNRECOGNIZED):
        printf("unrecognized keyword at the start of '%s'.\n", repl->input->buffer);
        continue;
    }

    switch (sql_execute_statement(&stmt, table)) {
      case (EXECUTE_SUCCESS):
        printf("Executed\n");
        break;
      case (EXECUTE_DUPLICATE_KEY):
        printf("error: duplicate key\n");
        break;
      case (EXECUTE_TABLE_FULL):
        printf("error: table full\n");
        break;
    }
  }
}
