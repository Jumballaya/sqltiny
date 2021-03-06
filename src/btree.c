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

bool is_root(void* node) {
  uint8_t value = *((uint8_t*)(node + I_ROOT_OFFSET));
  return (bool)value;
}

void btree_node_set_root(void* node, bool is_root) {
  uint8_t value = is_root;
  *((uint8_t*)(node + I_ROOT_OFFSET)) = value;
};


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

uint32_t* btree_leaf_node_next_leaf(void* node) {
  return node + LEAF_NODE_NEXT_LEAF_OFFSET;
}

void btree_initialize_leaf_node(void* node) {
  btree_set_node_type(node, NODE_LEAF);
  btree_node_set_root(node, false);
  *btree_leaf_node_num_cells(node) = 0;
  *btree_leaf_node_next_leaf(node) = 0; // 0 means no sibling
};

void btree_leaf_node_insert(Cursor* cursor, uint32_t key, Row* value) {
  void* node = db_get_page(cursor->table->pager, cursor->page_num);

  uint32_t num_cells = *btree_leaf_node_num_cells(node);
  if (num_cells >= LEAF_NODE_MAX_CELLS) {
    // Node full
    btree_leaf_node_split_and_insert(cursor, key, value);
    return;
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

uint32_t* btree_internal_node_num_keys(void* node) {
  return node + INTERNAL_NODE_NUM_KEYS_OFFSET;
}

uint32_t* btree_internal_node_right_child(void* node) {
  return node + INTERNAL_NODE_RIGHT_CHILD_OFFSET;
}

uint32_t* btree_internal_node_cell(void* node, uint32_t cell_num) {
  return node + INTERNAL_NODE_HEADER_SIZE + cell_num * INTERNAL_NODE_CELL_SIZE;
}

uint32_t* btree_internal_node_child(void* node, uint32_t child_num) {
  uint32_t num_keys = *btree_internal_node_num_keys(node);
  if (child_num > num_keys) {
    printf("error: tried to access child %d outside bounds of keys %d\n", child_num, num_keys);
    exit(EXIT_FAILURE);
  } else if (child_num == num_keys) {
    return btree_internal_node_right_child(node);
  } else {
    return btree_internal_node_cell(node, child_num);
  }
}

uint32_t* btree_internal_node_key(void* node, uint32_t key_num) {
  return (void*)btree_internal_node_cell(node, key_num) + INTERNAL_NODE_CHILD_SIZE;
}

void btree_initialize_internal_node(void* node) {
  btree_set_node_type(node, NODE_INTERNAL);
  btree_node_set_root(node, false);
  *btree_internal_node_num_keys(node) = 0;
}

void btree_update_internal_node_key(void* node, uint32_t old_key, uint32_t new_key) {
  uint32_t old_child_index = btree_internal_node_find_child(node, old_key);
  *btree_internal_node_key(node, old_child_index) = new_key;
}

uint32_t btree_get_node_max_key(void* node) {
  switch (btree_get_node_type(node)) {
    case NODE_INTERNAL:
      return *btree_internal_node_key(node, *btree_internal_node_num_keys(node) - 1);
    case NODE_LEAF:
      return *btree_leaf_node_key(node, *btree_leaf_node_num_cells(node) - 1);
  }
}

uint32_t* btree_node_parent(void* node) {
  return node + PARENT_POINTER_OFFSET;
}

void btree_create_new_root(Table* table, uint32_t right_page_num) {
  // Old root is copied to new page, becomes the left child
  // address of the right child is passed in and re-init as root page to contain the
  // new root node. New root points to 2 children
  void* root = db_get_page(table->pager, table->root_page_num);
  void* right_child = db_get_page(table->pager, right_page_num);
  uint32_t left_page_num = db_get_unused_page_num(table->pager);
  void* left_child = db_get_page(table->pager, left_page_num);

  // Old root is copied to the left child
  memcpy(left_child, root, PAGE_SIZE);
  btree_node_set_root(left_child, false);

  // Root node is a new internal node with one key and two children
  btree_initialize_internal_node(root);
  btree_node_set_root(root, true);
  *btree_internal_node_num_keys(root) = 1;
  *btree_internal_node_child(root, 0) = left_page_num;
  uint32_t left_max_key = btree_get_node_max_key(left_child);
  *btree_internal_node_key(root, 0) = left_max_key;
  *btree_internal_node_right_child(root) = right_page_num;
  *btree_node_parent(left_child) = table->root_page_num;
  *btree_node_parent(right_child) = table->root_page_num;
}

void btree_leaf_node_split_and_insert(Cursor* cursor, uint32_t key, Row* value) {
  // Create a new node and move half the cells over
  // Insert the new value in one of the two nodes
  // Update the parent or create a new parent
  void* old_node = db_get_page(cursor->table->pager, cursor->page_num);
  uint32_t old_max = btree_get_node_max_key(old_node);
  uint32_t new_page_num = db_get_unused_page_num(cursor->table->pager);
  void* new_node = db_get_page(cursor->table->pager, new_page_num);
  btree_initialize_leaf_node(new_node);
  *btree_node_parent(new_node) = *btree_node_parent(old_node);
  *btree_leaf_node_next_leaf(new_node) = *btree_leaf_node_next_leaf(old_node);
  *btree_leaf_node_next_leaf(old_node) = new_page_num;

  // All existing keys plus new key should be divided evenly between
  // old (left) and new (right). Starting from the right, move each key to
  // the correct spot.
  for (int32_t i = LEAF_NODE_MAX_CELLS; i >= 0; i--) {
    void* dest_node;
    if (i >= LEAF_NODE_LEFT_SPLIT_COUNT) {
      dest_node = new_node;
    } else {
      dest_node = old_node;
    }
    uint32_t index_in_node = i % LEAF_NODE_LEFT_SPLIT_COUNT;
    void* dest = btree_leaf_node_cell(dest_node, index_in_node);

    if (i == cursor->cell_num) {
      db_serialize_row(value, btree_leaf_node_value(dest_node, index_in_node));
      *btree_leaf_node_key(dest_node, index_in_node) = key;
    } else if (i > cursor->cell_num) {
      memcpy(dest, btree_leaf_node_cell(old_node, i - 1), LEAF_NODE_CELL_SIZE);
    } else {
      memcpy(dest, btree_leaf_node_cell(old_node, i), LEAF_NODE_CELL_SIZE);
    }
  }

  // Update cell count on both leaf nodes
  *(btree_leaf_node_num_cells(old_node)) = LEAF_NODE_LEFT_SPLIT_COUNT;
  *(btree_leaf_node_num_cells(new_node)) = LEAF_NODE_RIGHT_SPLIT_COUNT;

  if (is_root(old_node)) {
    return btree_create_new_root(cursor->table, new_page_num);
  } else {
    uint32_t parent_page_num = *btree_node_parent(old_node);
    uint32_t new_max = btree_get_node_max_key(old_node);
    void* parent = db_get_page(cursor->table->pager, parent_page_num);
    btree_update_internal_node_key(parent, old_max, new_max);
    btree_internal_node_insert(cursor->table, parent_page_num, new_page_num);
    return;
  }
}

uint32_t btree_internal_node_find_child(void* node, uint32_t key) {
  // Return the index of the child which should contain the given key
  uint32_t num_keys = *btree_internal_node_num_keys(node);

  // Binary search
  uint32_t min_i = 0;
  uint32_t max_i = num_keys;

  while (min_i != max_i) {
    uint32_t i = (min_i + max_i) / 2;
    uint32_t right_key = *btree_internal_node_key(node, i);
    if (right_key >= key) {
      max_i = i;
    } else {
      min_i = i + 1;
    }
  }

  return min_i;
}

Cursor* btree_internal_node_find(Table* table, uint32_t page_num, uint32_t key) {
  void* node = db_get_page(table->pager, page_num);

  uint32_t child_index = btree_internal_node_find_child(node, key);
  uint32_t child_num = *btree_internal_node_child(node, child_index);
  void* child = db_get_page(table->pager, child_num);
  switch (btree_get_node_type(child)) {
    case NODE_LEAF:
      return btree_leaf_node_find(table, child_num, key);
    case NODE_INTERNAL:
      return btree_internal_node_find(table, child_num, key);
  }
}

void btree_internal_node_insert(Table* table, uint32_t parent_page_num, uint32_t child_page_num) {
  // Add a new child/key pair to the parent that corresponds to the child
  void* parent = db_get_page(table->pager, parent_page_num);
  void* child = db_get_page(table->pager, child_page_num);
  uint32_t child_max_key = btree_get_node_max_key(child);
  uint32_t index = btree_internal_node_find_child(parent, child_max_key);

  uint32_t og_num_keys = *btree_internal_node_num_keys(parent);
  *btree_internal_node_num_keys(parent) = og_num_keys + 1;

  if (og_num_keys >= INTERNAL_NODE_MAX_CELLS) {
    printf("Need to implement splitting internal node\n");
    exit(EXIT_FAILURE);
  }

  uint32_t right_child_page_num = *btree_internal_node_right_child(parent);
  void* right_child = db_get_page(table->pager, right_child_page_num);


  if (child_max_key > btree_get_node_max_key(right_child)) {
    // replace right child
    *btree_internal_node_child(parent, og_num_keys) = right_child_page_num;
    *btree_internal_node_key(parent, og_num_keys) = btree_get_node_max_key(right_child);
    *btree_internal_node_right_child(parent) = child_page_num;
  } else {
    // Make room for new cell
    for (uint32_t i = og_num_keys; i > index; i--) {
      void* dest = btree_internal_node_cell(parent, i);
      void* src = btree_internal_node_cell(parent, i - 1);
      memcpy(dest, src, INTERNAL_NODE_CELL_SIZE);
    }
    *btree_internal_node_child(parent, index) = child_page_num;
    *btree_internal_node_key(parent, index) = child_max_key;
  }
}
