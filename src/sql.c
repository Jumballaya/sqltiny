/**
 * SQL
 */
#include <stdio.h>
#include <string.h>

#include "sql.h"

// Prepares the SQL statement from the buffer
PrepareResult sql_prepare_statement(InputBuffer* buf, Statement* stmt) {
  // INSERT statement
  if (strncmp(buf->buffer, "insert", 6) == 0) {
    stmt->type = STATEMENT_INSERT;
    return PREPARE_SUCCESS;
  }

  // SELECT statement
  if (strcmp(buf->buffer, "select") == 0) {
    stmt->type = STATEMENT_SELECT;
    return PREPARE_SUCCESS;
  }

  return PREPARE_UNRECOGNIZED;
};

// Executes the SQL statement
void execute_statement(Statement* stmt) {
  switch (stmt->type) {
    case (STATEMENT_INSERT):
      printf("INSERT statement here...\n");
      break;
    case (STATEMENT_SELECT):
      printf("SELECT statement here...\n");
      break;
  }
};
