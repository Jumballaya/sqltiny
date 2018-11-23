/**
 * CLI
 */
#ifndef CLI_H
#define CLI_H

#include "util.h"

#define APP_NAME      "sqltiny"
#define APP_VERSION   "0.0.1"

struct cli_t {
  int     argc;
  char*   version;
  char**  raw;
  char*   host;
  char*   file;
  int     port;
};
typedef struct cli_t CLI;

CLI* cli_new(int argc, char** raw);

#endif
