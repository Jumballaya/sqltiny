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
  void* node = db_get_page(table->pager, table->root_page_num);
  uint32_t num_cells = (*btree_leaf_node_num_cells(node));
  if (num_cells >= LEAF_NODE_MAX_CELLS) {
    return EXECUTE_TABLE_FULL;
  }

  Row* row = &(stmt->row);
  uint32_t key = row->id;
  Cursor* cursor = db_table_find(table, key);

  if (cursor->cell_num < num_cells) {
    uint32_t key_at_index = *btree_leaf_node_key(node, cursor->cell_num);
    if (key_at_index == key) {
      return EXECUTE_DUPLICATE_KEY;
    }
  }

  btree_leaf_node_insert(cursor, row->id, row);

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
