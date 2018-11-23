/**
 * Btree
 */
#include <stdio.h>
#include <string.h>

#include "btree.h"
#include "db.h"


struct HeaderCommon_t {
  uint32_t type_size;
  uint32_t type_offset;
  uint32_t i_root_size;
  uint32_t parent_pointer_size;
  uint32_t parent_pointer_offset;
  uint8_t header_size;
};
typedef struct HeaderCommon_t HeaderCommon;

struct HeaderLeaf_t {
  uint32_t num_cells_size;
  uint32_t num_cells_offset;
  uint32_t header_size;
};
typedef struct HeaderLeaf_t HeaderLeaf;

struct BodyLeaf_t {
  uint32_t key_size;
  uint32_t key_offset;
  uint32_t value_size;
  uint32_t value_offset;
  uint32_t cell_size;
  uint32_t space_for_cells;
  uint32_t max_cells;
};
typedef struct BodyLeaf_t BodyLeaf;


NodeType btree_get_node_type(void* node) {
  uint8_t val = *((uint8_t*)(node + NODE_TYPE_OFFSET));
  return (NodeType) val;
};

void btree_set_node_type(void* node, NodeType type) {
  uint8_t val = type;
  *((uint8_t*)(node + NODE_TYPE_OFFSET)) = val;
}


// Leaf Nodes

uint32_t* btree_leaf_node_num_cells(void* node) {
  return (char *)node + LEAF_NODE_NUM_CELLS_OFFSET;
};

void* btree_leaf_node_cell(void* node, uint32_t cell_num) {
  return (char *)node + LEAF_NODE_HEADER_SIZE + cell_num * LEAF_NODE_CELL_SIZE;
};

uint32_t* btree_leaf_node_key(void* node, uint32_t cell_num) {
  return btree_leaf_node_cell(node, cell_num);
};

void* btree_leaf_node_value(void* node, uint32_t cell_num) {
  return btree_leaf_node_cell(node, cell_num) + LEAF_NODE_KEY_SIZE;
};

void btree_initialize_leaf_node(void* node) {
  btree_set_node_type(node, NODE_LEAF);
  *btree_leaf_node_num_cells(node) = 0;
};

void btree_leaf_node_insert(Cursor* cursor, uint32_t key, Row* value) {
  void* node = db_get_page(cursor->table->pager, cursor->page_num);

  uint32_t num_cells = *btree_leaf_node_num_cells(node);
  if (num_cells >= LEAF_NODE_MAX_CELLS) {
    // Node full
    printf("error: need to implement splitting a leaf node.\n");
    exit(EXIT_FAILURE);
  }

  if (cursor->cell_num < num_cells) {
    // Make room for the new cell
    for (uint32_t i = num_cells; i > cursor->cell_num; i--) {
      memcpy(btree_leaf_node_cell(node, i), btree_leaf_node_cell(node, i - 1), LEAF_NODE_CELL_SIZE);
    }
  }

  *(btree_leaf_node_num_cells(node)) += 1;
  *(btree_leaf_node_key(node, cursor->cell_num)) = key;
  db_serialize_row(value, btree_leaf_node_value(node, cursor->cell_num));
};

Cursor* btree_leaf_node_find(Table* table, uint32_t page_num, uint32_t key) {
  void* node = db_get_page(table->pager, page_num);
  uint32_t num_cells = *btree_leaf_node_num_cells(node);

  Cursor* cursor = malloc(sizeof(Cursor));
  cursor->table = table;
  cursor->page_num = page_num;

  // Binary search
  uint32_t min_index = 0;
  uint32_t one_past_max_index = num_cells;

  while (one_past_max_index != min_index) {
    uint32_t index = (min_index + one_past_max_index) / 2;
    uint32_t key_at_index = *btree_leaf_node_key(node, index);
    if (key == key_at_index) {
      cursor->cell_num = index;
      return cursor;
    }
    if (key < key_at_index) {
      one_past_max_index = index;
    } else {
      min_index = index + 1;
    }
  }

  cursor->cell_num = min_index;
  return cursor;
};
