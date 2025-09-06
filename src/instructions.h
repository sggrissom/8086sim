

#define REG(reg, w_bit)  (register_file[reg + 8*w_bit])
#define TABLE_LEN(x)  (sizeof(x)/sizeof(*(x)))

struct BitsLocation {
  u8 overriden;
  u8 overriden_value;
  u8 byte_count;
  u8 match;
  u8 mask;
  u8 shift;
};

enum InstructionType {
  Solo,
  Register,
  Register_Immediate,
  Memory,
  Memory_Immediate,
  Register_Memory,
  ConditionalJump,
};

enum RepPrefix  { RepNone, RepF3, RepF2 };

struct CpuInstructionDefinition {
  InstructionType type;
  BitsLocation op_bits;
  const char* operation;
  u8 min_byte_count;
  bool is_accumulator;
  BitsLocation opcode;
  BitsLocation pattern;
  BitsLocation segment_register;
  BitsLocation reg;
  BitsLocation mod;
  BitsLocation rm;
  BitsLocation w_bit;
  BitsLocation d_bit;
  BitsLocation s_bit;
  BitsLocation v_bit;
  BitsLocation z_bit;
};

struct CpuInstruction {
  u16 instruction_address;
  InstructionType type;
  const char* operation;

  const char* source;
  const char* dest;
  const char* segment_reg;
  u8 rm;
  u8 mod;
  u8 w_bit;
  u8 s_bit;
  u8 d_bit;
  u8 v_bit;
  u8 z_bit;
  u8 use_v_bit;
  u8 is_accumulator;
  u16 displacement;
  u16 immediate;
  const char *effective_address;
  i16 address_offset;

  u8 byte_len;
  u16 next_ip;
  u8 rep_prefix;
};

static const char* segment_register[4] = {
  "es", "cs", "ss", "ds"
};

static const char* register_file[16] = {
  "al", "cl", "dl", "bl",
  "ah", "ch", "dh", "bh",
  "ax", "cx", "dx", "bx",
  "sp", "bp", "si", "di",
};

static const char* effective_address[8] = {
  "bx + si", "bx + di", "bp + si", "bp + di",
  "si", "di", "bp", "bx",
};

static const char* alu_ops[8] = {
  "add", "or", "adc", "sbb", "and", "sub", "xor", "cmp"
};

void print_byte(u8 byte) {
    for (int i = 7; i >= 0; i--) {
        printf("%c", (byte >> i) & 1 ? '1' : '0');
    }
    printf("\n");
}
