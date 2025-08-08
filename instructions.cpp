
struct BitsLocation {
  u8 byte_count;
  u8 match;
  u8 mask;
  u8 shift;
};

struct CpuInstructionDefinition {
  const char* operation;
  BitsLocation opcode;
  BitsLocation segment_register;
  BitsLocation reg;
};


#define REG(reg, w_bit)  (register_file[reg + 8*w_bit])

CpuInstructionDefinition instruction_table[] = {
  { .operation="push",
    .opcode={ .byte_count=0, .match=0b00000110, .mask=0b11100111 },
    .segment_register={ .byte_count=0, .mask=0b00011000, .shift=3 } 
  },
  { .operation="push",
    .opcode={ .byte_count=0, .match=0b01010000, .mask=0b11111000 },
    .reg={ .byte_count=0, .mask=0b00000111 } 
  },
};

struct CpuInstruction {
  const char* operation;
  const char* operand1;
  const char* operand2;
};

const char* segment_register[4] = {
  "es", "cs", "ss", "ds"
};

const char* register_file[16] = {
  "al", "cl", "dl", "bl",
  "ah", "ch", "dh", "bh",
  "ax", "cx", "dx", "bx",
  "sp", "bp", "si", "di",
};

#define TABLE_LEN(x)  (sizeof(x)/sizeof(*(x)))

CpuInstruction decode_instruction(u8 opcode, FILE *f) {
  for (size_t i = 0; i < TABLE_LEN(instruction_table); i++) {
    const CpuInstructionDefinition *d = &instruction_table[i];
    if ((opcode & d->opcode.mask) == d->opcode.match) {
      CpuInstruction inst = { .operation = d->operation};
      if (d->segment_register.mask != 0) {
        inst.operand1 = segment_register[(opcode & d->segment_register.mask) >> d->segment_register.shift];
      }
      if (d->reg.mask != 0) {
        inst.operand1 = REG(((opcode & d->reg.mask) >> d->reg.shift), 1);
      }

      return inst;
    }
  }

  return { .operation = "nop" };
}
