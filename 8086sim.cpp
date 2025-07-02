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

u16 get_immediate(u8 w_bit, FILE* file) {
  u16 dest = (u8)fgetc(file);
  if (w_bit) {
    u8 next_byte = (u8)fgetc(file);
    dest = (next_byte << 8) | dest;
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

void print_move_immediate_to_register(u8 opcode_byte, FILE* file) {
  u8 w_bit = get_bit(opcode_byte , 3);
  u8 reg = get_bits(opcode_byte, 0, 2);

  u16 dest = get_immediate(w_bit, file);
  const char* source = register_file[reg + 8*w_bit];

  printf("mov %s, %d\n", source, dest);
}

void print_move_immediate_to_register_memory(u8 opcode_byte, FILE* file) {
  u8 byte = (u8)fgetc(file);
  u8 w_bit = get_bit(opcode_byte , 0);
  u8 mod = get_bits(byte, 6, 7);
  u8 rm = get_bits(byte, 0, 2);
  i16 displacement = 0;

  char source[20] = {};

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

  u16 immediate = get_immediate(w_bit, file);

  if (w_bit) {
    printf("mov %s, word %d\n", source, immediate);
  } else {
    printf("mov %s, byte %d\n", source, immediate);
  }
}

void print_move_instruction(u8 opcode_byte, FILE* file) {
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
    strcpy(source, register_file[rm + 8*w_bit]);
    strcpy(dest, register_file[reg + 8*w_bit]);
    printf("mov %s, %s\n", source, dest);
    return;
  }

  const char* address;

  strcpy(source, register_file[reg + 8*w_bit]);
  if (rm == 0b110 && mod == 0b00) {
    unsigned int immediate = get_immediate(w_bit, file);
    printf("mov %s, [%d]\n", source, immediate);
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
    printf("mov %s, %s\n", dest, source);
  } else {
    printf("mov %s, %s\n", source, dest);
  }
}

void print_move_memory_to_accumulator(u8 opcode_byte, FILE* file) {
  u8 w_bit = get_bit(opcode_byte , 0);
  u16 immediate = get_immediate(w_bit, file);
  printf("mov ax, [%d]\n", immediate);
}

void print_move_accumulator_to_memory(u8 opcode_byte, FILE* file) {
  u8 w_bit = get_bit(opcode_byte , 0);
  u16 immediate = get_immediate(w_bit, file);
  printf("mov [%d], ax\n", immediate);
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
    while ((ch = fgetc(file)) != EOF) {
        u8 byte = (u8)ch;

        if (get_bits(byte, 2, 7) == 0b100010) {
          print_move_instruction(byte, file);
        }
        else if (get_bits(byte, 4, 7) == 0b1011) {
          print_move_immediate_to_register(byte, file);
        }
        else if (get_bits(byte, 1, 7) == 0b1100011) {
          print_move_immediate_to_register_memory(byte, file);
        }
        else if (get_bits(byte, 1, 7) == 0b1010000) {
          print_move_memory_to_accumulator(byte, file);
        }
        else if (get_bits(byte, 1, 7) == 0b1010001) {
          print_move_accumulator_to_memory(byte, file);
        }
    }

    fclose(file);
    return 0;
}

