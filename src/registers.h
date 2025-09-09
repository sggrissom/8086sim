
enum RegisterId {
  REG_AL = 0, REG_CL = 1, REG_DL = 2, REG_BL = 3,
  REG_AH = 4, REG_CH = 5, REG_DH = 6, REG_BH = 7,
  REG_AX = 8, REG_CX = 9, REG_DX = 10, REG_BX = 11,
  REG_SP = 12, REG_BP = 13, REG_SI = 14, REG_DI = 15
};

static const char* register_file[16] = {
  "al", "cl", "dl", "bl",
  "ah", "ch", "dh", "bh",
  "ax", "cx", "dx", "bx",
  "sp", "bp", "si", "di",
};

static const char* segment_register[4] = {
  "es", "cs", "ss", "ds"
};

RegisterId get_register_id_from_name(const char* reg_name) {
  if (!reg_name) return REG_AL;
  
  for (int i = 0; i < 16; i++) {
    if (strcmp(register_file[i], reg_name) == 0) {
      return (RegisterId)i;
    }
  }
  return REG_AL;
}

const char* get_register_name_from_id(RegisterId reg_id) {
  if (reg_id >= 0 && reg_id < 16) {
    return register_file[reg_id];
  }
  return "unknown";
}
