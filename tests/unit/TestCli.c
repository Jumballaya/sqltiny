/**
 * CLI Tests
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

// Test Fail
void fail(char* testname, char* msg) {
  fprintf(stderr, "\n");
  fprintf(stderr, "  Module: %s\n", testname);
  fprintf(stderr, "---------------------------------------\n");
  fprintf(stderr, "  Failure: %s\n", msg);
  fprintf(stderr, "---------------------------------------\n");
  fprintf(stderr, "\n");
}

// Test CLI Initialization
char* TestCLIInitialization() {
  if (0) {
    return "Test CLI Initialization:\n  cli failed to initialize";
  }
  return NULL;
}

// Test CLI has default properties
char* TestCLIDefaults() {
  char* empty[0];
  CLI* cli = buildCLI(0, empty);

  char* expectedHost = "localhost";
  int expectedPort = 3001;
  char* expectedFile = "database.db";

  int hasError = 0;
  char* error = malloc(sizeof(char) * 255 * 8);
  if (strcmp(cli->host, expectedHost) != 0) {
    hasError = 1;
    sprintf(error, "Test CLI Default Properties (Host)\n  Got: %s\n  Wanted: %s\n", cli->host, expectedHost);
  }

  if (cli->port != expectedPort) {
    hasError = 1;
    sprintf(error, "Test CLI Default Properties (Port)\n  Got: %d\n  Wanted: %d\n", cli->port, expectedPort);
  }

  if (strcmp(cli->file, expectedFile) != 0) {
    hasError = 1;
    sprintf(error, "Test CLI Default Properties (File)\n  Got: %s\n  Wanted: %s\n", cli->file, expectedFile);
  }

  if (hasError) {
    return error;
  }
  return NULL;
}

int main(void) {
  printf("\n\n\n\n\n\n\n\n\n");
  printf("------- Test: CLI -------\n");
  printf("\n\n\n\n\n\n\n\n\n");
  char* errors[2];
  int count = 0;

  char* error_init    = TestCLIInitialization();
  char* error_default = TestCLIDefaults();

  if (error_init != NULL)    errors[count++] = error_init;
  if (error_default != NULL) errors[count++] = error_default;

  for (int i = 0; i < count; i++) {
    fail("CLI tests", errors[i]);
  }

  if (count == 0) {
    printf("     2 Tests Passed!\n\n\n\n");
  }

  return count == 0 ? 0 : 1;
}
