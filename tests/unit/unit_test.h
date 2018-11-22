/**
 * Unit Testing Tools
 */

// TEST macro
#define TEST(condition, msg) \
  do { \
    if(!(condition)) { \
      fprintf(stderr, "----------------------------------------\n"); \
      fprintf(stderr, "\n"); \
      fprintf(stderr, "  Test failed: \n    %s\n", msg); \
      fprintf(stderr, "\n"); \
      fprintf(stderr, "----------------------------------------\n"); \
      return 0; \
    } \
  } while(0)
