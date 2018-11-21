/**
 * SQL
 */
#ifndef SQL_H
#define SQL_H

#include "buffer.h"
#include "db.h"


// Results
enum PrepareResult_t {
  PREPARE_SUCCESS,
  PREPARE_NEGATIVE_ID,
  PREPARE_STRING_TOO_LONG,
  PREPARE_SYNTAX_ERROR,
  PREPARE_UNRECOGNIZED
};
typedef enum PrepareResult_t PrepareResult;

enum ExecuteResult_t {
  EXECUTE_SUCCESS,
  EXECUTE_DUPLICATE_KEY,
  EXECUTE_TABLE_FULL
};
typedef enum ExecuteResult_t ExecuteResult;

// Statement
enum StatementType_t {
  STATEMENT_INSERT,
  STATEMENT_SELECT
};
typedef enum StatementType_t StatementType;

struct Statement_t {
  StatementType type;
  Row row;
};
typedef struct Statement_t Statement;

PrepareResult sql_prepare_statement(InputBuffer* buf, Statement* stmt);
ExecuteResult sql_execute_statement(Statement* stmt, Table* table);

#endif
