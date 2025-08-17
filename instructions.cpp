
struct BitsLocation {
  u8 byte_count;
  u8 match;
  u8 mask;
  u8 shift;
};

struct CpuInstructionDefinition {
  const char* operation;
  bool is_accumulator;
  BitsLocation opcode;
  BitsLocation segment_register;
  BitsLocation reg;
  BitsLocation mod;
  BitsLocation rm;
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
  { .operation="push",
    .opcode={ .byte_count=0, .match=0b11111111, .mask=0b11111111 },
    .mod={ .byte_count=1, .mask=0b11000000, .shift=6 } ,
    .rm={ .byte_count=1, .mask=0b00000111 }
  },
  { .operation="pop",
    .opcode={ .byte_count=0, .match=0b10001111, .mask=0b11111111 },
    .mod={ .byte_count=1, .mask=0b11000000, .shift=6 } ,
    .rm={ .byte_count=1, .mask=0b00000111 }
  },
  { .operation="pop",
    .opcode={ .byte_count=0, .match=0b00000111, .mask=0b11100111 },
    .segment_register={ .byte_count=0, .mask=0b00011000, .shift=3 } 
  },
  { .operation="pop",
    .opcode={ .byte_count=0, .match=0b01011000, .mask=0b11111000 },
    .reg={ .byte_count=0, .mask=0b00000111 } 
  },
  { .operation="xchg",
    .is_accumulator = true,
    .opcode={ .byte_count=0, .match=0b10010000, .mask=0b11111000 },
    .reg={ .byte_count=0, .mask=0b00000111 }
  },
};

struct CpuInstruction {
  u16 instruction_address;
  const char* operation;

  const char* reg;
  const char* segment_reg;
  u8 rm;
  u8 mod;
  u8 w_bit;
  u8 s_bit;
  u8 d_bit;
  u8 is_accumulator;
  u16 displacement;
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

bool load_byte_if_missing(u8 *bytes, MemoryReader *r, u8 byte_number) {
  if (bytes[byte_number] == 0) {
    read(r, &bytes[byte_number]);
  }
  return true;
}


static u8 get_bits(u8 *bytes, MemoryReader *r, BitsLocation location) {
  load_byte_if_missing(bytes, r, location.byte_count);
  return (bytes[location.byte_count] & location.mask) >> location.shift;
}
    

CpuInstruction decode_instruction(u8 opcode, MemoryReader *r) {
  u8 bytes[6] = {opcode};
  for (size_t i = 0; i < TABLE_LEN(instruction_table); i++) {
    const CpuInstructionDefinition *d = &instruction_table[i];

    if ((opcode & d->opcode.mask) == d->opcode.match) {
      CpuInstruction inst = { .instruction_address = r->ip, .operation = d->operation};
      if (d->segment_register.mask != 0) {
        inst.segment_reg = segment_register[get_bits(bytes, r, d->segment_register)];
      }
      if (d->reg.mask != 0) {
        inst.reg = REG(get_bits(bytes, r, d->reg), 1);
      }
      if (d->mod.mask != 0) {
        inst.mod = get_bits(bytes, r, d->mod);
      }
      if (d->rm.mask != 0) {
        inst.rm = get_bits(bytes, r, d->rm);
        inst.effective_address = effective_address[inst.rm];
      }
      if (inst.rm == 0b110 && inst.mod == 0b00) {
        u8 byte;
        read(r, &byte);
        i16 dest = (u16)byte;
        read(r, &byte);
        dest = ((i8)byte << 8) | dest;
        inst.displacement = dest;
      }
      if (inst.mod == 0b01) {
        u8 byte;
        read(r, &byte);
        i16 dest = (u16)byte;
        if (dest & 0x80) {
          dest |= 0xFF00;
        }
        inst.address_offset = dest;
      }
      if (inst.mod == 0b10) {
        u8 byte;
        read(r, &byte);
        i16 dest = (u16)byte;
        read(r, &byte);
        dest = ((i8)byte << 8) | dest;
        inst.address_offset = dest;
      }
      if (d->is_accumulator) {
        inst.is_accumulator = true;
      }

      return inst;
    }
  }

  return { .operation = "nop" };
}
