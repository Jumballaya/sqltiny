/**
 * REPL Tests
 */
#include <stdio.h>
#include <stdlib.h>

#include "unit_test.h"

// Test CLI has default properties
int TestReplDefaults() {
  TEST(1 == 2, "REPL defaults are incorrect");
}

int main(void) {
  printf("\n\n\n");
  MESSAGE("------- Test: %s -------\n", "REPL");
  printf("\n");

  int (*tests[1])() = {
    TestReplDefaults,
  };

  int fails = 0;
  for (int i = 0; i < 1; i++) {
    fails += tests[i]();
  }
  if (fails > 0) {
    MESSAGE("------- Number of failures : %d -------\n", fails);
    printf("\n");
  }

  return 0;
}
