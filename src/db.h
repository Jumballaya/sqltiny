/**
 * Database
 */
#ifndef DB_H
#define DB_H

#include <stdint.h>

// Database Types Sizes
#define COL_VARCHAR_SIZE 255
#define COL_INT_SIZE 32
#define COL_ID_SIZE 16

// Table
#define TABLE_MAX_PAGES 100
#define PAGE_SIZE 4096

// Macros
#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)

// Column Type
struct ColType_t {
  char* name;
  int   size;
};
typedef struct ColType_t ColType;

// Col
struct Col_t {
  ColType type;
  char*   name;
};
typedef struct Col_t Col;

// Row
struct Row_t {
  uint32_t  id;
  char      username[COL_VARCHAR_SIZE + 1];
  char      email[COL_VARCHAR_SIZE + 1];
  //Col** columns;
};
typedef struct Row_t Row;

void db_serialize_row(Row* src, void* dest);
void db_deserialize_row(void* src, Row* dest);
void db_print_row();

// Pager
struct Pager_t {
  int file_descriptor;
  uint32_t file_len;
  void* pages[TABLE_MAX_PAGES];
};
typedef struct Pager_t Pager;

// Table
struct Table_t {
  Pager*   pager;
  uint32_t max_rows;
  uint32_t row_count;
  uint32_t row_size;
  uint32_t rows_per_page;
  //Row** rows;
};
typedef struct Table_t Table;

Table* db_new_table();
void* db_row_slot(Table* table, uint32_t row_count);


// Database
struct SQLTiny_t {
  char*   filename;
  Table** tables;
};
typedef struct SQLTiny_t SQLTiny;


#endif
