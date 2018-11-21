/**
 * Database
 */
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#include "db.h"

const uint32_t ID_OFFSET = 0;

// Serialize Row
void db_serialize_row(Row* src, void* dest) {
  uint32_t USERNAME_OFFSET = ID_OFFSET + COL_ID_SIZE;
  uint32_t EMAIL_OFFSET = USERNAME_OFFSET + COL_VARCHAR_SIZE;

  memcpy(dest + ID_OFFSET, &(src->id), COL_ID_SIZE);
  memcpy(dest + USERNAME_OFFSET, &(src->username), COL_VARCHAR_SIZE);
  memcpy(dest + EMAIL_OFFSET, &(src->email), COL_VARCHAR_SIZE);
}

// Deserialize Row
void db_deserialize_row(void* src, Row* dest) {
  uint32_t USERNAME_OFFSET = ID_OFFSET + COL_ID_SIZE;
  uint32_t EMAIL_OFFSET = USERNAME_OFFSET + COL_VARCHAR_SIZE;

  memcpy(&(dest->id), src + ID_OFFSET, COL_ID_SIZE);
  memcpy(&(dest->username), src + USERNAME_OFFSET, COL_VARCHAR_SIZE);
  memcpy(&(dest->email), src + EMAIL_OFFSET, COL_VARCHAR_SIZE);
}

// Prints Row
void db_print_row(Row* row) {
  printf("(%d, %s, %s)\n", row->id, row->username, row->email);
}

// Gets page without faults
void* get_page(Pager* pager, uint32_t page_count) {
  if (page_count > TABLE_MAX_PAGES) {
    printf("error: can't fetch page number out of bounds. %d > %d\n", page_count, TABLE_MAX_PAGES);
    exit(EXIT_FAILURE);
  }

  if (pager->pages[page_count] == NULL) {
    // Cache miss. Allocate memoty and load from file
    void* page = malloc(PAGE_SIZE);
    uint32_t num_pages = pager->file_len / PAGE_SIZE;

    // We might save a partial page at the end of the file
    if (pager->file_len % PAGE_SIZE) {
      num_pages += 1;
    }

    if (page_count <= num_pages) {
      lseek(pager->file_descriptor, page_count * PAGE_SIZE, SEEK_SET);
      ssize_t bytes = read(pager->file_descriptor, page, PAGE_SIZE);
      if (bytes == -1) {
        printf("error: can't read file: %d\n", errno);
        exit(EXIT_FAILURE);
      }
    }

    pager->pages[page_count] = page;
  }

  return pager->pages[page_count];
};

// Flush the page cache to the disk
void pager_flush(Pager* pager, uint32_t page_count, uint32_t size) {
  if (pager->pages[page_count] == NULL) {
    printf("error: attempted to flush null page\n");
    exit(EXIT_FAILURE);
  }

  off_t offset = lseek(pager->file_descriptor, page_count * PAGE_SIZE, SEEK_SET);
  if (offset == -1) {
    printf("error: problem seeking: %d\n", errno);
    exit(EXIT_FAILURE);
  }

  ssize_t bytes = write(pager->file_descriptor, pager->pages[page_count], size);

  if (bytes == -1) {
    printf("error: problem writing: %d\n", errno);
    exit(EXIT_FAILURE);
  }
};

// Close database connection
void db_close(Table* table) {
  uint32_t num_full_pages = table->row_count / table->rows_per_page;

  for (uint32_t i = 0; i < num_full_pages; i++) {
    if (table->pager->pages[i] == NULL) {
      continue;
    }
    pager_flush(table->pager, i, PAGE_SIZE);
    free(table->pager->pages[i]);
    table->pager->pages[i] = NULL;
  }

  uint32_t additional_rows = table->row_count % table->rows_per_page;
  if (additional_rows > 0) {
    uint32_t page_num = num_full_pages;
    if (table->pager->pages[page_num] != NULL) {
      pager_flush(table->pager, page_num, additional_rows * table->row_size);
      free(table->pager->pages[page_num]);
      table->pager->pages[page_num] = NULL;
    }
  }

  int result = close(table->pager->file_descriptor);
  if (result == -1) {
    printf("error: issues closing the db file\n");
    exit(EXIT_FAILURE);
  }

  for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
    void* page = table->pager->pages[i];
    if (page) {
      free(page);
      table->pager->pages[i] = NULL;
    }
  }
  free(table->pager);
}

// Open a new pager
Pager* pager_open(const char* filename) {
  // Open file in read/write mode, create if not exists, user read/write permissions
  int fd = open(filename, O_RDWR | O_CREAT, S_IWUSR | S_IRUSR);

  if (fd == -1) {
    printf("error: unable to open file\n");
    exit(EXIT_FAILURE);
  }

  off_t file_len = lseek(fd, 0, SEEK_END);
  Pager* pager = malloc(sizeof(Pager));
  pager->file_descriptor = fd;
  pager->file_len = file_len;

  for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
    pager->pages[i] = NULL;
  }

  return pager;
}

// Create a new table
Table* db_open(const char* filename) {
  Pager* pager = pager_open(filename);
  uint32_t row_size = COL_ID_SIZE + COL_VARCHAR_SIZE + COL_VARCHAR_SIZE;
  uint32_t num_rows = pager->file_len / row_size;

  Table* table = malloc(sizeof(Table));
  table->row_count = num_rows;
  table->row_size = row_size;
  table->rows_per_page = PAGE_SIZE / table->row_size;
  table->max_rows = table->rows_per_page * TABLE_MAX_PAGES;
  table->pager = pager;

  return table;
};

// Table Start (create a cursor at the start of the table)
Cursor* db_table_start(Table* table) {
  Cursor* cursor = malloc(sizeof(Cursor));
  cursor->table = table;
  cursor->row_num = 0;
  cursor->end = (table->row_count == 0);
  return cursor;
};

// Table End (create a cursor at the end of the table)
Cursor* db_table_end(Table* table) {
  Cursor* cursor = malloc(sizeof(Cursor));
  cursor->table = table;
  cursor->row_num = table->row_count;
  cursor->end = true;
  return cursor;
};

// Get the memory position for the cursor
void* db_cursor_value(Cursor* cursor) {
  uint32_t row_num = cursor->row_num;
  uint32_t page_num = row_num / cursor->table->rows_per_page;
  void* page = get_page(cursor->table->pager, page_num);
  uint32_t row_offset = row_num % cursor->table->rows_per_page;
  uint32_t byte_offset = row_offset * cursor->table->row_size;
  return page + byte_offset;
};

// Advance the cursor
void db_cursor_advance(Cursor* cursor) {
  cursor->row_num += 1;
  if (cursor->row_num >= cursor->table->row_count) {
    cursor->end = true;
  }
}
