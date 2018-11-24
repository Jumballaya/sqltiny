/**
 * Database
 */
#ifndef DB_H
#define DB_H

#include <stdint.h>
#include <stdbool.h>

// Database Types Sizes
#define COL_VARCHAR_SIZE 139
#define COL_INT_SIZE 32
#define COL_ID_SIZE 16

// Table
#define TABLE_MAX_PAGES 100
#define PAGE_SIZE 4096

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
  char      username[33];
  char      email[256];
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
  uint32_t num_pages;
  void* pages[TABLE_MAX_PAGES];
};
typedef struct Pager_t Pager;

void* db_get_page(Pager* pager, uint32_t page_count);
uint32_t db_get_unused_page_num(Pager* pager);

// Table
struct Table_t {
  Pager*   pager;
  uint32_t root_page_num;
  uint32_t row_count;
  uint32_t row_size;
};
typedef struct Table_t Table;

Table* db_open(const char* filename);
void db_close(Table* table);

// Cursor
struct Cursor_t {
  Table* table;
  uint32_t page_num;
  uint32_t cell_num;
  bool end; // Indicates a position 1 past the last element
};
typedef struct Cursor_t Cursor;

void* db_cursor_value(Cursor* cursor);
Cursor* db_table_start(Table* table);
Cursor* db_table_find(Table* table, uint32_t key);
void db_cursor_advance(Cursor* cursor);



// Database
struct SQLTiny_t {
  char*   filename;
  Table** tables;
};
typedef struct SQLTiny_t SQLTiny;


#endif
