/**
 * REPL Tests
 */
#include <stdio.h>
#include <stdlib.h>

#include "unit_test.h"

// Test CLI has default properties
void* TestReplDefaults() {
  TEST(1 == 2, "REPL defaults are incorrect");
}

int main(void) {
  printf("\n\n\n");
  MESSAGE("------- Test: REPL -------\n", "");
  printf("\n");

  TestReplDefaults();

  return 0;
}
