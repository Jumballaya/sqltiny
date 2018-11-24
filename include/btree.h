/**
 * B-Tree (balanced tree)
 *
 * Self balancing tree
 */
#ifndef BTREE_H
#define BTREE_H

#include <stdint.h>
#include "db.h"
#include "sql.h"

enum NodeType_t {
  NODE_INTERNAL,
  NODE_LEAF
};
typedef enum NodeType_t NodeType;


NodeType btree_get_node_type(void* node);
void btree_set_node_type(void* node, NodeType type);
void btree_node_set_root(void* node, bool is_root);

uint32_t* btree_leaf_node_num_cells(void* node);
void* btree_leaf_node_cell(void* node, uint32_t cell_num);
uint32_t* btree_leaf_node_key(void* node, uint32_t cell_num);
void* btree_leaf_node_value(void* node, uint32_t cell_num);
void btree_initialize_leaf_node(void* node);

void btree_leaf_node_insert(Cursor* cursor, uint32_t key, Row* value);
Cursor* btree_leaf_node_find(Table* table, uint32_t page_num, uint32_t key);

void btree_leaf_node_split_and_insert(Cursor* cursor, uint32_t key, Row* value);

uint32_t* btree_internal_node_num_keys(void* node);
uint32_t* btree_internal_node_right_child(void* node);
uint32_t* btree_internal_node_cell(void* node, uint32_t cell_num);
uint32_t* btree_internal_node_child(void* node, uint32_t child_num);
uint32_t* btree_internal_node_key(void* node, uint32_t key_num);
void btree_initialize_internal_node(void* node);
uint32_t btree_get_node_max_key(void* node);
void btree_create_new_root(Table* table, uint32_t right_page_num);
void btree_leaf_node_split_and_insert(Cursor* cursor, uint32_t key, Row* value);

/**
 * COMMON NODE HEADER LAYOUT
 */
#define NODE_TYPE_SIZE (uint32_t) sizeof(uint8_t)
#define NODE_TYPE_OFFSET (uint32_t) 0
#define I_ROOT_SIZE (uint32_t) sizeof(uint8_t)
#define I_ROOT_OFFSET (uint32_t) sizeof(uint8_t) // NODE_TYPE_SIZE
#define PARENT_POINTER_SIZE (uint32_t) sizeof(uint32_t)
#define PARENT_POINTER_OFFSET (uint32_t) (sizeof(uint8_t) * 2) // I_ROOT_OFFSET + I_ROOT_SIZE
#define COMMON_NODE_HEADER_SIZE (uint8_t) (sizeof(uint8_t) * 3) // NODE_TYPE_SIZE + I_ROOT_SIZE + PARENT_POINTER_SIZE

/**
 * LEAF NODE HEADER LAYOUT
 */
#define LEAF_NODE_NUM_CELLS_SIZE (uint32_t) sizeof(uint32_t)
#define LEAF_NODE_NUM_CELLS_OFFSET (uint32_t) (sizeof(uint8_t) * 3) // COMMON_NODE_HEADER_SIZE
#define LEAF_NODE_HEADER_SIZE (uint32_t) ((sizeof(uint8_t) * 3) + sizeof(uint32_t)) // COMMON_NODE_HEADER_SIZE + LEAF_NODE_NUM_CELLS_SIZE

/**
 * LEAF NODE BODY LAYOUT
 */
#define LEAF_NODE_KEY_SIZE (uint32_t) sizeof(uint32_t)
#define LEAF_NODE_KEY_OFFSET (uint32_t) 0
#define LEAF_NODE_VALUE_SIZE (uint32_t) (COL_ID_SIZE + COL_VARCHAR_SIZE + COL_VARCHAR_SIZE) // size of a row
#define LEAF_NODE_VALUE_OFFSET (uint32_t) (0 + sizeof(uint32_t)) // LEAF_NODE_KEY_OFFSET + LEAF_NODE_KEY_SIZE
#define LEAF_NODE_CELL_SIZE (uint32_t) (sizeof(uint32_t) + COL_ID_SIZE + COL_VARCHAR_SIZE + COL_VARCHAR_SIZE) // LEAF_NODE_KEY_SIZE + LEAF_NODE_VALUE_SIZE
#define LEAF_NODE_SPACE_FOR_CELLS (uint32_t) (PAGE_SIZE - ((sizeof(uint8_t) * 4) + sizeof(uint32_t))) // LEAF_NODE_HEADER_SIZE

// LEAF_NODE_SPACE_FOR_CELLS / LEAF_NODE_CELL_SIZE
#define LEAF_NODE_MAX_CELLS ((uint32_t) ((PAGE_SIZE - ((sizeof(uint8_t) * 4) + sizeof(uint32_t))) / (sizeof(uint32_t) + COL_ID_SIZE + COL_VARCHAR_SIZE + COL_VARCHAR_SIZE)))

// (LEAF_NODE_MAX_CELLS + 1) / 2
#define LEAF_NODE_RIGHT_SPLIT_COUNT ((((uint32_t) ((PAGE_SIZE - ((sizeof(uint8_t) * 4) + sizeof(uint32_t))) / (sizeof(uint32_t) + COL_ID_SIZE + COL_VARCHAR_SIZE + COL_VARCHAR_SIZE))) + 1) / 2)

// (LEAF_NODE_MAX_CELLS + 1) - LEAF_NODE_RIGHT_SPLIT_COUNT
#define LEAF_NODE_LEFT_SPLIT_COUNT (uint32_t)((((uint32_t) ((PAGE_SIZE - ((sizeof(uint8_t) * 4) + sizeof(uint32_t))) / (sizeof(uint32_t) + COL_ID_SIZE + COL_VARCHAR_SIZE + COL_VARCHAR_SIZE))) + 1) - ((((uint32_t) ((PAGE_SIZE - ((sizeof(uint8_t) * 4) + sizeof(uint32_t))) / (sizeof(uint32_t) + COL_ID_SIZE + COL_VARCHAR_SIZE + COL_VARCHAR_SIZE))) + 1) / 2))




/**************** INTERNAL NODE ******************/

/**
 * INTERNAL NODE HEADER LAYOUT
 */
#define INTERNAL_NODE_NUM_KEYS_SIZE (uint32_t) sizeof(uint32_t)
#define INTERNAL_NODE_NUM_KEYS_OFFSET (uint32_t) (sizeof(uint8_t) * 3) // COMMON_NODE_HEADER_SIZE
#define INTERNAL_NODE_RIGHT_CHILD_SIZE (uint32_t) sizeof(uint32_t)
#define INTERNAL_NODE_RIGHT_CHILD_OFFSET (uint32_t) ((uint32_t) (sizeof(uint8_t) * 3) + (uint32_t) sizeof(uint32_t)) // INTERNAL_NODE_NUM_KEYS_OFFSET + INTERNAL_NODE_NUM_KEYS_SIZE
#define INTERNAL_NODE_HEADER_SIZE (uint32_t) ((uint8_t) (sizeof(uint8_t) * 3) + (uint32_t) sizeof(uint32_t) + (uint32_t) sizeof(uint32_t)) // COMMON_NODE_HEADER_SIZE + INTERNAL_NODE_NUM_KEYS_SIZE + INTERNAL_NODE_RIGHT_CHILD_SIZE

/**
 * INTERNAL NODE BODY LAYOUT
 */
#define INTERNAL_NODE_KEY_SIZE (uint32_t) sizeof(uint32_t)
#define INTERNAL_NODE_CHILD_SIZE (uint32_t) sizeof(uint32_t)
#define INTERNAL_NODE_CELL_SIZE (uint32_t) ((uint32_t) sizeof(uint32_t) + (uint32_t) sizeof(uint32_t)) // INTERNAL_NODE_CHILD_SIZE + INTERNAL_NODE_KEY_SIZE


#endif
