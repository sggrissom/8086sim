#include <cstdio>

unsigned char get_bits(unsigned char byte, int start, int end) {
  int width = end - start + 1;
  unsigned char mask = (1 << width) - 1;
  unsigned char result = (byte >> start) & mask;
  return result;
}

char get_bit(unsigned char byte, int num) {
  return get_bits(byte, num, num);
}

const char* register_file[16] = {
    "al", "cl", "dl", "bl",
    "ah", "ch", "dh", "bh",
    "ax", "cx", "dx", "bx",
    "sp", "bp", "si", "di",
};


void print_move_instruction(unsigned char byte1, unsigned char byte2) {
  char w_bit = get_bit(byte1, 0);
  char d_bit = get_bit(byte1, 1);
  char mod = get_bits(byte2, 6, 7);
  char reg = get_bits(byte2, 3, 5);
  char rm = get_bits(byte2, 0, 2);

  printf("mov %s, %s\n", register_file[rm + 8*w_bit], register_file[reg + 8*w_bit]);
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
        char opcode = get_bits(byte, 2, 7);
        if (opcode == 0b100010) {
          unsigned char next_byte = (unsigned char)fgetc(file);
          print_move_instruction(byte, next_byte);
        }
    }

    fclose(file);
    return 0;
}

