/**
 * REPL Tests
 */
#include <stdio.h>
#include <stdlib.h>

// Test Fail
void fail(char* testname, char* msg) {
  fprintf(stderr, "\n");
  fprintf(stderr, "  Module: %s\n", testname);
  fprintf(stderr, "---------------------------------------\n");
  fprintf(stderr, "  Failure: %s\n", msg);
  fprintf(stderr, "---------------------------------------\n");
  fprintf(stderr, "\n");
}

// Test CLI has default properties
char* TestReplDefaults() {
  if (0) {
    return "Error!";
  }
  return NULL;
}

int main(void) {
  char* errors[2];
  int count = 0;

  char* error_default = TestReplDefaults();

  if (error_default != NULL) errors[count++] = error_default;

  for (int i = 0; i < count; i++) {
    fail("REPL tests", errors[i]);
  }

  return count == 0 ? 0 : 1;
}
