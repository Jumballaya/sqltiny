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
int TestCLIInitialization() {
  CLI* cli = buildCLI(-1, NULL);
  TEST(cli != NULL, "Test CLI Initialization:\n  cli failed to initialize");
}

// Test CLI has default properties (host)
int TestCLIDefaultHost() {
  char* empty[0];
  CLI* cli = buildCLI(0, empty);
  TEST(strcmp(cli->host, "localhost") == 0, "default property host is incorrect");
}

// Test CLI has default properties (port)
int TestCLIDefaultPort() {
  char* empty[0];
  CLI* cli = buildCLI(0, empty);
  TEST(cli->port == 3001, "default property port is incorrect");
}

// Test CLI has default properties (file)
int TestCLIDefaultFile() {
  char* empty[0];
  CLI* cli = buildCLI(0, empty);
  TEST(strcmp(cli->file, "database.db") == 0, "default property file is incorrect");
}

int main() {
  printf("\n\n\n");
  MESSAGE("------- Test: %s -------\n", "CLI");
  printf("\n");

  int (*tests[4])() = {
    TestCLIInitialization,
    TestCLIDefaultHost,
    TestCLIDefaultPort,
    TestCLIDefaultFile
  };

  int fails = 0;
  for (int i = 0; i < 4; i++) {
    fails += tests[i]();
  }
  if (fails > 0) {
    MESSAGE("------- Number of failures : %d -------\n", fails);
    printf("\n");
  }

  return 0;
}
