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

void immediate_to_register_memory(char* instruction, u8 opcode_byte, FILE* file) {
  u8 byte = (u8)fgetc(file);
  u8 w_bit = get_bit(opcode_byte , 0);
  u8 s_bit = get_bit(opcode_byte , 1);
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

  u16 immediate = get_immediate(w_bit, s_bit, file);

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
    sprintf(instruction + strlen(instruction), " %s, %s", source, dest);
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
  immediate_to_register_memory(instruction, opcode_byte, file);
}

void move_memory_to_accumulator(char* instruction, u8 opcode_byte, FILE* file) {
  sprintf(instruction, "mov");
  memory_to_accumulator(instruction, opcode_byte, file);
}

void move_accumulator_to_memory(char* instruction, u8 opcode_byte, FILE* file) {
  sprintf(instruction, "mov");
  accumulator_to_memory(instruction, opcode_byte, file);
}

void add_register_to_register(char* instruction, u8 opcode_byte, FILE* file) {
  sprintf(instruction, "add");
  register_to_register(instruction, opcode_byte, file);
}

void add_immediate_to_register(char* instruction, u8 opcode_byte, FILE* file) {
  sprintf(instruction, "add");
  immediate_to_register_memory(instruction, opcode_byte, file);
}

void add_immediate_to_accumulator(char* instruction, u8 opcode_byte, FILE* file) {
  sprintf(instruction, "add");
  immediate_to_accumulator(instruction, opcode_byte, file);
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

  i8 ch;
  char instruction[128] = {};
  while ((ch = fgetc(file)) != EOF) {
    u8 byte = (u8)ch;

    if (get_bits(byte, 2, 7) == 0b000000) {
      add_register_to_register(instruction, byte, file);
    }
    else if (get_bits(byte, 2, 7) == 0b100000) {
      add_immediate_to_register(instruction, byte, file);
    }
    else if (get_bits(byte, 1, 7) == 0b0000010) {
      add_immediate_to_accumulator(instruction, byte, file);
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

