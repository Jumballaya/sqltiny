/**
 * CLI Tests
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unit_test.h"

#include "cli.h"

// Create test CLI
CLI* buildCLI(int argc, char** argv) {
  if (argc < 0) {
    argc = 5;
    char* input[] = { "./file", "-p", "8080", "--host", "localhost" };
    argv = input;
  }
  CLI* cli = cli_new(argc, argv);
  return cli;
}

// Test CLI Initialization
char* TestCLIInitialization() {
  CLI* cli = buildCLI(-1, NULL);
  TEST(cli != NULL, "Test CLI Initialization:\n  cli failed to initialize");
}

// Test CLI has default properties
void* TestCLIDefaults() {
  char* empty[0];
  CLI* cli = buildCLI(0, empty);

  TEST(strcmp(cli->host, "localhost") == 0, "default property host is incorrect");
  TEST(cli->port == 3001, "default property port is incorrect");
  TEST(strcmp(cli->file, "database.db") == 0, "default property file is incorrect");
}

int main(void) {
  printf("\n\n\n");
  MESSAGE("------- Test: CLI -------\n", "");
  printf("\n");

  TestCLIInitialization();
  TestCLIDefaults();
  return 0;
}
