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
    "[bx + si]", "[bx + di]", "[bp + si]", "[bp + di]",
    "si", "di", "-", "bx",
};

void print_move_instruction(unsigned char opcode_byte, FILE* file) {
  unsigned char byte = (unsigned char)fgetc(file);

  unsigned char w_bit = get_bit(opcode_byte , 0);
  unsigned char d_bit = get_bit(opcode_byte, 1);
  unsigned char mod = get_bits(byte, 6, 7);
  unsigned char reg = get_bits(byte, 3, 5);
  unsigned char rm = get_bits(byte, 0, 2);

  char source[20];
  char dest[20];

  if (mod == 0b00) {
    if (d_bit == 0b0) {
      strcpy(source, register_file[reg + 8*w_bit]);
    } else {
      strcpy(source, register_file[rm + 8*w_bit]);
    }
    if (rm == 0b110) {
      //unsigned char dest = (unsigned char)fgetc(file);
      //printf("mov %s, %u\n", source, dest);
    } else {
      if (d_bit == 0b0) {
        strcpy(dest, effective_address[rm]);
      } else {
        strcpy(dest, effective_address[reg]);
      }
    }
  } else if (mod == 0b01) {
    //printf("mod 01\n");
  } else if (mod == 0b10) {
    //printf("mod 10\n");
  } else if (mod == 0b11) {
    strcpy(source, register_file[rm + 8*w_bit]);
    strcpy(dest, register_file[reg + 8*w_bit]);
  }

  printf("mov %s, %s\n", source, dest);
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

        // check for write instruction
        unsigned char opcode = get_bits(byte, 2, 7);
        if (opcode == 0b100010) {
          print_move_instruction(byte, file);
        }
    }

    fclose(file);
    return 0;
}

