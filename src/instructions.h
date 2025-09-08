

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
  Call,
  Return,
  Interrupt,
  FarCall,
};

enum RepPrefix  { RepNone, RepF3, RepF2 };

struct PrefixDefinition {
    u8 opcode;
    enum { PREFIX_SEGMENT, PREFIX_REP, PREFIX_LOCK } type;
    union {
        const char* segment;
        u8 rep_type;
    };
};

static const PrefixDefinition prefix_table[] = {
    // Segment override prefixes
    { 0b00100110, PrefixDefinition::PREFIX_SEGMENT, { .segment = "es" } },
    { 0b00101110, PrefixDefinition::PREFIX_SEGMENT, { .segment = "cs" } },
    { 0b00110110, PrefixDefinition::PREFIX_SEGMENT, { .segment = "ss" } },
    { 0b00111110, PrefixDefinition::PREFIX_SEGMENT, { .segment = "ds" } },
    // REP prefixes
    { 0b11110010, PrefixDefinition::PREFIX_REP, { .rep_type = RepF2 } },
    { 0b11110011, PrefixDefinition::PREFIX_REP, { .rep_type = RepF3 } },
    // LOCK prefix
    { 0b11110000, PrefixDefinition::PREFIX_LOCK, { .rep_type = 0 } },
};

enum InstructionDefFlags {
  FLAG_IS_ACCUMULATOR = 1 << 0,
};

enum InstructionFlags {
  INST_FLAG_USE_V_BIT = 1 << 0,
  INST_FLAG_IS_ACCUMULATOR = 1 << 1,
  INST_FLAG_W_BIT = 1 << 2,
  INST_FLAG_D_BIT = 1 << 3,
  INST_FLAG_S_BIT = 1 << 4,
  INST_FLAG_V_BIT = 1 << 5,
  INST_FLAG_Z_BIT = 1 << 6,
  INST_FLAG_LOCK = 1 << 7,
};

enum Operation {
  OP_AAA,
  OP_AAD,
  OP_AAM,
  OP_AAS,
  OP_ADC,
  OP_ADD,
  OP_AND,
  OP_CALL,
  OP_CALLF,
  OP_CBW,
  OP_CLC,
  OP_CLD,
  OP_CLI,
  OP_CMC,
  OP_CMP,
  OP_CMPS,
  OP_CWD,
  OP_DAA,
  OP_DAS,
  OP_DEC,
  OP_DIV,
  OP_HLT,
  OP_IDIV,
  OP_IMUL,
  OP_IN,
  OP_INC,
  OP_INT,
  OP_INT3,
  OP_INTO,
  OP_IRET,
  OP_JB,
  OP_JBE,
  OP_JCXZ,
  OP_JE,
  OP_JL,
  OP_JLE,
  OP_JNB,
  OP_JNBE,
  OP_JNE,
  OP_JNL,
  OP_JNLE,
  OP_JNO,
  OP_JNP,
  OP_JNS,
  OP_JO,
  OP_JP,
  OP_JS,
  OP_JMP,
  OP_JMPF,
  OP_LAHF,
  OP_LDS,
  OP_LEA,
  OP_LES,
  OP_LODS,
  OP_LOOP,
  OP_LOOPNZ,
  OP_LOOPZ,
  OP_MOV,
  OP_MOVS,
  OP_MUL,
  OP_NEG,
  OP_NOP,
  OP_NOT,
  OP_OR,
  OP_OUT,
  OP_POP,
  OP_POPF,
  OP_PUSH,
  OP_PUSHF,
  OP_RCL,
  OP_RCR,
  OP_RET,
  OP_RETF,
  OP_ROL,
  OP_ROR,
  OP_SAHF,
  OP_SAR,
  OP_SBB,
  OP_SCAS,
  OP_SHL,
  OP_SHR,
  OP_STC,
  OP_STD,
  OP_STI,
  OP_STOS,
  OP_SUB,
  OP_TEST,
  OP_WAIT,
  OP_XCHG,
  OP_XLAT,
  OP_XOR
};

struct CpuInstructionDefinition {
  InstructionType type;
  BitsLocation op_bits;
  Operation operation;
  u8 min_byte_count;
  u8 flags;
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
  Operation operation;

  const char* source;
  const char* dest;
  const char* segment_reg;
  const char* segment_override;
  u8 rm;
  u8 mod;
  u8 flags;
  u16 displacement;
  u16 immediate;
  u16 segment;
  const char *effective_address;
  i16 address_offset;

  u8 byte_len;
  u16 next_ip;
  u8 rep_prefix;
};

#define GET_W_BIT(inst) ((inst).flags & INST_FLAG_W_BIT ? 1 : 0)
#define GET_D_BIT(inst) ((inst).flags & INST_FLAG_D_BIT ? 1 : 0)
#define GET_S_BIT(inst) ((inst).flags & INST_FLAG_S_BIT ? 1 : 0)
#define GET_V_BIT(inst) ((inst).flags & INST_FLAG_V_BIT ? 1 : 0)
#define GET_Z_BIT(inst) ((inst).flags & INST_FLAG_Z_BIT ? 1 : 0)
#define HAS_LOCK(inst) ((inst).flags & INST_FLAG_LOCK ? 1 : 0)

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

static const char* operation_strings[] = {
  "aaa",
  "aad",
  "aam",
  "aas",
  "adc",
  "add",
  "and",
  "call",
  "call",
  "cbw",
  "clc",
  "cld",
  "cli",
  "cmc",
  "cmp",
  "cmps",
  "cwd",
  "daa",
  "das",
  "dec",
  "div",
  "hlt",
  "idiv",
  "imul",
  "in",
  "inc",
  "int",
  "int3",
  "into",
  "iret",
  "jb",
  "jbe",
  "jcxz",
  "je",
  "jl",
  "jle",
  "jnb",
  "jnbe",
  "jne",
  "jnl",
  "jnle",
  "jno",
  "jnp",
  "jns",
  "jo",
  "jp",
  "js",
  "jmp",
  "jmp",
  "lahf",
  "lds",
  "lea",
  "les",
  "lods",
  "loop",
  "loopnz",
  "loopz",
  "mov",
  "movs",
  "mul",
  "neg",
  "nop",
  "not",
  "or",
  "out",
  "pop",
  "popf",
  "push",
  "pushf",
  "rcl",
  "rcr",
  "ret",
  "retf",
  "rol",
  "ror",
  "sahf",
  "sar",
  "sbb",
  "scas",
  "shl",
  "shr",
  "stc",
  "std",
  "sti",
  "stos",
  "sub",
  "test",
  "wait",
  "xchg",
  "xlat",
  "xor"
};

static inline const char* operation_to_string(Operation op) {
  if (op >= 0 && op < TABLE_LEN(operation_strings)) {
    return operation_strings[op];
  }
  return "unknown";
}
