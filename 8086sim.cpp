#include <cstdio>
#include <cstring>

unsigned char get_bits(unsigned char byte, int start, int end) {
  int width = end - start + 1;
  unsigned char mask = (1 << width) - 1;
  unsigned char result = (byte >> start) & mask;
  return result;
}

unsigned char get_bit(unsigned char byte, int num) {
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

unsigned int get_immediate(unsigned char w_bit, FILE* file) {
  unsigned int dest = (unsigned char)fgetc(file);
  if (w_bit) {
    unsigned int next_byte = (unsigned char)fgetc(file);
    dest = (next_byte << 8) | dest;
  }
  return dest;
}

void print_move_immediate_to_register(unsigned char opcode_byte, FILE* file) {
  unsigned char w_bit = get_bit(opcode_byte , 3);
  unsigned char reg = get_bits(opcode_byte, 0, 2);

  unsigned int dest = get_immediate(w_bit, file);
  const char* source = register_file[reg + 8*w_bit];

  printf("mov %s, %d\n", source, dest);
}

void print_move_instruction(unsigned char opcode_byte, FILE* file) {
  unsigned char byte = (unsigned char)fgetc(file);

  unsigned char w_bit = get_bit(opcode_byte , 0);
  unsigned char d_bit = get_bit(opcode_byte, 1);
  unsigned char mod = get_bits(byte, 6, 7);
  unsigned char reg = get_bits(byte, 3, 5);
  unsigned char rm = get_bits(byte, 0, 2);

  char source[20] = {};
  char dest[20] = {};
  unsigned int immediate = 0;

  if (mod == 0b11) {
    strcpy(source, register_file[rm + 8*w_bit]);
    strcpy(dest, register_file[reg + 8*w_bit]);
    printf("mov %s, %s\n", source, dest);
    return;
  }

  const char* address;

  strcpy(source, register_file[reg + 8*w_bit]);
  if (rm == 0b110 && mod == 0b00) {
    //unsigned char dest = (unsigned char)fgetc(file);
    //printf("mov %s, %u\n", source, dest);
  } else {
    address = effective_address[rm];
  }
  if (mod == 0b01) {
    immediate = get_immediate(0, file);
  } else if (mod == 0b10) {
    immediate = get_immediate(1, file);
  }
  if (immediate > 0) {
    sprintf(dest, "[%s + %d]", address, immediate);
  } else {
    sprintf(dest, "[%s]", address);
  }

  if (d_bit == 0b0) {
    printf("mov %s, %s\n", dest, source);
  } else {
    printf("mov %s, %s\n", source, dest);
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

    int ch;
    while ((ch = fgetc(file)) != EOF) {
        unsigned char byte = (unsigned char)ch;

        if (get_bits(byte, 2, 7) == 0b100010) {
          print_move_instruction(byte, file);
        }
        if (get_bits(byte, 4, 7) == 0b1011) {
          print_move_immediate_to_register(byte, file);
        }
    }

    fclose(file);
    return 0;
}

