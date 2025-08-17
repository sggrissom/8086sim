#include <cstdio>
#include <cstring>
#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef int8_t i8;
typedef int16_t i16;

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

const char* opcode_instruction[3] = {
  "add", "sub", "cmp",
};

const char* get_register(u8 reg, u8 w_bit) {
  return register_file[reg + 8*w_bit];
}

void print_byte(u8 byte) {
    for (int i = 7; i >= 0; i--) {
        printf("%c", (byte >> i) & 1 ? '1' : '0');
    }
}

u16 get_immediate(u8 w_bit, u8 s_bit, MemoryReader *reader) {
  u8 byte;
  read(reader, &byte);
  u16 dest = (u16)byte;
  if (w_bit) {
    u8 next_byte;
    read(reader, &next_byte);
    dest = (next_byte << 8) | dest;
  } else if (s_bit && (dest & 0x80)) {
    dest |= 0xFF00;
  }
  return dest;
}

i16 get_displacement(u8 w_bit, MemoryReader *reader) {
  u8 byte;
  read(reader, &byte);
  i16 dest = (u16)byte;
  if (w_bit) {
    u8 next_byte;
    read(reader, &next_byte);
    dest = ((i8)next_byte << 8) | dest;
  } else if (dest & 0x80) {
    dest |= 0xFF00;
  }
  return dest;
}

void immediate_to_register(char* instruction, u8 opcode_byte, MemoryReader *reader) {
  u8 w_bit = get_bit(opcode_byte , 3);
  u8 reg = get_bits(opcode_byte, 0, 2);

  u16 dest = get_immediate(w_bit, 0, reader);
  const char* source = get_register(reg, w_bit);

  sprintf(instruction + strlen(instruction), " %s, %d", source, dest);
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

void register_to_register(char *instruction, u8 opcode_byte, MemoryReader *reader, bool use_d_bit, bool use_w_bit) {
  u8 byte;
  read(reader, &byte);

  u8 w_bit = use_w_bit ? get_bit(opcode_byte , 0) : 1;
  u8 d_bit = use_d_bit ? get_bit(opcode_byte, 1) : 1;
  u8 mod = get_bits(byte, 6, 7);
  u8 reg = get_bits(byte, 3, 5);
  u8 rm = get_bits(byte, 0, 2);

  char source[20] = {};
  char dest[20] = {};
  short displacement = 0;

  if (mod == 0b11) {
    strcpy(source, get_register(rm, w_bit));
    strcpy(dest, get_register(reg, w_bit));
    if (d_bit == 0b0) {
      sprintf(instruction + strlen(instruction), " %s, %s", source, dest);
    } else {
      sprintf(instruction + strlen(instruction), " %s, %s", dest, source);
    }
    return;
  }

  const char* address;

  strcpy(source, get_register(reg, w_bit));
  if (rm == 0b110 && mod == 0b00) {
    u16 immediate = get_immediate(w_bit, 0, reader);
    sprintf(instruction + strlen(instruction), " %s, [%d]", source, immediate);
    return;
  } else {
    address = effective_address[rm];
  }
  if (mod == 0b01) {
    displacement = get_displacement(0, reader);
  } else if (mod == 0b10) {
    displacement = get_displacement(1, reader);
  }
  if (displacement > 0) {
    sprintf(dest, "[%s + %hd]", address, displacement);
  } else if (displacement < 0) {
    sprintf(dest, "[%s - %hd]", address, -displacement);
  } else {
    sprintf(dest, "[%s]", address);
  }

  if (d_bit == 0b0) {
    sprintf(instruction + strlen(instruction), " %s, %s", dest, source);
  } else {
    sprintf(instruction + strlen(instruction), " %s, %s", source, dest);
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

void move_register_to_register(char* instruction, u8 opcode_byte, MemoryReader *reader) {
  sprintf(instruction, "mov");
  register_to_register(instruction, opcode_byte, reader, true, true);
}

void move_immediate_to_register(char* instruction, u8 opcode_byte, MemoryReader *reader) {
  sprintf(instruction, "mov");
  immediate_to_register(instruction, opcode_byte, reader);
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

const char * get_op(u8 op) {
  if (op == 0b000) {
    return opcode_instruction[0];
  }
  else if (op == 0b101) {
    return opcode_instruction[1];
  }
  else if (op == 0b111) {
    return opcode_instruction[2];
  }
  return "";
}

void add_sub_cmp_register_to_register(char* instruction, u8 opcode_byte, MemoryReader *reader) {
  u8 op = get_bits(opcode_byte, 3, 5);
  sprintf(instruction, get_op(op));
  register_to_register(instruction, opcode_byte, reader, true, true);
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

void exchange_memory_with_register(char *instruction, u8 opcode_byte, MemoryReader *reader) {
  sprintf(instruction, "xchg");
  register_to_register(instruction, opcode_byte, reader, true, true);
}

void lea_memory_with_register(char *instruction, u8 opcode_byte, MemoryReader *reader) {
  sprintf(instruction, "lea");
  register_to_register(instruction, opcode_byte, reader, false, true);
}

void lds_memory_with_register(char *instruction, u8 opcode_byte, MemoryReader *reader) {
  sprintf(instruction, "lds");
  register_to_register(instruction, opcode_byte, reader, false, true);
}

void les_memory_with_register(char *instruction, u8 opcode_byte, MemoryReader *reader) {
  sprintf(instruction, "les");
  register_to_register(instruction, opcode_byte, reader, false, false);
}

void in_fixed_port(char *instruction, u8 opcode_byte, MemoryReader *reader) {
  u8 w_bit = get_bit(opcode_byte, 0);
  u16 data = get_immediate(0, 0, reader);
  sprintf(instruction, "in %s, %hd", w_bit ? "ax" : "al", data);
}

void in_variable_port(char *instruction, u8 opcode_byte) {
  u8 w_bit = get_bit(opcode_byte, 0);
  sprintf(instruction, "in %s, dx", w_bit ? "ax" : "al");
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
        else if (inst.reg && inst.is_accumulator) {
          printf("%s ax, %s\n", inst.operation, inst.reg);
        }
        else if (inst.reg) {
          printf("%s %s\n", inst.operation, inst.reg);
        }
        break;
      }
    case Memory:
      {
        if (inst.displacement) {
          printf("%s %s [%d]\n", inst.operation, "word", inst.displacement);
        }
        else if (inst.effective_address && inst.address_offset != 0) {
          printf("%s word [%s + %hd]\n", inst.operation, inst.effective_address, inst.address_offset);
        }
        else if (inst.effective_address) {
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

    if (get_bits(byte, 1, 7) == 0b1000011) {
      exchange_memory_with_register(instruction, byte, &reader);
    }
    else if (get_bits(byte, 1, 7) == 0b1110010) {
      in_fixed_port(instruction, byte, &reader);
    }
    else if (get_bits(byte, 1, 7) == 0b1110110) {
      in_variable_port(instruction, byte);
    }
    else if (get_bits(byte, 1, 7) == 0b1110011) {
      out_fixed_port(instruction, byte, &reader);
    }
    else if (get_bits(byte, 1, 7) == 0b1110111) {
      out_variable_port(instruction, byte);
    }
    else if (byte == 0b10001101) {
      lea_memory_with_register(instruction, byte, &reader);
    }
    else if (byte == 0b11000101) {
      lds_memory_with_register(instruction, byte, &reader);
    }
    else if (byte == 0b11000100) {
      les_memory_with_register(instruction, byte, &reader);
    }
    else if (get_bits(byte, 2, 7) == 0b000000) {
      add_sub_cmp_register_to_register(instruction, byte, &reader);
    }
    else if (get_bits(byte, 2, 7) == 0b001010) {
      add_sub_cmp_register_to_register(instruction, byte, &reader);
    }
    else if (get_bits(byte, 2, 7) == 0b001110) {
      add_sub_cmp_register_to_register(instruction, byte, &reader);
    }
    else if (get_bits(byte, 2, 7) == 0b100000) {
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
    else if (get_bits(byte, 2, 7) == 0b100010) {
      move_register_to_register(instruction, byte, &reader);
    }
    else if (get_bits(byte, 4, 7) == 0b1011) {
      move_immediate_to_register(instruction, byte, &reader);
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

