#include <cstdio>
#include <cstring>
#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef int8_t i8;
typedef int16_t i16;

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

const char* register_file[16] = {
  "al", "cl", "dl", "bl",
  "ah", "ch", "dh", "bh",
  "ax", "cx", "dx", "bx",
  "sp", "bp", "si", "di",
};

const char* effective_address[8] = {
  "bx + si", "bx + di", "bp + si", "bp + di",
  "si", "di", "bp", "bx",
};

const char* segment_register[4] = {
  "es", "cs", "ss", "ds"
};

const char* get_register(u8 reg, u8 w_bit) {
  return register_file[reg + 8*w_bit];
}

void print_byte(u8 byte) {
    for (int i = 7; i >= 0; i--) {
        printf("%c", (byte >> i) & 1 ? '1' : '0');
    }
}

u16 get_immediate(u8 w_bit, u8 s_bit, FILE* file) {
  u16 dest = (u8)fgetc(file);
  if (w_bit) {
    u8 next_byte = (u8)fgetc(file);
    dest = (next_byte << 8) | dest;
  } else if (s_bit && (dest & 0x80)) {
    dest |= 0xFF00;
  }
  return dest;
}

i16 get_displacement(u8 w_bit, FILE* file) {
  i16 dest = fgetc(file);
  if (w_bit) {
    i8 next_byte = fgetc(file);
    dest = (next_byte << 8) | dest;
  } else if (dest & 0x80) {
    dest |= 0xFF00;
  }
  return dest;
}

void immediate_to_register(char* instruction, u8 opcode_byte, FILE* file) {
  u8 w_bit = get_bit(opcode_byte , 3);
  u8 reg = get_bits(opcode_byte, 0, 2);

  u16 dest = get_immediate(w_bit, 0, file);
  const char* source = get_register(reg, w_bit);

  sprintf(instruction + strlen(instruction), " %s, %d", source, dest);
}

void immediate_to_register_memory(char* instruction, u8 opcode_byte, FILE* file, bool use_s_bit) {
  u8 byte = (u8)fgetc(file);
  u8 w_bit = get_bit(opcode_byte , 0);
  u8 s_bit = use_s_bit ? get_bit(opcode_byte , 1) : 0;
  u8 mod = get_bits(byte, 6, 7);
  u8 rm = get_bits(byte, 0, 2);
  i16 displacement = 0;

  char source[20] = {};

  if (mod == 0b11) {
    strcpy(source, get_register(rm, w_bit));

    u16 immediate = get_immediate(w_bit && !s_bit, s_bit, file);
    sprintf(instruction + strlen(instruction), " %s, %d", source, immediate);
    return;
  }

  if (rm == 0b110 && mod == 0b00) {
    u16 disp = get_immediate(w_bit, 0, file);
    u16 immediate = get_immediate(w_bit && !s_bit, s_bit, file);
    if (w_bit) {
      sprintf(instruction + strlen(instruction), " [%d], word %d", disp, immediate);
    } else {
      sprintf(instruction + strlen(instruction), " [%d], byte %d", disp, immediate);
    }
    return;
  }

  const char* address = effective_address[rm];

  if (mod == 0b01) {
    displacement = get_displacement(0, file);
  } else if (mod == 0b10) {
    displacement = get_displacement(1, file);
  }
  if (displacement != 0) {
    sprintf(source, "[%s + %hd]", address, displacement);
  } else {
    sprintf(source, "[%s]", address);
  }

  u16 immediate = get_immediate(w_bit && !s_bit, s_bit, file);

  if (w_bit) {
    sprintf(instruction + strlen(instruction), " %s, word %d", source, immediate);
  } else {
    sprintf(instruction + strlen(instruction), " %s, byte %d", source, immediate);
  }
}

void register_to_register(char *instruction, u8 opcode_byte, FILE* file) {
  u8 byte = (u8)fgetc(file);

  u8 w_bit = get_bit(opcode_byte , 0);
  u8 d_bit = get_bit(opcode_byte, 1);
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
    u16 immediate = get_immediate(w_bit, 0, file);
    sprintf(instruction + strlen(instruction), " %s, [%d]", source, immediate);
    return;
  } else {
    address = effective_address[rm];
  }
  if (mod == 0b01) {
    displacement = get_displacement(0, file);
  } else if (mod == 0b10) {
    displacement = get_displacement(1, file);
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

void memory_to_accumulator(char* instruction, u8 opcode_byte, FILE* file) {
  u8 w_bit = get_bit(opcode_byte , 0);
  u16 immediate = get_immediate(w_bit, 0, file);
  sprintf(instruction + strlen(instruction), " ax, [%d]", immediate);
}

void accumulator_to_memory(char* instruction, u8 opcode_byte, FILE* file) {
  u8 w_bit = get_bit(opcode_byte , 0);
  u16 immediate = get_immediate(w_bit, 0, file);
  sprintf(instruction + strlen(instruction), " [%d], ax", immediate);
}

void immediate_to_accumulator(char* instruction, u8 opcode_byte, FILE* file) {
  u8 w_bit = get_bit(opcode_byte , 0);
  u16 immediate = get_immediate(w_bit, 0, file);
  if (w_bit) {
    sprintf(instruction + strlen(instruction), " ax, %d", immediate);
  } else {
    sprintf(instruction + strlen(instruction), " al, %d", immediate);
  }
}

void move_register_to_register(char* instruction, u8 opcode_byte, FILE* file) {
  sprintf(instruction, "mov");
  register_to_register(instruction, opcode_byte, file);
}

void move_immediate_to_register(char* instruction, u8 opcode_byte, FILE* file) {
  sprintf(instruction, "mov");
  immediate_to_register(instruction, opcode_byte, file);
}

void move_immediate_to_register_memory(char* instruction, u8 opcode_byte, FILE* file) {
  sprintf(instruction, "mov");
  immediate_to_register_memory(instruction, opcode_byte, file, false);
}

void move_memory_to_accumulator(char* instruction, u8 opcode_byte, FILE* file) {
  sprintf(instruction, "mov");
  memory_to_accumulator(instruction, opcode_byte, file);
}

void move_accumulator_to_memory(char* instruction, u8 opcode_byte, FILE* file) {
  sprintf(instruction, "mov");
  accumulator_to_memory(instruction, opcode_byte, file);
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

void add_sub_cmp_register_to_register(char* instruction, u8 opcode_byte, FILE* file) {
  u8 op = get_bits(opcode_byte, 3, 5);
  sprintf(instruction, get_op(op));
  register_to_register(instruction, opcode_byte, file);
}

void add_sub_cmp_immediate_to_register(char* instruction, u8 opcode_byte, FILE* file) {
  u8 next_byte = (u8)fgetc(file);
  u8 op = get_bits(next_byte, 3, 5);
  ungetc(next_byte, file);

  sprintf(instruction, get_op(op));
  immediate_to_register_memory(instruction, opcode_byte, file, true);
}

void add_sub_cmp_immediate_to_accumulator(char* instruction, u8 opcode_byte, FILE* file) {
  u8 op = get_bits(opcode_byte, 3, 5);
  sprintf(instruction, get_op(op));
  immediate_to_accumulator(instruction, opcode_byte, file);
}

void pop_segment_register(char* instruction, u8 opcode_byte, FILE* file) {
  u8 reg = get_bits(opcode_byte, 3, 4);
  const char* address = segment_register[reg];
  sprintf(instruction, "pop %s", address);
}

void pop_register(char* instruction, u8 opcode_byte, FILE* file) {
  u8 reg = get_bits(opcode_byte, 0, 2);
  const char* address = get_register(reg, 1);
  sprintf(instruction, "pop %s", address);
}

void pop_register_memory(char* instruction, u8 opcode_byte, FILE* file) {
  sprintf(instruction, "pop");

  u8 byte = (u8)fgetc(file);
  u8 mod = get_bits(byte, 6, 7);
  u8 rm = get_bits(byte, 0, 2);

  if (rm == 0b110 && mod == 0b00) {
    u16 displacement = get_displacement(1, file);
    sprintf(instruction + strlen(instruction), " word [%d]", displacement);
    return;
  }

  char source[20] = {};
  char dest[20] = {};
  short displacement = 0;
  const char* address = effective_address[rm];

  if (mod == 0b01) {
    displacement = get_displacement(0, file);
  } else if (mod == 0b10) {
    displacement = get_displacement(1, file);
  }
  if (displacement != 0) {
    sprintf(source, "[%s + %hd]", address, displacement);
  } else {
    sprintf(source, "[%s]", address);
  }

  sprintf(instruction + strlen(instruction), " word %s", source);
}

void push_segment_register(char* instruction, u8 opcode_byte, FILE* file) {
  u8 reg = get_bits(opcode_byte, 3, 4);
  const char* address = segment_register[reg];
  sprintf(instruction, "push %s", address);
}

void push_register(char* instruction, u8 opcode_byte, FILE* file) {
  u8 reg = get_bits(opcode_byte, 0, 2);
  const char* address = get_register(reg, 1);
  sprintf(instruction, "push %s", address);
}

void push_register_memory(char* instruction, u8 opcode_byte, FILE* file) {
  sprintf(instruction, "push");

  u8 byte = (u8)fgetc(file);
  u8 mod = get_bits(byte, 6, 7);
  u8 rm = get_bits(byte, 0, 2);

  if (rm == 0b110 && mod == 0b00) {
    u16 displacement = get_displacement(1, file);
    sprintf(instruction + strlen(instruction), " word [%d]", displacement);
    return;
  }

  char source[20] = {};
  char dest[20] = {};
  short displacement = 0;
  const char* address = effective_address[rm];

  if (mod == 0b01) {
    displacement = get_displacement(0, file);
  } else if (mod == 0b10) {
    displacement = get_displacement(1, file);
  }
  if (displacement != 0) {
    sprintf(source, "[%s + %hd]", address, displacement);
  } else {
    sprintf(source, "[%s]", address);
  }

  sprintf(instruction + strlen(instruction), " word %s", source);
}

void exchange_memory_with_register(char *instruction, u8 opcode_byte, FILE* file) {
  sprintf(instruction, "xchg");
  register_to_register(instruction, opcode_byte, file);
}

void exchange_register_with_accumulator(char *instruction, u8 opcode_byte, FILE* file) {
  sprintf(instruction, "xchg");
  u8 reg = get_bits(opcode_byte, 0, 2);
  sprintf(instruction + strlen(instruction), " ax, %s", get_register(reg, 1));
}

void in_fixed_port(char *instruction, u8 opcode_byte, FILE* file) {
  u8 w_bit = get_bit(opcode_byte, 0);
  u16 data = get_immediate(0, 0, file);
  sprintf(instruction, "in %s, %hd", w_bit ? "ax" : "al", data);
}

void in_variable_port(char *instruction, u8 opcode_byte) {
  u8 w_bit = get_bit(opcode_byte, 0);
  sprintf(instruction, "in %s, dx", w_bit ? "ax" : "al");
}

void out_fixed_port(char *instruction, u8 opcode_byte, FILE* file) {
  u8 w_bit = get_bit(opcode_byte, 0);
  u16 data = get_immediate(0, 0, file);
  sprintf(instruction, "out %hd, %s", data, w_bit ? "ax" : "al");
}

void out_variable_port(char *instruction, u8 opcode_byte) {
  u8 w_bit = get_bit(opcode_byte, 0);
  sprintf(instruction, "out dx, %s", w_bit ? "ax" : "al");
}

void conditional_jump(char* instruction, const char* jump, FILE* file) {
  i8 data = fgetc(file);
  if (data+2 > 0) {
    sprintf(instruction, "%s $+%d+0", jump, data+2);
  } else if (data+2 == 0) {
    sprintf(instruction, "%s $+0", jump);
  } else {
    sprintf(instruction, "%s $%d+0", jump, data+2);
  }
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("Usage: %s <filename>\n", argv[0]);
    return 1;
  }

  const char* filename = argv[1];
  FILE* file = fopen(filename, "rb");

  if (!file) {
    perror("Error opening file");
    return 1;
  }

  printf("bits 16\n");

  i16 ch;
  char instruction[128] = {};
  while ((ch = fgetc(file)) != EOF) {
    memset(instruction, 0, sizeof(instruction));
    u8 byte = (u8)ch;

    if (byte == 0b11111111) {
      push_register_memory(instruction, byte, file);
    }
    else if (get_bits(byte, 5, 7) == 0b000 && get_bits(byte, 0, 2) == 0b110) {
      push_segment_register(instruction, byte, file);
    }
    else if (get_bits(byte, 3, 7) == 0b01010) {
      push_register(instruction, byte, file);
    }
    if (byte == 0b10001111) {
      pop_register_memory(instruction, byte, file);
    }
    else if (get_bits(byte, 5, 7) == 0b000 && get_bits(byte, 0, 2) == 0b111) {
      pop_segment_register(instruction, byte, file);
    }
    else if (get_bits(byte, 3, 7) == 0b01011) {
      pop_register(instruction, byte, file);
    }
    else if (get_bits(byte, 1, 7) == 0b1000011) {
      exchange_memory_with_register(instruction, byte, file);
    }
    else if (get_bits(byte, 3, 7) == 0b10010) {
      exchange_register_with_accumulator(instruction, byte, file);
    }
    else if (get_bits(byte, 1, 7) == 0b1110010) {
      in_fixed_port(instruction, byte, file);
    }
    else if (get_bits(byte, 1, 7) == 0b1110110) {
      in_variable_port(instruction, byte);
    }
    else if (get_bits(byte, 1, 7) == 0b1110011) {
      out_fixed_port(instruction, byte, file);
    }
    else if (get_bits(byte, 1, 7) == 0b1110111) {
      out_variable_port(instruction, byte);
    }
    else if (byte == 0b01110100) {
      conditional_jump(instruction, "je", file);
    }
    else if (byte == 0b01111100) {
      conditional_jump(instruction, "jl", file);
    }
    else if (byte == 0b01111110) {
      conditional_jump(instruction, "jle", file);
    }
    else if (byte == 0b01110010) {
      conditional_jump(instruction, "jb", file);
    }
    else if (byte == 0b01110110) {
      conditional_jump(instruction, "jbe", file);
    }
    else if (byte == 0b01111010) {
      conditional_jump(instruction, "jp", file);
    }
    else if (byte == 0b01110000) {
      conditional_jump(instruction, "jo", file);
    }
    else if (byte == 0b01111000) {
      conditional_jump(instruction, "js", file);
    }
    else if (byte == 0b01110101) {
      conditional_jump(instruction, "jne", file);
    }
    else if (byte == 0b01111101) {
      conditional_jump(instruction, "jnl", file);
    }
    else if (byte == 0b01111111) {
      conditional_jump(instruction, "jnle", file);
    }
    else if (byte == 0b01110011) {
      conditional_jump(instruction, "jnb", file);
    }
    else if (byte == 0b01110111) {
      conditional_jump(instruction, "jnbe", file);
    }
    else if (byte == 0b01111011) {
      conditional_jump(instruction, "jnp", file);
    }
    else if (byte == 0b01110001) {
      conditional_jump(instruction, "jno", file);
    }
    else if (byte == 0b01111001) {
      conditional_jump(instruction, "jns", file);
    }
    else if (byte == 0b11100010) {
      conditional_jump(instruction, "loop", file);
    }
    else if (byte == 0b11100001) {
      conditional_jump(instruction, "loopz", file);
    }
    else if (byte == 0b11100000) {
      conditional_jump(instruction, "loopnz", file);
    }
    else if (byte == 0b11100011) {
      conditional_jump(instruction, "jcxz", file);
    }
    else if (get_bits(byte, 2, 7) == 0b000000) {
      add_sub_cmp_register_to_register(instruction, byte, file);
    }
    else if (get_bits(byte, 2, 7) == 0b001010) {
      add_sub_cmp_register_to_register(instruction, byte, file);
    }
    else if (get_bits(byte, 2, 7) == 0b001110) {
      add_sub_cmp_register_to_register(instruction, byte, file);
    }
    else if (get_bits(byte, 2, 7) == 0b100000) {
      add_sub_cmp_immediate_to_register(instruction, byte, file);
    }
    else if (get_bits(byte, 1, 7) == 0b0000010) {
      add_sub_cmp_immediate_to_accumulator(instruction, byte, file);
    }
    else if (get_bits(byte, 1, 7) == 0b0010110) {
      add_sub_cmp_immediate_to_accumulator(instruction, byte, file);
    }
    else if (get_bits(byte, 1, 7) == 0b0011110) {
      add_sub_cmp_immediate_to_accumulator(instruction, byte, file);
    }
    else if (get_bits(byte, 2, 7) == 0b100010) {
      move_register_to_register(instruction, byte, file);
    }
    else if (get_bits(byte, 4, 7) == 0b1011) {
      move_immediate_to_register(instruction, byte, file);
    }
    else if (get_bits(byte, 1, 7) == 0b1100011) {
      move_immediate_to_register_memory(instruction, byte, file);
    }
    else if (get_bits(byte, 1, 7) == 0b1010000) {
      move_memory_to_accumulator(instruction, byte, file);
    }
    else if (get_bits(byte, 1, 7) == 0b1010001) {
      move_accumulator_to_memory(instruction, byte, file);
    }

    printf("%s\n", instruction);
  }

  fclose(file);
  return 0;
}

