
CpuInstructionDefinition instruction_table[] = {
  {
    .type=Solo,
    .operation=OP_AAM,
    .min_byte_count=2,
    .opcode={ .byte_count=0, .match=0b11010100 , .mask=0b11111111 },
    .pattern={ .byte_count=1, .match=0b00001010, .mask=0b11111111},
  },
  {
    .type=Solo,
    .operation=OP_AAD,
    .min_byte_count=2,
    .opcode={ .byte_count=0, .match=0b11010101 , .mask=0b11111111 },
    .pattern={ .byte_count=1, .match=0b00001010, .mask=0b11111111},
  },
  {
    .type=Solo,
    .operation=OP_CBW,
    .opcode={ .byte_count=0, .match=0b10011000 , .mask=0b11111111 },
  },
  {
    .type=Solo,
    .operation=OP_CWD,
    .opcode={ .byte_count=0, .match=0b10011001 , .mask=0b11111111 },
  },
  {
    .type=Solo,
    .operation=OP_MOVS,
    .opcode={ .byte_count=0, .match=0b10100100, .mask=0b11111110 },
    .w_bit={ .byte_count=0, .mask=0b00000001 }
  },
  {
    .type=Solo,
    .operation=OP_CMPS,
    .opcode={ .byte_count=0, .match=0b10100110, .mask=0b11111110 },
    .w_bit={ .byte_count=0, .mask=0b00000001 }
  },
  {
    .type=Solo,
    .operation=OP_SCAS,
    .opcode={ .byte_count=0, .match=0b10101110, .mask=0b11111110 },
    .w_bit={ .byte_count=0, .mask=0b00000001 }
  },
  {
    .type=Solo,
    .operation=OP_LODS,
    .opcode={ .byte_count=0, .match=0b10101100, .mask=0b11111110 },
    .w_bit={ .byte_count=0, .mask=0b00000001 }
  },
  {
    .type=Solo,
    .operation=OP_STOS,
    .opcode={ .byte_count=0, .match=0b10101010, .mask=0b11111110 },
    .w_bit={ .byte_count=0, .mask=0b00000001 }
  },
  {
    .type=Memory,
    .operation=OP_INC,
    .min_byte_count=2,
    .opcode={ .byte_count=0, .match=0b11111110, .mask=0b11111110 },
    .pattern={ .byte_count=1, .match=0b00000000, .mask=0b00111000 },
    .mod={ .byte_count=1, .mask=0b11000000, .shift=6 } ,
    .rm={ .byte_count=1, .mask=0b00000111 },
    .w_bit={ .mask=0b00000001 }
  },
  {
    .type=Register,
    .operation=OP_INC,
    .opcode={ .byte_count=0, .match=0b01000000, .mask=0b11111000 },
    .reg={ .byte_count=0, .mask=0b00000111 },
    .w_bit={ .overriden=true, .overriden_value = 1 }
  },
  {
    .type=Memory,
    .operation=OP_DEC,
    .min_byte_count=2,
    .opcode={ .byte_count=0, .match=0b11111110, .mask=0b11111110 },
    .pattern={ .byte_count=1, .match=0b00001000, .mask=0b00111000 },
    .mod={ .byte_count=1, .mask=0b11000000, .shift=6 } ,
    .rm={ .byte_count=1, .mask=0b00000111 },
    .w_bit={ .mask=0b00000001 }
  },
  {
    .type=Register,
    .operation=OP_DEC,
    .opcode={ .byte_count=0, .match=0b01001000, .mask=0b11111000 },
    .reg={ .byte_count=0, .mask=0b00000111 },
    .w_bit={ .overriden=true, .overriden_value = 1 }
  },
  {
    .type=Memory,
    .operation=OP_NEG,
    .min_byte_count=2,
    .opcode={ .byte_count=0, .match=0b11110110, .mask=0b11111110 },
    .pattern={ .byte_count=1, .match=0b00011000, .mask=0b00111000 },
    .mod={ .byte_count=1, .mask=0b11000000, .shift=6 } ,
    .rm={ .byte_count=1, .mask=0b00000111 },
    .w_bit={ .mask=0b00000001 }
  },
  {
    .type=Memory,
    .operation=OP_MUL,
    .min_byte_count=2,
    .opcode={ .byte_count=0, .match=0b11110110, .mask=0b11111110 },
    .pattern={ .byte_count=1, .match=0b00100000, .mask=0b00111000 },
    .mod={ .byte_count=1, .mask=0b11000000, .shift=6 } ,
    .rm={ .byte_count=1, .mask=0b00000111 },
    .w_bit={ .mask=0b00000001 }
  },
  {
    .type=Memory,
    .operation=OP_IMUL,
    .min_byte_count=2,
    .opcode={ .byte_count=0, .match=0b11110110, .mask=0b11111110 },
    .pattern={ .byte_count=1, .match=0b00101000, .mask=0b00111000 },
    .mod={ .byte_count=1, .mask=0b11000000, .shift=6 } ,
    .rm={ .byte_count=1, .mask=0b00000111 },
    .w_bit={ .mask=0b00000001 }
  },
  {
    .type=Memory,
    .operation=OP_NOT,
    .min_byte_count=2,
    .opcode={ .byte_count=0, .match=0b11110110, .mask=0b11111110 },
    .pattern={ .byte_count=1, .match=0b00010000, .mask=0b00111000 },
    .mod={ .byte_count=1, .mask=0b11000000, .shift=6 } ,
    .rm={ .byte_count=1, .mask=0b00000111 },
    .w_bit={ .mask=0b00000001 }
  },
  {
    .type=Memory,
    .operation=OP_SHL,
    .min_byte_count=2,
    .opcode={ .byte_count=0, .match=0b11010000, .mask=0b11111100 },
    .pattern={ .byte_count=1, .match=0b00100000, .mask=0b00111000 },
    .mod={ .byte_count=1, .mask=0b11000000, .shift=6 } ,
    .rm={ .byte_count=1, .mask=0b00000111 },
    .w_bit={ .mask=0b00000001 },
    .v_bit={ .mask=0b00000010, .shift=1 },
  },
  {
    .type=Memory,
    .operation=OP_SHR,
    .min_byte_count=2,
    .opcode={ .byte_count=0, .match=0b11010000, .mask=0b11111100 },
    .pattern={ .byte_count=1, .match=0b00101000, .mask=0b00111000 },
    .mod={ .byte_count=1, .mask=0b11000000, .shift=6 } ,
    .rm={ .byte_count=1, .mask=0b00000111 },
    .w_bit={ .mask=0b00000001 },
    .v_bit={ .mask=0b00000010, .shift=1 },
  },
  {
    .type=Memory,
    .operation=OP_SAR,
    .min_byte_count=2,
    .opcode={ .byte_count=0, .match=0b11010000, .mask=0b11111100 },
    .pattern={ .byte_count=1, .match=0b00111000, .mask=0b00111000 },
    .mod={ .byte_count=1, .mask=0b11000000, .shift=6 } ,
    .rm={ .byte_count=1, .mask=0b00000111 },
    .w_bit={ .mask=0b00000001 },
    .v_bit={ .mask=0b00000010, .shift=1 },
  },
  {
    .type=Memory,
    .operation=OP_ROL,
    .min_byte_count=2,
    .opcode={ .byte_count=0, .match=0b11010000, .mask=0b11111100 },
    .pattern={ .byte_count=1, .match=0b00000000, .mask=0b00111000 },
    .mod={ .byte_count=1, .mask=0b11000000, .shift=6 } ,
    .rm={ .byte_count=1, .mask=0b00000111 },
    .w_bit={ .mask=0b00000001 },
    .v_bit={ .mask=0b00000010, .shift=1 },
  },
  {
    .type=Memory,
    .operation=OP_ROR,
    .min_byte_count=2,
    .opcode={ .byte_count=0, .match=0b11010000, .mask=0b11111100 },
    .pattern={ .byte_count=1, .match=0b00001000, .mask=0b00111000 },
    .mod={ .byte_count=1, .mask=0b11000000, .shift=6 } ,
    .rm={ .byte_count=1, .mask=0b00000111 },
    .w_bit={ .mask=0b00000001 },
    .v_bit={ .mask=0b00000010, .shift=1 },
  },
  {
    .type=Memory,
    .operation=OP_RCL,
    .min_byte_count=2,
    .opcode={ .byte_count=0, .match=0b11010000, .mask=0b11111100 },
    .pattern={ .byte_count=1, .match=0b00010000, .mask=0b00111000 },
    .mod={ .byte_count=1, .mask=0b11000000, .shift=6 } ,
    .rm={ .byte_count=1, .mask=0b00000111 },
    .w_bit={ .mask=0b00000001 },
    .v_bit={ .mask=0b00000010, .shift=1 },
  },
  {
    .type=Memory,
    .operation=OP_RCR,
    .min_byte_count=2,
    .opcode={ .byte_count=0, .match=0b11010000, .mask=0b11111100 },
    .pattern={ .byte_count=1, .match=0b00011000, .mask=0b00111000 },
    .mod={ .byte_count=1, .mask=0b11000000, .shift=6 } ,
    .rm={ .byte_count=1, .mask=0b00000111 },
    .w_bit={ .mask=0b00000001 },
    .v_bit={ .mask=0b00000010, .shift=1 },
  },
  {
    .type=Memory,
    .operation=OP_DIV,
    .min_byte_count=2,
    .opcode={ .byte_count=0, .match=0b11110110, .mask=0b11111110 },
    .pattern={ .byte_count=1, .match=0b00110000, .mask=0b00111000 },
    .mod={ .byte_count=1, .mask=0b11000000, .shift=6 } ,
    .rm={ .byte_count=1, .mask=0b00000111 },
    .w_bit={ .mask=0b00000001 }
  },
  {
    .type=Memory,
    .operation=OP_IDIV,
    .min_byte_count=2,
    .opcode={ .byte_count=0, .match=0b11110110, .mask=0b11111110 },
    .pattern={ .byte_count=1, .match=0b00111000, .mask=0b00111000 },
    .mod={ .byte_count=1, .mask=0b11000000, .shift=6 } ,
    .rm={ .byte_count=1, .mask=0b00000111 },
    .w_bit={ .mask=0b00000001 }
  },
  {
    .type=Register_Immediate,
    .operation=OP_TEST,
    .min_byte_count=2,
    .opcode={ .byte_count=0, .match=0b11110110, .mask=0b11111110 },
    .pattern={ .byte_count=1, .match=0b00000000, .mask=0b00111000 },
    .mod={ .byte_count=1, .mask=0b11000000, .shift=6 } ,
    .rm={ .byte_count=1, .mask=0b00000111 },
    .w_bit={ .mask=0b00000001 }
  },
  { 
    .type=Register,
    .operation=OP_PUSH,
    .opcode={ .byte_count=0, .match=0b00000110, .mask=0b11100111 },
    .segment_register={ .byte_count=0, .mask=0b00011000, .shift=3 } 
  },
  {
    .type=Register,
    .operation=OP_PUSH,
    .opcode={ .byte_count=0, .match=0b01010000, .mask=0b11111000 },
    .reg={ .byte_count=0, .mask=0b00000111 },
    .w_bit={ .overriden=true, .overriden_value = 1 }
  },
  {
    .type=Memory,
    .operation=OP_PUSH,
    .min_byte_count=2,
    .opcode={ .byte_count=0, .match=0b11111111, .mask=0b11111111 },
    .pattern={ .byte_count=1, .match=0b00110000, .mask=0b00111000 },
    .mod={ .byte_count=1, .mask=0b11000000, .shift=6 } ,
    .rm={ .byte_count=1, .mask=0b00000111 },
    .w_bit={ .overriden=true, .overriden_value = 1 }
  },
  {
    .type=Memory,
    .operation=OP_POP,
    .min_byte_count=2,
    .opcode={ .byte_count=0, .match=0b10001111, .mask=0b11111111 },
    .mod={ .byte_count=1, .mask=0b11000000, .shift=6 } ,
    .rm={ .byte_count=1, .mask=0b00000111 },
    .w_bit={ .overriden=true, .overriden_value = 1 }
  },
  {
    .type=Register,
    .operation=OP_POP,
    .opcode={ .byte_count=0, .match=0b00000111, .mask=0b11100111 },
    .segment_register={ .byte_count=0, .mask=0b00011000, .shift=3 } 
  },
  {
    .type=Register,
    .operation=OP_POP,
    .opcode={ .byte_count=0, .match=0b01011000, .mask=0b11111000 },
    .reg={ .byte_count=0, .mask=0b00000111 },
    .w_bit={ .overriden=true, .overriden_value = 1 }
  },
  {
    .type=Register,
    .operation=OP_XCHG,
    .is_accumulator = true,
    .opcode={ .byte_count=0, .match=0b10010000, .mask=0b11111000 },
    .reg={ .byte_count=0, .mask=0b00000111 },
    .w_bit={ .overriden=true, .overriden_value = 1 }
  },
  {
    .type=Register_Memory,
    .operation=OP_XCHG,
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
    .operation=OP_LEA,
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
    .operation=OP_LDS,
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
    .operation=OP_LES,
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
    .operation=OP_JE,
    .opcode={ .byte_count=0, .match=0b01110100, .mask=0b11111111 }
  },
  {
    .type=ConditionalJump,
    .operation=OP_JL,
    .opcode={ .byte_count=0, .match=0b01111100, .mask=0b11111111 }
  },
  {
    .type=ConditionalJump,
    .operation=OP_JLE,
    .opcode={ .byte_count=0, .match=0b01111110, .mask=0b11111111 }
  },
  {
    .type=ConditionalJump,
    .operation=OP_JB,
    .opcode={ .byte_count=0, .match=0b01110010, .mask=0b11111111 }
  },
  {
    .type=ConditionalJump,
    .operation=OP_JBE,
    .opcode={ .byte_count=0, .match=0b01110110, .mask=0b11111111 }
  },
  {
    .type=ConditionalJump,
    .operation=OP_JP,
    .opcode={ .byte_count=0, .match=0b01111010, .mask=0b11111111 }
  },
  {
    .type=ConditionalJump,
    .operation=OP_JO,
    .opcode={ .byte_count=0, .match=0b01110000, .mask=0b11111111 }
  },
  {
    .type=ConditionalJump,
    .operation=OP_JS,
    .opcode={ .byte_count=0, .match=0b01111000, .mask=0b11111111 }
  },
  {
    .type=ConditionalJump,
    .operation=OP_JNE,
    .opcode={ .byte_count=0, .match=0b01110101, .mask=0b11111111 }
  },
  {
    .type=ConditionalJump,
    .operation=OP_JNL,
    .opcode={ .byte_count=0, .match=0b01111101, .mask=0b11111111 }
  },
  {
    .type=ConditionalJump,
    .operation=OP_JNLE,
    .opcode={ .byte_count=0, .match=0b01111111, .mask=0b11111111 }
  },
  {
    .type=ConditionalJump,
    .operation=OP_JNB,
    .opcode={ .byte_count=0, .match=0b01110011, .mask=0b11111111 }
  },
  {
    .type=ConditionalJump,
    .operation=OP_JNBE,
    .opcode={ .byte_count=0, .match=0b01110111, .mask=0b11111111 }
  },
  {
    .type=ConditionalJump,
    .operation=OP_JNP,
    .opcode={ .byte_count=0, .match=0b01111011, .mask=0b11111111 }
  },
  {
    .type=ConditionalJump,
    .operation=OP_JNO,
    .opcode={ .byte_count=0, .match=0b01110001, .mask=0b11111111 }
  },
  {
    .type=ConditionalJump,
    .operation=OP_JNS,
    .opcode={ .byte_count=0, .match=0b01111001, .mask=0b11111111 }
  },
  {
    .type=ConditionalJump,
    .operation=OP_LOOP,
    .opcode={ .byte_count=0, .match=0b11100010, .mask=0b11111111 }
  },
  {
    .type=ConditionalJump,
    .operation=OP_LOOPZ,
    .opcode={ .byte_count=0, .match=0b11100001, .mask=0b11111111 }
  },
  {
    .type=ConditionalJump,
    .operation=OP_LOOPNZ,
    .opcode={ .byte_count=0, .match=0b11100000, .mask=0b11111111 }
  },
  {
    .type=ConditionalJump,
    .operation=OP_JCXZ,
    .opcode={ .byte_count=0, .match=0b11100011, .mask=0b11111111 }
  },
  {
    .type=Solo,
    .operation=OP_XLAT,
    .opcode={ .byte_count=0, .match=0b11010111, .mask=0b11111111 }
  },
  {
    .type=Solo,
    .operation=OP_LAHF,
    .opcode={ .byte_count=0, .match=0b10011111, .mask=0b11111111 }
  },
  {
    .type=Solo,
    .operation=OP_SAHF,
    .opcode={ .byte_count=0, .match=0b10011110, .mask=0b11111111 }
  },
  {
    .type=Solo,
    .operation=OP_PUSHF,
    .opcode={ .byte_count=0, .match=0b10011100, .mask=0b11111111 }
  },
  {
    .type=Solo,
    .operation=OP_POPF,
    .opcode={ .byte_count=0, .match=0b10011101, .mask=0b11111111 }
  },
  {
    .type=Solo,
    .operation=OP_AAA,
    .opcode={ .byte_count=0, .match=0b00110111, .mask=0b11111111 }
  },
  {
    .type=Solo,
    .operation=OP_DAA,
    .opcode={ .byte_count=0, .match=0b00100111, .mask=0b11111111 }
  },
  {
    .type=Solo,
    .operation=OP_AAS,
    .opcode={ .byte_count=0, .match=0b00111111, .mask=0b11111111 }
  },
  {
    .type=Solo,
    .operation=OP_DAS,
    .opcode={ .byte_count=0, .match=0b00101111, .mask=0b11111111 }
  },
  {
    .type=Register_Memory,
    .operation=OP_MOV,
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
    .operation=OP_MOV,
    .opcode={ .byte_count=0, .match=0b10110000, .mask=0b11110000 },
    .reg={ .byte_count=0, .mask=0b00000111 },
    .w_bit={ .byte_count=0, .mask=0b00001000, .shift=3 },
  },
  {
    .type=Register_Memory,
    .operation=OP_ADC,
    .min_byte_count=2,
    .opcode={ .byte_count=0, .match=0b00010000, .mask=0b11111100 },
    .pattern={ .byte_count=1, .match=0b00010000, .mask=0b00111000 },
    .reg={ .byte_count=1, .mask=0b00111000, .shift=3 },
    .mod={ .byte_count=1, .mask=0b11000000, .shift=6 },
    .rm={ .byte_count=1, .mask=0b00000111 },
    .w_bit={ .byte_count=0, .mask=0b00000001 },
    .d_bit={ .byte_count=0, .mask=0b00000010, .shift=1 }
  },
  {
    .type=Register_Memory,
    .operation=OP_TEST,
    .min_byte_count=2,
    .opcode={ .byte_count=0, .match=0b10000100, .mask=0b11111110 },
    .reg={ .byte_count=1, .mask=0b00111000, .shift=3 },
    .mod={ .byte_count=1, .mask=0b11000000, .shift=6 },
    .rm={ .byte_count=1, .mask=0b00000111 },
    .w_bit={ .byte_count=0, .mask=0b00000001 },
  },
  {
    .type=Register_Immediate,
    .operation=OP_MOV,
    .min_byte_count=2,
    .opcode={ .byte_count=0, .match=0b11000110, .mask=0b11111110 },
    .mod={ .byte_count=1, .mask=0b11000000, .shift=6 },
    .rm={ .byte_count=1, .mask=0b00000111 },
    .w_bit={ .byte_count=0, .mask=0b00000001 }
  },
  {
    .type=Register_Immediate,
    .operation=OP_IN,
    .opcode={ .byte_count=0, .match=0b11100100, .mask=0b11111110 },
    .reg={ .overriden=true, .overriden_value = 0 },
    .w_bit={ .byte_count=0, .mask=0b00000001 },
  },
  {
    .type=Register,
    .operation=OP_IN,
    .is_accumulator=true,
    .opcode={ .byte_count=0, .match=0b11101100, .mask=0b11111110 },
    .reg={ .overriden=true, .overriden_value = 2 },
    .w_bit={ .byte_count=0, .mask=0b00000001 },
  },
  {
    .type=Register_Immediate,
    .operation=OP_OUT,
    .opcode={ .byte_count=0, .match=0b11100110, .mask=0b11111110 },
    .reg={ .overriden=true, .overriden_value = 0 },
    .w_bit={ .byte_count=0, .mask=0b00000001 },
    .d_bit={ .overriden=true, .overriden_value = 1 }
  },
  {
    .type=Register,
    .operation=OP_OUT,
    .is_accumulator=true,
    .opcode={ .byte_count=0, .match=0b11101110, .mask=0b11111110 },
    .reg={ .overriden=true, .overriden_value = 2 },
    .w_bit={ .byte_count=0, .mask=0b00000001 },
    .d_bit={ .overriden=true, .overriden_value = 1 }
  },
  {
    .type=Memory_Immediate,
    .operation=OP_MOV,
    .is_accumulator=true,
    .opcode={ .byte_count=0, .match=0b10100000, .mask=0b11111110 },
    .w_bit={ .byte_count=0, .mask=0b00000001 },
    .d_bit={ .overriden=true, .overriden_value = 1 }
  },
  {
    .type=Memory_Immediate,
    .operation=OP_MOV,
    .is_accumulator=true,
    .opcode={ .byte_count=0, .match=0b10100010, .mask=0b11111110 },
    .w_bit={ .byte_count=0, .mask=0b00000001 },
  },
  {
    .type=Register_Immediate,
    .operation=OP_ADC,
    .is_accumulator = true,
    .opcode={ .byte_count=0, .match=0b00010100, .mask=0b11111110 },
    .w_bit={ .byte_count=0, .mask=0b00000001 },
  },
  {
    .type=Register_Immediate,
    .operation=OP_TEST,
    .is_accumulator = true,
    .opcode={ .byte_count=0, .match=0b10101000, .mask=0b11111110 },
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
    .opcode={ .byte_count=0, .match=0b00000100, .mask=0b11000110 },
    .w_bit={ .byte_count=0, .mask=0b00000001 },
  },
  {
    .type=Register_Immediate,
    .op_bits={ .byte_count=0, .mask=0b00111000, .shift=3 },
    .is_accumulator = true,
    .opcode={ .byte_count=0, .match=0b00111100, .mask=0b11111110 },
    .w_bit={ .byte_count=0, .mask=0b00000001 },
  },
  {
    .type=Register_Memory,
    .op_bits={ .byte_count=0, .mask=0b00111000, .shift=3 },
    .min_byte_count=2,
    .opcode={ .byte_count=0, .match=0b00000000, .mask=0b11000100 },
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
    .opcode={ .byte_count=0, .match=0b10000000, .mask=0b11000100 },
    .mod={ .byte_count=1, .mask=0b11000000, .shift=6 },
    .rm={ .byte_count=1, .mask=0b00000111 },
    .w_bit={ .byte_count=0, .mask=0b00000001 },
    .s_bit={ .byte_count=0, .mask=0b00000010, .shift=1 }
  },
  {
    .type=Call,
    .operation=OP_CALL,
    .min_byte_count=3,
    .opcode={ .byte_count=0, .match=0b11101000, .mask=0b11111111 }
  },
  {
    .type=Call,
    .operation=OP_CALL,
    .min_byte_count=2,
    .opcode={ .byte_count=0, .match=0b11111111, .mask=0b11111111 },
    .pattern={ .byte_count=1, .match=0b00010000, .mask=0b00111000 },
    .mod={ .byte_count=1, .mask=0b11000000, .shift=6 },
    .rm={ .byte_count=1, .mask=0b00000111 }
  },
};
