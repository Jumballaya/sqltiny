/**
 * SQL
 */
#include <stdio.h>
#include <string.h>

#include "sql.h"

PrepareResult sql_prepare_insert(InputBuffer* buf, Statement* stmt) {
  stmt->type = STATEMENT_INSERT;

  strtok(buf->buffer, " ");
  char* id_str = strtok(NULL, " ");
  char* username = strtok(NULL, " ");
  char* email = strtok(NULL, " ");

  if (id_str == NULL || username == NULL || email == NULL) {
    return PREPARE_SYNTAX_ERROR;
  }

  int id = atoi(id_str);
  if (id < 0) {
    return PREPARE_NEGATIVE_ID;
  }

  if (strlen(username) > COL_VARCHAR_SIZE) {
    return PREPARE_STRING_TOO_LONG;
  }

  if (strlen(email) > COL_VARCHAR_SIZE) {
    return PREPARE_STRING_TOO_LONG;
  }

  stmt->row.id = id;
  strcpy(stmt->row.username, username);
  strcpy(stmt->row.email, email);

  return PREPARE_SUCCESS;
}

// Prepares the SQL statement from the buffer
PrepareResult sql_prepare_statement(InputBuffer* buf, Statement* stmt) {
  // INSERT statement
  if (strncmp(buf->buffer, "insert", 6) == 0) {
    return sql_prepare_insert(buf, stmt);
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
  Cursor* cursor = db_table_end(table);

  db_serialize_row(row_to_insert, db_cursor_value(cursor));
  table->row_count += 1;
  free(cursor);
  return EXECUTE_SUCCESS;
}

// Execute Select Statement
ExecuteResult sql_execute_select(Statement* stmt, Table* table) {
  Cursor* cursor = db_table_start(table);
  Row row;

  while(!(cursor->end)) {
    db_deserialize_row(db_cursor_value(cursor), &row);
    db_print_row(&row);
    db_cursor_advance(cursor);
  }

  free(cursor);
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
