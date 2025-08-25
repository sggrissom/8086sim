
#include <stdlib.h>

struct Memory {
  u16 byte_count;
  u8 *data;
};

struct MemoryReader {
  Memory *memory;
  u16 ip;
};

MemoryReader load_instruction_memory_from_file(const char* filename) {
  FILE *file = fopen(filename, "rb");
  MemoryReader reader = {};
  reader.memory = (Memory *)malloc(sizeof(Memory));

  if (!file) {
    perror("Error opening file");
    return reader;
  }

  fseek(file, 0, SEEK_END);
  reader.memory->byte_count = ftell(file);
  rewind(file);

  reader.memory->data = (u8 *)malloc(reader.memory->byte_count);
  if (reader.memory->data == NULL) {
    perror("Error allocating memory");
    fclose(file);
    reader.memory->byte_count = 0;
    return reader;
  }

  fread(reader.memory->data, 1, reader.memory->byte_count, file);
  fclose(file);

  reader.ip = 0;

  return reader;
}

static bool can_read(MemoryReader *r) {
  return r->ip < r->memory->byte_count;
}

static bool peek(MemoryReader *r, u8 *out) {
  if (!can_read(r)) return false;
  *out = r->memory->data[r->ip];
  return true;
}

static bool read(MemoryReader *r, u8 *out) {
  if (!peek(r, out)) return false;
  r->ip += 1;
  return true;
}
