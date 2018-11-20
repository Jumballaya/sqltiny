/**
 * Database
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "db.h"

const uint32_t ID_OFFSET = 0;
const uint32_t ID_SIZE = size_of_attribute(Row, id);
const uint32_t USERNAME_SIZE = size_of_attribute(Row, username);
const uint32_t EMAIL_SIZE = size_of_attribute(Row, email);

// Serialize Row
void db_serialize_row(Row* src, void* dest) {
  uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
  uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;

  memcpy(dest + ID_OFFSET, &(src->id), ID_SIZE);
  memcpy(dest + USERNAME_OFFSET, &(src->username), USERNAME_SIZE);
  memcpy(dest + EMAIL_OFFSET, &(src->email), EMAIL_SIZE);
}

// Deserialize Row
void db_deserialize_row(void* src, Row* dest) {
  uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
  uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;

  memcpy(&(dest->id), src + ID_OFFSET, ID_SIZE);
  memcpy(&(dest->username), src + USERNAME_OFFSET, USERNAME_SIZE);
  memcpy(&(dest->email), src + EMAIL_OFFSET, EMAIL_SIZE);
}

// Prints Row
void db_print_row(Row* row) {
  printf("(%d, %s, %s)\n", row->id, row->username, row->email);
}

// Get the memory slot for our row
void* db_row_slot(Table* table, uint32_t row_count) {
  uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;
  uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;

  uint32_t page_count = row_count / ROWS_PER_PAGE;
  void* page = table->pages[page_count];
  if (!page) {
    // Allocate memory only when we try to access page
    page = table->pages[page_count] = malloc(PAGE_SIZE);
  }

  uint32_t row_offset = row_count % ROWS_PER_PAGE;
  uint32_t byte_offset = row_offset * ROW_SIZE;
  return page + byte_offset;
};

// Create a new table
Table* db_new_table() {
  Table* table = malloc(sizeof(Table));
  table->row_count = 0;
  table->row_size = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;
  table->rows_per_page = PAGE_SIZE / table->row_size;
  table->max_rows = table->rows_per_page * TABLE_MAX_PAGES;
  return table;
};
