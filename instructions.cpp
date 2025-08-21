
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
  Register_Memory,
  ConditionalJump,
};

struct CpuInstructionDefinition {
  InstructionType type;
  const char* operation;
  u8 min_byte_count;
  bool is_accumulator;
  BitsLocation opcode;
  BitsLocation segment_register;
  BitsLocation reg;
  BitsLocation mod;
  BitsLocation rm;
  BitsLocation w_bit;
  BitsLocation d_bit;
  BitsLocation s_bit;
};

#define REG(reg, w_bit)  (register_file[reg + 8*w_bit])

CpuInstructionDefinition instruction_table[] = {
  { 
    .type=Register,
    .operation="push",
    .opcode={ .byte_count=0, .match=0b00000110, .mask=0b11100111 },
    .segment_register={ .byte_count=0, .mask=0b00011000, .shift=3 } 
  },
  {
    .type=Register,
    .operation="push",
    .opcode={ .byte_count=0, .match=0b01010000, .mask=0b11111000 },
    .reg={ .byte_count=0, .mask=0b00000111 },
    .w_bit={ .overriden=true, .overriden_value = 1 }
  },
  {
    .type=Memory,
    .operation="push",
    .min_byte_count=2,
    .opcode={ .byte_count=0, .match=0b11111111, .mask=0b11111111 },
    .mod={ .byte_count=1, .mask=0b11000000, .shift=6 } ,
    .rm={ .byte_count=1, .mask=0b00000111 },
  },
  {
    .type=Memory,
    .operation="pop",
    .min_byte_count=2,
    .opcode={ .byte_count=0, .match=0b10001111, .mask=0b11111111 },
    .mod={ .byte_count=1, .mask=0b11000000, .shift=6 } ,
    .rm={ .byte_count=1, .mask=0b00000111 }
  },
  {
    .type=Register,
    .operation="pop",
    .opcode={ .byte_count=0, .match=0b00000111, .mask=0b11100111 },
    .segment_register={ .byte_count=0, .mask=0b00011000, .shift=3 } 
  },
  {
    .type=Register,
    .operation="pop",
    .opcode={ .byte_count=0, .match=0b01011000, .mask=0b11111000 },
    .reg={ .byte_count=0, .mask=0b00000111 },
    .w_bit={ .overriden=true, .overriden_value = 1 }
  },
  {
    .type=Register,
    .operation="xchg",
    .is_accumulator = true,
    .opcode={ .byte_count=0, .match=0b10010000, .mask=0b11111000 },
    .reg={ .byte_count=0, .mask=0b00000111 },
    .w_bit={ .overriden=true, .overriden_value = 1 }
  },
  {
    .type=ConditionalJump,
    .operation="je",
    .opcode={ .byte_count=0, .match=0b01110100, .mask=0b11111111 }
  },
  {
    .type=ConditionalJump,
    .operation="jl",
    .opcode={ .byte_count=0, .match=0b01111100, .mask=0b11111111 }
  },
  {
    .type=ConditionalJump,
    .operation="jle",
    .opcode={ .byte_count=0, .match=0b01111110, .mask=0b11111111 }
  },
  {
    .type=ConditionalJump,
    .operation="jb",
    .opcode={ .byte_count=0, .match=0b01110010, .mask=0b11111111 }
  },
  {
    .type=ConditionalJump,
    .operation="jbe",
    .opcode={ .byte_count=0, .match=0b01110110, .mask=0b11111111 }
  },
  {
    .type=ConditionalJump,
    .operation="jp",
    .opcode={ .byte_count=0, .match=0b01111010, .mask=0b11111111 }
  },
  {
    .type=ConditionalJump,
    .operation="jo",
    .opcode={ .byte_count=0, .match=0b01110000, .mask=0b11111111 }
  },
  {
    .type=ConditionalJump,
    .operation="js",
    .opcode={ .byte_count=0, .match=0b01111000, .mask=0b11111111 }
  },
  {
    .type=ConditionalJump,
    .operation="jne",
    .opcode={ .byte_count=0, .match=0b01110101, .mask=0b11111111 }
  },
  {
    .type=ConditionalJump,
    .operation="jnl",
    .opcode={ .byte_count=0, .match=0b01111101, .mask=0b11111111 }
  },
  {
    .type=ConditionalJump,
    .operation="jnle",
    .opcode={ .byte_count=0, .match=0b01111111, .mask=0b11111111 }
  },
  {
    .type=ConditionalJump,
    .operation="jnb",
    .opcode={ .byte_count=0, .match=0b01110011, .mask=0b11111111 }
  },
  {
    .type=ConditionalJump,
    .operation="jnbe",
    .opcode={ .byte_count=0, .match=0b01110111, .mask=0b11111111 }
  },
  {
    .type=ConditionalJump,
    .operation="jnp",
    .opcode={ .byte_count=0, .match=0b01111011, .mask=0b11111111 }
  },
  {
    .type=ConditionalJump,
    .operation="jno",
    .opcode={ .byte_count=0, .match=0b01110001, .mask=0b11111111 }
  },
  {
    .type=ConditionalJump,
    .operation="jns",
    .opcode={ .byte_count=0, .match=0b01111001, .mask=0b11111111 }
  },
  {
    .type=ConditionalJump,
    .operation="loop",
    .opcode={ .byte_count=0, .match=0b11100010, .mask=0b11111111 }
  },
  {
    .type=ConditionalJump,
    .operation="loopz",
    .opcode={ .byte_count=0, .match=0b11100001, .mask=0b11111111 }
  },
  {
    .type=ConditionalJump,
    .operation="loopnz",
    .opcode={ .byte_count=0, .match=0b11100000, .mask=0b11111111 }
  },
  {
    .type=ConditionalJump,
    .operation="jcxz",
    .opcode={ .byte_count=0, .match=0b11100011, .mask=0b11111111 }
  },
  {
    .type=Solo,
    .operation="xlat",
    .opcode={ .byte_count=0, .match=0b11010111, .mask=0b11111111 }
  },
  {
    .type=Solo,
    .operation="lahf",
    .opcode={ .byte_count=0, .match=0b10011111, .mask=0b11111111 }
  },
  {
    .type=Solo,
    .operation="sahf",
    .opcode={ .byte_count=0, .match=0b10011110, .mask=0b11111111 }
  },
  {
    .type=Solo,
    .operation="pushf",
    .opcode={ .byte_count=0, .match=0b10011100, .mask=0b11111111 }
  },
  {
    .type=Solo,
    .operation="popf",
    .opcode={ .byte_count=0, .match=0b10011101, .mask=0b11111111 }
  },
  {
    .type=Register_Memory,
    .operation="mov",
    .min_byte_count=2,
    .opcode={ .byte_count=0, .match=0b10001000, .mask=0b11111100 },
    .reg={ .byte_count=1, .mask=0b00111000, .shift=3 },
    .mod={ .byte_count=1, .mask=0b11000000, .shift=6 },
    .rm={ .byte_count=1, .mask=0b00000111 },
    .w_bit={ .byte_count=0, .mask=0b00000001 },
    .d_bit={ .byte_count=0, .mask=0b00000010, .shift=1 }
  },
  {
    .type=Register_Immediate,
    .operation="mov",
    .opcode={ .byte_count=0, .match=0b10110000, .mask=0b11110000 },
    .reg={ .byte_count=0, .mask=0b00000111 },
    .w_bit={ .byte_count=0, .mask=0b00001000, .shift=3 },
  },
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
  u8 is_accumulator;
  u16 displacement;
  u16 immediate;
  const char *effective_address;
  i16 address_offset;
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

const char* effective_address[8] = {
  "bx + si", "bx + di", "bp + si", "bp + di",
  "si", "di", "bp", "bx",
};

#define TABLE_LEN(x)  (sizeof(x)/sizeof(*(x)))

static u8 get_bits(u8 *bytes, MemoryReader *r, BitsLocation location) {
  if (location.overriden) return location.overriden_value;
  return (bytes[location.byte_count] & location.mask) >> location.shift;
}

i16 get_displacement(u8 w_bit, MemoryReader *reader) {
  u8 byte;
  read(reader, &byte);
  i16 dest = (u16)byte;
  if (w_bit) {
    u8 next_byte;
    read(reader, &next_byte);
    dest = ((i8)next_byte << 8) | dest;
  } else if (dest & 0x80) {
    dest |= 0xFF00;
  }
  return dest;
}

u16 get_immediate(u8 w_bit, u8 s_bit, MemoryReader *reader) {
  u8 byte;
  read(reader, &byte);
  u16 dest = (u16)byte;
  if (w_bit) {
    u8 next_byte;
    read(reader, &next_byte);
    dest = (next_byte << 8) | dest;
  } else if (s_bit && (dest & 0x80)) {
    dest |= 0xFF00;
  }
  return dest;
}

CpuInstruction decode_instruction(u8 opcode, MemoryReader *r) {
  u8 bytes[6] = {opcode};
  for (size_t i = 0; i < TABLE_LEN(instruction_table); i++) {
    const CpuInstructionDefinition *d = &instruction_table[i];

    if ((opcode & d->opcode.mask) == d->opcode.match) {
      CpuInstruction inst = {
        .instruction_address = r->ip,
        .type = d->type,
        .operation = d->operation
      };
      for (int i = 1; i < d->min_byte_count; ++i) {
        read(r, &bytes[i]);
      }

      if (d->segment_register.mask != 0) {
        inst.segment_reg = segment_register[get_bits(bytes, r, d->segment_register)];
      }
      if (d->w_bit.mask != 0 || d->w_bit.overriden) {
        inst.w_bit = get_bits(bytes, r, d->w_bit);
      }
      if (d->d_bit.mask != 0) {
        inst.d_bit = get_bits(bytes, r, d->d_bit);
      }
      if (d->s_bit.mask != 0) {
        inst.s_bit = get_bits(bytes, r, d->s_bit);
      }
      if (d->reg.mask != 0) {
        inst.source = REG(get_bits(bytes, r, d->reg), inst.w_bit);
      }
      if (d->mod.mask != 0) {
        inst.mod = get_bits(bytes, r, d->mod);
      }
      if (d->rm.mask != 0) {
        inst.rm = get_bits(bytes, r, d->rm);
        inst.effective_address = effective_address[inst.rm];
      }

      if (inst.type == Register_Immediate) {
        inst.immediate = get_immediate(inst.w_bit, 0, r);
      }
      if (inst.type == Memory) {
        if (inst.rm == 0b110 && inst.mod == 0b00) {
          u8 byte;
          read(r, &byte);
          i16 dest = (u16)byte;
          read(r, &byte);
          dest = ((i8)byte << 8) | dest;
          inst.displacement = dest;
        }
        if (inst.mod == 0b01) {
          inst.address_offset = get_displacement(0, r);
        }
        if (inst.mod == 0b10) {
          inst.address_offset = get_displacement(1, r);
        }
      }
      if (d->type == ConditionalJump) {
        u8 byte;
        read(r, &byte);
        inst.address_offset = byte;
      }
      if (inst.type == Register_Memory) {
        if (inst.mod == 0b11) {
          inst.dest = REG(inst.rm, inst.w_bit);
          if (inst.d_bit == 0) {
            const char * source = inst.source;
            inst.source = inst.dest;
            inst.dest = source;
          }
          return inst;
        }
        if (inst.rm == 0b110 && inst.mod == 0b00) {
          inst.immediate = get_immediate(inst.w_bit, 0, r);
        } else {
          inst.dest = effective_address[inst.rm];
          if (inst.mod == 0b01) {
            inst.displacement = get_displacement(0, r);
          } else if (inst.mod == 0b10) {
            inst.displacement = get_displacement(1, r);
          }
        }
      }
      if (d->is_accumulator) {
        inst.is_accumulator = true;
      }

      return inst;
    }
  }

  return { .operation = "nop" };
}
