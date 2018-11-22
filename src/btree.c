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

