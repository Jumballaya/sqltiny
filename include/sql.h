/**
 * SQL
 */
#ifndef SQL_H
#define SQL_H

#include "buffer.h"
#include "db.h"
#include "util.h"
#include "btree.h"

// Database Types Sizes
#define COL_VARCHAR_SIZE 139
#define COL_INT_SIZE 32
#define COL_ID_SIZE 16

// Table
#define TABLE_MAX_PAGES 100
#define PAGE_SIZE 4096

// Macros
#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)


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
