/**
 * Database
 *
 * @TODO: Right now there is only 1 schema [id username email]. I need to make this dynamic.
 *
 *      I need to be able to serialize and deserialize the schema as a part of each table.
 *      When you read from a file it sets the schema and validates against it when you write to it.
 *
 */
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#include "db.h"
#include "btree.h"

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
void* db_get_page(Pager* pager, uint32_t page_count) {
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

    if (page_count >= pager->num_pages) {
      pager->num_pages = page_count + 1;
    };
  }

  return pager->pages[page_count];
};

// Flush the page cache to the disk
void pager_flush(Pager* pager, uint32_t page_count) {
  if (pager->pages[page_count] == NULL) {
    printf("error: attempted to flush null page\n");
    exit(EXIT_FAILURE);
  }

  off_t offset = lseek(pager->file_descriptor, page_count * PAGE_SIZE, SEEK_SET);
  if (offset == -1) {
    printf("error: problem seeking: %d\n", errno);
    exit(EXIT_FAILURE);
  }

  ssize_t bytes = write(pager->file_descriptor, pager->pages[page_count], PAGE_SIZE);

  if (bytes == -1) {
    printf("error: problem writing: %d\n", errno);
    exit(EXIT_FAILURE);
  }
};

// Close database connection
void db_close(Table* table) {
  for (uint32_t i = 0; i < table->pager->num_pages; i++) {
    if (table->pager->pages[i] == NULL) {
      continue;
    }
    pager_flush(table->pager, i);
    free(table->pager->pages[i]);
    table->pager->pages[i] = NULL;
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
  pager->num_pages = (file_len / PAGE_SIZE);

  if (file_len % PAGE_SIZE != 0) {
    printf("db file is not a whole number of pages. corrupt file.\n");
    exit(EXIT_FAILURE);
  }

  for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
    pager->pages[i] = NULL;
  }

  return pager;
}

// Create a new table
Table* db_open(const char* filename) {
  Pager* pager = pager_open(filename);
  uint32_t row_size = COL_ID_SIZE + COL_VARCHAR_SIZE + COL_VARCHAR_SIZE;

  Table* table = malloc(sizeof(Table));
  table->pager = pager;
  table->row_size = row_size;

  if (pager->num_pages == 0) {
    // New DB file, init page 0 as leaf node
    void* root_node = db_get_page(pager, 0);
    btree_initialize_leaf_node(root_node);
    btree_node_set_root(root_node, true);
  }

  return table;
};

// Table Start (create a cursor at the start of the table)
Cursor* db_table_start(Table* table) {
  Cursor* cursor = db_table_find(table, 0);

  void* node = db_get_page(table->pager, cursor->page_num);
  uint32_t num_cells = *btree_leaf_node_num_cells(node);
  cursor->end = (num_cells == 0);

  return cursor;
};

// Return the position of the given key
// if no key is present, return the position
// where is should be inserted
Cursor* db_table_find(Table* table, uint32_t key) {
  uint32_t root_page_num = table->root_page_num;
  void* root_node = db_get_page(table->pager, root_page_num);

  if (btree_get_node_type(root_node) == NODE_LEAF) {
    return btree_leaf_node_find(table, root_page_num, key);
  }

  return btree_internal_node_find(table, root_page_num, key);
}

// Get the memory position for the cursor
void* db_cursor_value(Cursor* cursor) {
  uint32_t page_num = cursor->page_num;
  void* page = db_get_page(cursor->table->pager, page_num);
  return btree_leaf_node_value(page, cursor->cell_num);
};

// Advance the cursor
void db_cursor_advance(Cursor* cursor) {
  uint32_t page_num = cursor->page_num;
  void* node = db_get_page(cursor->table->pager, page_num);
  cursor->cell_num += 1;

  if (cursor->cell_num >= (*btree_leaf_node_num_cells(node))) {
    // Advance to next leaf node
    uint32_t next_page_num = *btree_leaf_node_next_leaf(node);
    if (next_page_num == 0) {
      // Right most leaf
      cursor->end = true;
    } else {
      cursor->page_num = next_page_num;
      cursor->cell_num = 0;
    }
  }
}

// Until we start recycling free pages, new pages will always go until the end of the db file
uint32_t db_get_unused_page_num(Pager* pager) {
  return pager->num_pages;
}
