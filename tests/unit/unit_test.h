/**
 * Unit Testing Tools
 */

// TEST macro
#define TEST(condition) \
  do { \
    if(!(condition)) { \
      fprintf(stderr, "Test failed\n"); \
      exit(EXIT_FAILURE); \
    } \
  } while(0)
