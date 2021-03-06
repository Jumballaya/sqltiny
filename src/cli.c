/**
 * CLI
 *
 *  @TODO: Add the ability to run a 1-off query against the database. Clean up the CLI code and
 *      make it a little more extendable.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cli.h"

// Print Help Message
void cli_print_usage() {
  printf("Usage: %s [OPTIONS] FILENAME\n", APP_NAME);
  printf("FILENAME is the name of a SQLTiny database. A new database is\n");
  printf("created if the file does not already exist. Defaults to database.db\n");
  printf("OPTIONS include:\n");
  printf("   -h, --host           hostname of the tinysql server, defaults to localhost\n");
  printf("   -p, --port           port of the tinysql server, defaults to 3001\n");
  printf("   -help, --help        this help menu\n");
  printf("   -v, --version        print current version\n");
}

// Print version message
void cli_print_version() {
  printf("%s version %s\n", APP_NAME, APP_VERSION);
}

// Parses the raw argv array into the cli options
void cli_parse_raw(CLI* cli) {
  // Set defaults
  cli->host = "localhost";
  cli->port = 3001;
  cli->file = "database.db";

  char* tok;
  for (int i = 0; i < cli->argc; i++) {
    tok = cli->raw[i];

    // Hostname
    if (strcmp(tok, "-h") == 0 || strcmp(tok, "--host") == 0) {
      i++;
      cli->host = cli->raw[i];
      continue;
    }

    // Port Number
    if (strcmp(tok, "-p") == 0 || strcmp(tok, "--port") == 0) {
      i++;
      cli->port = atoi(cli->raw[i]);
      continue;
    }

    // Help
    if (strcmp(tok, "-help") == 0 || strcmp(tok, "--help") == 0) {
      i++;
      cli_print_usage();
      exit(0);
    }

    // Version
    if (strcmp(tok, "-v") == 0 || strcmp(tok, "--version") == 0) {
      i++;
      cli_print_version();
      exit(0);
    }

    // The other option is the FILENAME
    if (i != 0) {
      cli->file = cli->raw[i];
    }
  }
}

// Creates and returns a new CLI struct
CLI* cli_new(int argc, char** raw) {
  CLI* cli = malloc(sizeof(CLI));

  cli->version = APP_VERSION;
  cli->raw = raw;
  cli->argc = argc;

  cli_parse_raw(cli);

  return cli;
}
