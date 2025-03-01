#include "util.h"


#include <stdio.h>
#include <stdlib.h>


uint8_t ReadFile(const char *const path, char ** buffer) {
  FILE *file;
  size_t file_size;

  // Open file using standard lib functions
  file = fopen(path, "rb");

  if (NULL == file)
    return 1;

  // Get file size
  fseek(file, 0, SEEK_END);
  file_size = ftell(file);
  rewind(file);

  // Allocate buffer with extra byte for null terminator
  *buffer = malloc(file_size + 1);

  if (NULL == buffer)
    return 2;

  // Append null terminator
  buffer[0][file_size] = '\0';

  // Read file into allocated buffer
  size_t ret = fread(*buffer, 1, file_size, file);

  if (ret != file_size)
    return 3;

  // Clean up and exit
  fclose(file);
  return 0;
}