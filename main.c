/**
 * SQL Tiny application entry
 */
#include <stdio.h>
#include <stdlib.h>

#include "include/repl.h"

int main(int argc, char** argv) {
  CLI* cli = cli_new(argc, argv);
  Repl* repl = repl_new("> ", cli);
  repl_run(repl);

  return 0;
}
