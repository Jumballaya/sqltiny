/**
 * REPL
 */
#ifndef REPL_H
#define REPL_H

#include "cli.h"
#include "sql.h"
#include "db.h"

// Commands
enum CommandResult_t {
  COMMAND_SUCCESS,
  COMMAND_UNRECOGNIZED
};
typedef enum CommandResult_t CommandResult;

// REPL
struct Repl_t {
  CLI*         cli;
  char*        prompt;
  InputBuffer* input;
};
typedef struct Repl_t Repl;

// Create new initialized REPL
Repl* repl_new(char* prompt, CLI* cli);

// Run the repl
void repl_run(Repl* repl);

#endif
