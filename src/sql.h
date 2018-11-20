/**
 * SQL
 */
#ifndef SQL_H
#define SQL_H

#include "buffer.h"

// Results
enum PrepareResult_t {
  PREPARE_SUCCESS,
  PREPARE_UNRECOGNIZED
};
typedef enum PrepareResult_t PrepareResult;

// Statement
enum StatementType_t {
  STATEMENT_INSERT,
  STATEMENT_SELECT
};
typedef enum StatementType_t StatementType;

struct Statement_t {
  StatementType type;
};
typedef struct Statement_t Statement;

PrepareResult sql_prepare_statement(InputBuffer* buf, Statement* stmt);
void execute_statement(Statement* stmt);

#endif
