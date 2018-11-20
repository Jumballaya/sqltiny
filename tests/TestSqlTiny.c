#include "unity.h"

void test_Fake_should_PassTest(void) {
  int a = 1;
  TEST_ASSERT(a == 1);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_Fake_should_PassTest);
  return UNITY_END();
}
