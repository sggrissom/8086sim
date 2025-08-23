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

u8 get_bits(u8 byte, u8 start, u8 end) {
  u8 width = end - start + 1;
  u8 mask = (1 << width) - 1;
  u8 result = (byte >> start) & mask;
  return result;
}

u8 get_bit(u8 byte, u8 num) {
  return get_bits(byte, num, num);
}

const char* get_register(u8 reg, u8 w_bit) {
  return register_file[reg + 8*w_bit];
}

void immediate_to_register_memory(char* instruction, u8 opcode_byte, MemoryReader *reader, bool use_s_bit) {
  u8 byte;
  read(reader, &byte);
  u8 w_bit = get_bit(opcode_byte , 0);
  u8 s_bit = use_s_bit ? get_bit(opcode_byte , 1) : 0;
  u8 mod = get_bits(byte, 6, 7);
  u8 rm = get_bits(byte, 0, 2);
  i16 displacement = 0;

  char source[20] = {};

  if (mod == 0b11) {
    strcpy(source, get_register(rm, w_bit));

    u16 immediate = get_immediate(w_bit && !s_bit, s_bit, reader);
    sprintf(instruction + strlen(instruction), " %s, %d", source, immediate);
    return;
  }

  if (rm == 0b110 && mod == 0b00) {
    u16 disp = get_immediate(w_bit, 0, reader);
    u16 immediate = get_immediate(w_bit && !s_bit, s_bit, reader);
    if (w_bit) {
      sprintf(instruction + strlen(instruction), " [%d], word %d", disp, immediate);
    } else {
      sprintf(instruction + strlen(instruction), " [%d], byte %d", disp, immediate);
    }
    return;
  }

  const char* address = effective_address[rm];

  if (mod == 0b01) {
    displacement = get_displacement(0, reader);
  } else if (mod == 0b10) {
    displacement = get_displacement(1, reader);
  }
  if (displacement != 0) {
    sprintf(source, "[%s + %hd]", address, displacement);
  } else {
    sprintf(source, "[%s]", address);
  }

  u16 immediate = get_immediate(w_bit && !s_bit, s_bit, reader);

  if (w_bit) {
    sprintf(instruction + strlen(instruction), " %s, word %d", source, immediate);
  } else {
    sprintf(instruction + strlen(instruction), " %s, byte %d", source, immediate);
  }
}

void memory_to_accumulator(char* instruction, u8 opcode_byte, MemoryReader *reader) {
  u8 w_bit = get_bit(opcode_byte , 0);
  u16 immediate = get_immediate(w_bit, 0, reader);
  sprintf(instruction + strlen(instruction), " ax, [%d]", immediate);
}

void accumulator_to_memory(char* instruction, u8 opcode_byte, MemoryReader *reader) {
  u8 w_bit = get_bit(opcode_byte , 0);
  u16 immediate = get_immediate(w_bit, 0, reader);
  sprintf(instruction + strlen(instruction), " [%d], ax", immediate);
}

void immediate_to_accumulator(char* instruction, u8 opcode_byte, MemoryReader *reader) {
  u8 w_bit = get_bit(opcode_byte , 0);
  u16 immediate = get_immediate(w_bit, 0, reader);
  if (w_bit) {
    sprintf(instruction + strlen(instruction), " ax, %d", immediate);
  } else {
    sprintf(instruction + strlen(instruction), " al, %d", immediate);
  }
}

void move_immediate_to_register_memory(char* instruction, u8 opcode_byte, MemoryReader *reader) {
  sprintf(instruction, "mov");
  immediate_to_register_memory(instruction, opcode_byte, reader, false);
}

void move_memory_to_accumulator(char* instruction, u8 opcode_byte, MemoryReader *reader) {
  sprintf(instruction, "mov");
  memory_to_accumulator(instruction, opcode_byte, reader);
}

void move_accumulator_to_memory(char* instruction, u8 opcode_byte, MemoryReader *reader) {
  sprintf(instruction, "mov");
  accumulator_to_memory(instruction, opcode_byte, reader);
}

void add_sub_cmp_immediate_to_register(char* instruction, u8 opcode_byte, MemoryReader *reader) {
  u8 next_byte;
  peek(reader, &next_byte);
  u8 op = get_bits(next_byte, 3, 5);

  sprintf(instruction, get_op(op));
  immediate_to_register_memory(instruction, opcode_byte, reader, true);
}

void add_sub_cmp_immediate_to_accumulator(char* instruction, u8 opcode_byte, MemoryReader *reader) {
  u8 op = get_bits(opcode_byte, 3, 5);
  sprintf(instruction, get_op(op));
  immediate_to_accumulator(instruction, opcode_byte, reader);
}

void in_fixed_port(char *instruction, u8 opcode_byte, MemoryReader *reader) {
  u8 w_bit = get_bit(opcode_byte, 0);
  u16 data = get_immediate(0, 0, reader);
  sprintf(instruction, "in %s, %hd", w_bit ? "ax" : "al", data);
}

void out_fixed_port(char *instruction, u8 opcode_byte, MemoryReader *reader) {
  u8 w_bit = get_bit(opcode_byte, 0);
  u16 data = get_immediate(0, 0, reader);
  sprintf(instruction, "out %hd, %s", data, w_bit ? "ax" : "al");
}

void out_variable_port(char *instruction, u8 opcode_byte) {
  u8 w_bit = get_bit(opcode_byte, 0);
  sprintf(instruction, "out dx, %s", w_bit ? "ax" : "al");
}

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
    if (inst.operation != "nop") {
      print_instruction(inst);
      continue;
    }

    if (get_bits(byte, 2, 7) == 0b100000) {
      add_sub_cmp_immediate_to_register(instruction, byte, &reader);
    }
    else if (get_bits(byte, 1, 7) == 0b0000010) {
      add_sub_cmp_immediate_to_accumulator(instruction, byte, &reader);
    }
    else if (get_bits(byte, 1, 7) == 0b0010110) {
      add_sub_cmp_immediate_to_accumulator(instruction, byte, &reader);
    }
    else if (get_bits(byte, 1, 7) == 0b0011110) {
      add_sub_cmp_immediate_to_accumulator(instruction, byte, &reader);
    }
    else if (get_bits(byte, 1, 7) == 0b1100011) {
      move_immediate_to_register_memory(instruction, byte, &reader);
    }
    else if (get_bits(byte, 1, 7) == 0b1010000) {
      move_memory_to_accumulator(instruction, byte, &reader);
    }
    else if (get_bits(byte, 1, 7) == 0b1010001) {
      move_accumulator_to_memory(instruction, byte, &reader);
    }

    printf("%s\n", instruction);
  }

  free(reader.memory->data);
  free(reader.memory);
  return 0;
}

