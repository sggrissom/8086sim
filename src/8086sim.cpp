#include <cstdio>
#include <cstring>
#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef int8_t i8;
typedef int16_t i16;

#include "memory.cpp"
#include "instructions.cpp"
#include "print.cpp"

int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("Usage: %s <filename>\n", argv[0]);
    return 1;
  }

  MemoryReader reader = load_instruction_memory_from_file(argv[1]);
  if (reader.memory->byte_count == 0) {
    return 1;
  }

  printf("bits 16\n");

  u8 byte;
  char instruction[128] = {};
  while (read(&reader, &byte)) {
    memset(instruction, 0, sizeof(instruction));
    CpuInstruction inst = decode_instruction(byte, &reader);
    print_instruction(inst);
  }

  free(reader.memory->data);
  free(reader.memory);
  return 0;
}

