
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
    .type=Register_Memory,
    .operation="xchg",
    .min_byte_count=2,
    .opcode={ .byte_count=0, .match=0b10000110, .mask=0b11111110 },
    .reg={ .byte_count=1, .mask=0b00111000, .shift=3 },
    .mod={ .byte_count=1, .mask=0b11000000, .shift=6 },
    .rm={ .byte_count=1, .mask=0b00000111 },
    .w_bit={ .byte_count=0, .mask=0b00000001 },
    .d_bit={ .byte_count=0, .mask=0b00000010, .shift=1 }
  },
  {
    .type=Register_Memory,
    .operation="lea",
    .min_byte_count=2,
    .opcode={ .byte_count=0, .match=0b10001101, .mask=0b11111111 },
    .reg={ .byte_count=1, .mask=0b00111000, .shift=3 },
    .mod={ .byte_count=1, .mask=0b11000000, .shift=6 },
    .rm={ .byte_count=1, .mask=0b00000111 },
    .w_bit={ .byte_count=0, .mask=0b00000001 },
    .d_bit={ .overriden=true, .overriden_value = 1 }
  },
  {
    .type=Register_Memory,
    .operation="lds",
    .min_byte_count=2,
    .opcode={ .byte_count=0, .match=0b11000101, .mask=0b11111111 },
    .reg={ .byte_count=1, .mask=0b00111000, .shift=3 },
    .mod={ .byte_count=1, .mask=0b11000000, .shift=6 },
    .rm={ .byte_count=1, .mask=0b00000111 },
    .w_bit={ .byte_count=0, .mask=0b00000001 },
    .d_bit={ .overriden=true, .overriden_value = 1 }
  },
  {
    .type=Register_Memory,
    .operation="les",
    .min_byte_count=2,
    .opcode={ .byte_count=0, .match=0b11000100, .mask=0b11111111 },
    .reg={ .byte_count=1, .mask=0b00111000, .shift=3 },
    .mod={ .byte_count=1, .mask=0b11000000, .shift=6 },
    .rm={ .byte_count=1, .mask=0b00000111 },
    .w_bit={ .overriden=true, .overriden_value = 1 },
    .d_bit={ .overriden=true, .overriden_value = 1 }
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
  {
    .type=Register_Memory,
    .op_bits={ .byte_count=0, .mask=0b00111000, .shift=3 },
    .min_byte_count=2,
    .opcode={ .byte_count=0, .match=0b00000000, .mask=0b11111100 },
    .reg={ .byte_count=1, .mask=0b00111000, .shift=3 },
    .mod={ .byte_count=1, .mask=0b11000000, .shift=6 },
    .rm={ .byte_count=1, .mask=0b00000111 },
    .w_bit={ .byte_count=0, .mask=0b00000001 },
    .d_bit={ .byte_count=0, .mask=0b00000010, .shift=1 }
  },
  {
    .type=Register_Memory,
    .op_bits={ .byte_count=0, .mask=0b00111000, .shift=3 },
    .min_byte_count=2,
    .opcode={ .byte_count=0, .match=0b00101000, .mask=0b11111100 },
    .reg={ .byte_count=1, .mask=0b00111000, .shift=3 },
    .mod={ .byte_count=1, .mask=0b11000000, .shift=6 },
    .rm={ .byte_count=1, .mask=0b00000111 },
    .w_bit={ .byte_count=0, .mask=0b00000001 },
    .d_bit={ .byte_count=0, .mask=0b00000010, .shift=1 }
  },
  {
    .type=Register_Memory,
    .op_bits={ .byte_count=0, .mask=0b00111000, .shift=3 },
    .min_byte_count=2,
    .opcode={ .byte_count=0, .match=0b00111000, .mask=0b11111100 },
    .reg={ .byte_count=1, .mask=0b00111000, .shift=3 },
    .mod={ .byte_count=1, .mask=0b11000000, .shift=6 },
    .rm={ .byte_count=1, .mask=0b00000111 },
    .w_bit={ .byte_count=0, .mask=0b00000001 },
    .d_bit={ .byte_count=0, .mask=0b00000010, .shift=1 }
  },
  {
    .type=Register_Immediate,
    .op_bits={ .byte_count=1, .mask=0b00111000, .shift=3 },
    .min_byte_count=2,
    .opcode={ .byte_count=0, .match=0b10000000, .mask=0b11111100 },
    .mod={ .byte_count=1, .mask=0b11000000, .shift=6 },
    .rm={ .byte_count=1, .mask=0b00000111 },
    .w_bit={ .byte_count=0, .mask=0b00000001 },
    .s_bit={ .byte_count=0, .mask=0b00000010, .shift=1 }
  },
  {
    .type=Register_Immediate,
    .operation="mov",
    .min_byte_count=2,
    .opcode={ .byte_count=0, .match=0b11000110, .mask=0b11111110 },
    .mod={ .byte_count=1, .mask=0b11000000, .shift=6 },
    .rm={ .byte_count=1, .mask=0b00000111 },
    .w_bit={ .byte_count=0, .mask=0b00000001 }
  },
  {
    .type=Register_Immediate,
    .operation="in",
    .opcode={ .byte_count=0, .match=0b11100100, .mask=0b11111110 },
    .reg={ .overriden=true, .overriden_value = 0 },
    .w_bit={ .byte_count=0, .mask=0b00000001 },
  },
  {
    .type=Register,
    .operation="in",
    .is_accumulator=true,
    .opcode={ .byte_count=0, .match=0b11101100, .mask=0b11111110 },
    .reg={ .overriden=true, .overriden_value = 2 },
    .w_bit={ .byte_count=0, .mask=0b00000001 },
  },
  {
    .type=Register_Immediate,
    .operation="out",
    .opcode={ .byte_count=0, .match=0b11100110, .mask=0b11111110 },
    .reg={ .overriden=true, .overriden_value = 0 },
    .w_bit={ .byte_count=0, .mask=0b00000001 },
    .d_bit={ .overriden=true, .overriden_value = 1 }
  },
  {
    .type=Register,
    .operation="out",
    .is_accumulator=true,
    .opcode={ .byte_count=0, .match=0b11101110, .mask=0b11111110 },
    .reg={ .overriden=true, .overriden_value = 2 },
    .w_bit={ .byte_count=0, .mask=0b00000001 },
    .d_bit={ .overriden=true, .overriden_value = 1 }
  },
  {
    .type=Memory_Immediate,
    .operation="mov",
    .is_accumulator=true,
    .opcode={ .byte_count=0, .match=0b10100000, .mask=0b11111110 },
    .w_bit={ .byte_count=0, .mask=0b00000001 },
    .d_bit={ .overriden=true, .overriden_value = 1 }
  },
  {
    .type=Memory_Immediate,
    .operation="mov",
    .is_accumulator=true,
    .opcode={ .byte_count=0, .match=0b10100010, .mask=0b11111110 },
    .w_bit={ .byte_count=0, .mask=0b00000001 },
  },
  {
    .type=Register_Immediate,
    .op_bits={ .byte_count=0, .mask=0b00111000, .shift=3 },
    .is_accumulator = true,
    .opcode={ .byte_count=0, .match=0b00000100, .mask=0b11111110 },
    .w_bit={ .byte_count=0, .mask=0b00000001 },
  },
  {
    .type=Register_Immediate,
    .op_bits={ .byte_count=0, .mask=0b00111000, .shift=3 },
    .is_accumulator = true,
    .opcode={ .byte_count=0, .match=0b00101100, .mask=0b11111110 },
    .w_bit={ .byte_count=0, .mask=0b00000001 },
  },
  {
    .type=Register_Immediate,
    .op_bits={ .byte_count=0, .mask=0b00111000, .shift=3 },
    .is_accumulator = true,
    .opcode={ .byte_count=0, .match=0b00111100, .mask=0b11111110 },
    .w_bit={ .byte_count=0, .mask=0b00000001 },
  },
};
