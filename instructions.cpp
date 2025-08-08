
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
};

CpuInstructionDefinition instruction_table[] = {
  { .operation="push",
    .opcode={ .byte_count=0, .match=0b00000110, .mask=0b11100111 },
    .segment_register={ .byte_count=0, .mask=0b00011000, .shift=3 } 
  }
};

struct CpuInstruction {
  const char* operation;
  const char* operand1;
  const char* operand2;
};

const char* segment_register[4] = {
  "es", "cs", "ss", "ds"
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
      return inst;
    }
  }

  return { .operation = "nop" };
}
