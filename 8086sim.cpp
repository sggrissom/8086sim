#include <cstdio>
#include <cstring>
#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef int8_t i8;
typedef int16_t i16;

void print_byte(u8 byte) {
    for (int i = 7; i >= 0; i--) {
        printf("%c", (byte >> i) & 1 ? '1' : '0');
    }
    printf("\n");
}

#include "memory.cpp"
#include "instructions.cpp"

void print_address(CpuInstruction inst) {
    if (inst.displacement > 0) {
      printf("[%s + %hd]", inst.dest, inst.displacement);
    } else if (inst.displacement < 0) {
      printf("[%s - %hd]", inst.dest, -inst.displacement);
    } else {
      printf("[%s]", inst.dest);
    }
}

void print_instruction(CpuInstruction inst) {
  switch (inst.type) {
    case Solo:
      {
        printf("%s\n", inst.operation);
        break;
      }
    case Register:
      {
        if (inst.segment_reg) {
          printf("%s %s\n", inst.operation, inst.segment_reg);
        }
        else if (inst.source && inst.is_accumulator) {
          const char * accumulator = inst.w_bit ? "ax" : "al";
          if (inst.d_bit) {
            printf("%s %s, %s\n", inst.operation, inst.source, accumulator);
          } else {
            printf("%s %s, %s\n", inst.operation, accumulator, inst.source);
          }
        }
        else if (inst.source) {
          printf("%s %s\n", inst.operation, inst.source);
        }
        break;
      }
    case Memory:
      {
        if (inst.displacement) {
          printf("%s %s [%d]\n", inst.operation, "word", inst.displacement);
        }
        else if (inst.address_offset != 0) {
          printf("%s word [%s + %hd]\n", inst.operation, inst.effective_address, inst.address_offset);
        }
        else {
          printf("%s word [%s]\n", inst.operation, inst.effective_address);
        }
        break;
      }
    case ConditionalJump:
      {
        i8 jump_data = (i8)inst.address_offset;
        if (jump_data+2 > 0) {
          printf("%s $+%d+0\n", inst.operation, jump_data+2);
        } else if (jump_data+2 == 0) {
          printf("%s $+0\n", inst.operation);
        } else {
          printf("%s $%d+0\n", inst.operation, jump_data+2);
        }
        break;
      }
    case Register_Memory:
      {
        if (inst.mod == 0b11) {
          printf("%s %s, %s\n", inst.operation, inst.source, inst.dest);
        } else if (inst.rm == 0b110 && inst.mod == 0b00) {
          printf("%s %s, [%d]\n", inst.operation, inst.source, inst.immediate);
        } else if (inst.d_bit == 0b0) {
          printf("%s ", inst.operation);
          print_address(inst);
          printf(", %s\n", inst.source);
        } else {
          printf("%s %s, ", inst.operation, inst.source);
          print_address(inst);
          printf("\n");
        }
        break;
      }
    case Register_Immediate:
      {
        if (inst.rm == 0b110 && inst.mod == 0b00) {
          const char * width = inst.w_bit ? "word" : "byte";
          printf("%s [%d], %s %d\n", inst.operation, inst.displacement, width, inst.immediate);
        }
        else if (inst.source) {
          if (inst.d_bit) {
            printf("%s %d, %s\n", inst.operation, inst.immediate, inst.source);
          } else {
            printf("%s %s, %d\n", inst.operation, inst.source, inst.immediate);
          }
        }
        else if (inst.effective_address) {
          char source[20] = {};
          if (inst.displacement != 0) {
            sprintf(source, "[%s + %hd]", inst.effective_address, inst.displacement);
          } else {
            sprintf(source, "[%s]", inst.effective_address);
          }
          const char * width = inst.w_bit ? "word" : "byte";
          printf("%s %s, %s %d\n", inst.operation, source, width, inst.immediate);
        } else if (inst.is_accumulator) {
          const char * accumulator = inst.w_bit ? "ax" : "al";
          printf("%s %s, %d\n", inst.operation, accumulator, inst.immediate);
        }
        break;
      }
    case Memory_Immediate:
      {
        if (inst.is_accumulator) {
          if (inst.d_bit) {
            printf("%s ax, [%d]\n", inst.operation, inst.immediate);
          } else {
            printf("%s [%d], ax\n", inst.operation, inst.immediate);
          }
        }
        break;
      }
    default:
      break;
  }
}

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

