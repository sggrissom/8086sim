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

    int ch;
    while ((ch = fgetc(file)) != EOF) {
        unsigned char byte = (unsigned char)ch;

        // check for write instruction
        char opcode = get_bits(byte, 2, 7);
        if (opcode == 0b100010) {
            printf("Write instruction: 0x%02X\n", byte);
        }
    }

    fclose(file);
    return 0;
}

