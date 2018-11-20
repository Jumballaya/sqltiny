/**
 * Input Buffer
 */
#include "buffer.h"

// Creates a new input buffer
InputBuffer* new_input_buffer() {
  InputBuffer* buf = malloc(sizeof(InputBuffer));

  buf->buffer = NULL;
  buf->buffer_len = 0;
  buf->input_len = 0;

  return buf;
}
