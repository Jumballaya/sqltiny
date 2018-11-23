/**
 * Input Buffer
 */
#include <stdlib.h>

// Input Buffer
struct InputBuffer_t {
  char*   buffer;
  size_t  buffer_len;
  ssize_t input_len;
};
typedef struct InputBuffer_t InputBuffer;

// Create new input buffer
InputBuffer* new_input_buffer();
