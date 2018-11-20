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
    int args_assigned = sscanf(buf->buffer, "insert %d %s %s", &(stmt->row.id), stmt->row.username, stmt->row.email);
    if (args_assigned < 3) {
      return PREPARE_SYNTAX_ERROR;
    }
    return PREPARE_SUCCESS;
  }

  // SELECT statement
  if (strcmp(buf->buffer, "select") == 0) {
    stmt->type = STATEMENT_SELECT;
    return PREPARE_SUCCESS;
  }

  return PREPARE_UNRECOGNIZED;
};

// Execute Insert Statement
ExecuteResult sql_execute_insert(Statement* stmt, Table* table) {
  if (table->row_count >= table->max_rows) {
    return EXECUTE_TABLE_FULL;
  }
  Row* row_to_insert = &(stmt->row);

  db_serialize_row(row_to_insert, db_row_slot(table, table->row_count));
  table->row_count += 1;
  return EXECUTE_SUCCESS;
}

// Execute Select Statement
ExecuteResult sql_execute_select(Statement* stmt, Table* table) {
  Row row;
  for (uint32_t i = 0; i < table->row_count; i++) {
    db_deserialize_row(db_row_slot(table, i), &row);
    db_print_row(&row);
  }
  return EXECUTE_SUCCESS;
}


// Executes the SQL statement
ExecuteResult sql_execute_statement(Statement* stmt, Table* table) {
  switch (stmt->type) {
    case (STATEMENT_INSERT):
      return sql_execute_insert(stmt, table);
    case (STATEMENT_SELECT):
      return sql_execute_select(stmt, table);
    default:
      return EXECUTE_TABLE_FULL;
  }
};
