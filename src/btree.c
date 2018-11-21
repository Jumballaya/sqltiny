/**
 * Btree
 */
#include <stdint.h>
#include "btree.h"
#include "sql.h"

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


/**
 * COMMON NODE HEADER LAYOUT
 */
const uint32_t NODE_TYPE_SIZE = sizeof(uint8_t);
const uint32_t NODE_TYPE_OFFSET = 0;
const uint32_t I_ROOT_SIZE = sizeof(uint8_t);
const uint32_t I_ROOT_OFFSET = sizeof(uint8_t); // NODE_TYPE_SIZE
const uint32_t PARENT_POINTER_SIZE = sizeof(uint32_t);
const uint32_t PARENT_POINTER_OFFSET = sizeof(uint8_t) * 2; // I_ROOT_OFFSET + I_ROOT_SIZE
const uint8_t COMMON_NODE_HEADER_SIZE = sizeof(uint8_t) * 4; // NODE_TYPE_SIZE + I_ROOT_SIZE + PARENT_POINTER_SIZE

/**
 * LEAF NODE HEADER LAYOUT
 */
const uint32_t LEAF_NODE_NUM_CELLS_SIZE = sizeof(uint32_t);
const uint32_t LEAF_NODE_NUM_CELLS_OFFSET = sizeof(uint8_t) * 4; // COMMON_NODE_HEADER_SIZE
const uint32_t LEAF_NODE_HEADER_SIZE = (sizeof(uint8_t) * 4) + sizeof(uint32_t); // COMMON_NODE_HEADER_SIZE + LEAF_NODE_NUM_CELLS_SIZE

/**
 * LEAF NODE BODY LAYOUT
 */
const uint32_t LEAF_NODE_KEY_SIZE = sizeof(uint32_t);
const uint32_t LEAF_NODE_KEY_OFFSET = 0;
const uint32_t LEAF_NODE_VALUE_SIZE = COL_ID_SIZE + COL_VARCHAR_SIZE + COL_VARCHAR_SIZE; // size of a row
const uint32_t LEAF_NODE_VALUE_OFFSET = 0 + sizeof(uint32_t); // LEAF_NODE_KEY_OFFSET + LEAF_NODE_KEY_SIZE
const uint32_t LEAF_NODE_CELL_SIZE = sizeof(uint32_t) + COL_ID_SIZE + COL_VARCHAR_SIZE + COL_VARCHAR_SIZE; // LEAF_NODE_KEY_SIZE + LEAF_NODE_VALUE_SIZE
const uint32_t LEAF_NODE_SPACE_FOR_CELLS = PAGE_SIZE - ((sizeof(uint8_t) * 4) + sizeof(uint32_t)); // LEAF_NODE_HEADER_SIZE
// LEAF_NODE_SPACE_FOR_CELLS / LEAF_NODE_CELL_SIZE
const uint32_t LEAF_NODE_MAX_CELLS = (PAGE_SIZE - ((sizeof(uint8_t) * 4) + sizeof(uint32_t))) / (sizeof(uint32_t) + COL_ID_SIZE + COL_VARCHAR_SIZE + COL_VARCHAR_SIZE);




// Leaf Nodes

uint32_t leaf_node_num_cells(void* node) {
  return (char *)node + LEAF_NODE_NUM_CELLS_OFFSET;
};


