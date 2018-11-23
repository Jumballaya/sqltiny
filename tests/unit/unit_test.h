/**
 * Unit Testing Tools
 */

// Colors
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define printf_color(color, msg, ...) \
  printf (color); \
  printf (msg, __VA_ARGS__); \
  printf(ANSI_COLOR_RESET);

#define FAIL(msg, ...) \
  printf (ANSI_COLOR_RED); \
  printf (msg, __VA_ARGS__); \
  printf(ANSI_COLOR_RESET);


#define PASS(msg, ...) \
  printf (ANSI_COLOR_GREEN); \
  printf (msg, __VA_ARGS__); \
  printf(ANSI_COLOR_RESET);


#define WARNING(msg, ...) \
  printf (ANSI_COLOR_YELLOW); \
  printf (msg, __VA_ARGS__); \
  printf(ANSI_COLOR_RESET);


#define MESSAGE(msg, ...) \
  printf (ANSI_COLOR_BLUE); \
  printf (msg, __VA_ARGS__); \
  printf(ANSI_COLOR_RESET);

// TEST macro
#define TEST(condition, msg) \
  do { \
    if(!(condition)) { \
      FAIL("----------------------------------------\n", ""); \
      FAIL("\n", ""); \
      FAIL("  Test failed: \n    %s\n", msg); \
      FAIL("\n", ""); \
      FAIL("----------------------------------------\n", ""); \
      return 0; \
    } \
  } while(0)
