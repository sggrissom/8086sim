
#include "registers.h"
#include "instructions.h"
#include "instructions_table.h"

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

static inline u16 read_u16(MemoryReader* r) {
  u8 lo, hi;
  read(r, &lo);
  read(r, &hi);
  return (u16)((hi << 8) | lo);
}

static Operation alu_op_to_enum[8] = {
  OP_ADD, OP_OR, OP_ADC, OP_SBB, OP_AND, OP_SUB, OP_XOR, OP_CMP
};

static inline Operation get_op(u8 op) {
  return (op < 8) ? alu_op_to_enum[op] : OP_ADD;
}

static inline bool is_direct_memory_addressing(u8 mod, u8 rm) {
  return (mod == 0b00 && rm == 0b110);
}

static void handle_mod_rm_displacement(CpuInstruction* inst, const CpuInstructionDefinition* d, MemoryReader* r) {
  if (is_direct_memory_addressing(inst->mod, inst->rm)) {
    inst->displacement = (i16)read_u16(r);
    inst->effective_address = NULL;
  } else if (inst->mod == 0b01) {
    inst->displacement = get_displacement(0, r);
  } else if (inst->mod == 0b10) {
    inst->displacement = get_displacement(1, r);
  }
}

static void swap_source_dest_registers(CpuInstruction* inst) {
  RegisterId temp_reg = inst->source_reg_id;
  inst->source_reg_id = inst->dest_reg_id;
  inst->dest_reg_id = temp_reg;

  bool source_has = inst->flags & INST_FLAG_HAS_SOURCE_REG;
  bool dest_has = inst->flags & INST_FLAG_HAS_DEST_REG;
  inst->flags &= ~(INST_FLAG_HAS_SOURCE_REG | INST_FLAG_HAS_DEST_REG);
  if (dest_has) inst->flags |= INST_FLAG_HAS_SOURCE_REG;
  if (source_has) inst->flags |= INST_FLAG_HAS_DEST_REG;

  bool source_is_reg = inst->flags & INST_FLAG_SOURCE_IS_REGISTER;
  bool dest_is_reg = inst->flags & INST_FLAG_DEST_IS_REGISTER;
  inst->flags &= ~(INST_FLAG_SOURCE_IS_REGISTER | INST_FLAG_DEST_IS_REGISTER);
  if (dest_is_reg) inst->flags |= INST_FLAG_SOURCE_IS_REGISTER;
  if (source_is_reg) inst->flags |= INST_FLAG_DEST_IS_REGISTER;
}

const PrefixDefinition * get_matching_prefix(u8 opcode) {
  for (size_t i = 0; i < TABLE_LEN(prefix_table); i++) {
    const PrefixDefinition* p = &prefix_table[i];
    if (opcode == p->opcode) {
      return p;
    }
  }
  return NULL;
}

CpuInstruction decode_instruction(u8 opcode, MemoryReader *r) {
  u16 start_ip = (r->ip > 0) ? (u16)(r->ip - 1) : 0;

  u8 prefix = RepNone;
  const char* segment_override = NULL;
  bool has_lock = false;

  // Process all prefix bytes (can have multiple)
  const PrefixDefinition* p = get_matching_prefix(opcode);
  while (p != NULL) {
    if (p->type == PrefixDefinition::PREFIX_SEGMENT) {
      segment_override = p->segment;
    } else if (p->type == PrefixDefinition::PREFIX_REP) {
      prefix = p->rep_type;
    } else if (p->type == PrefixDefinition::PREFIX_LOCK) {
      has_lock = true;
    }
    read(r, &opcode);
    p = get_matching_prefix(opcode);
  }

  u8 bytes[6] = {opcode};
  for (size_t i = 0; i < TABLE_LEN(instruction_table); i++) {
    const CpuInstructionDefinition *d = &instruction_table[i];

    bool opcode_matches = (opcode & d->opcode.mask) == d->opcode.match;
    if (opcode_matches && d->pattern.mask != 0) {
      if (d->pattern.byte_count > 0) {
        peek(r, &bytes[d->pattern.byte_count]);
      }
      opcode_matches = (bytes[d->pattern.byte_count] & d->pattern.mask) == d->pattern.match;
    }
    if (d->op_bits.mask != 0) {
      if (d->op_bits.byte_count > 0) {
        peek(r, &bytes[d->op_bits.byte_count]);
      }
      u8 op = get_bits(bytes, r, d->op_bits);
      Operation operation = get_op(op);
    }
    if (opcode_matches) {
      CpuInstruction inst = {
        .instruction_address = start_ip,
        .type = d->type,
        .operation = d->operation,
        .segment_override = segment_override,
        .source_reg_id = REG_AL,
        .dest_reg_id = REG_AL,
        .segment_reg_id = 255,
        .flags = has_lock ? (u16)INST_FLAG_LOCK : (u16)0,
        .rep_prefix = prefix,
      };

      for (int i = 1; i < d->min_byte_count; ++i) {
        read(r, &bytes[i]);
      }

      if (d->op_bits.mask != 0) {
        u8 op = get_bits(bytes, r, d->op_bits);
        inst.operation = get_op(op);
      }
      if (d->segment_register.mask != 0) {
        inst.segment_reg_id = get_bits(bytes, r, d->segment_register);
      }
      if (d->w_bit.mask != 0 || d->w_bit.overriden) {
        if (get_bits(bytes, r, d->w_bit)) {
          inst.flags |= INST_FLAG_W_BIT;
        }
      }
      if (d->d_bit.mask != 0 || d->d_bit.overriden) {
        if (get_bits(bytes, r, d->d_bit)) {
          inst.flags |= INST_FLAG_D_BIT;
        }
      }
      if (d->s_bit.mask != 0) {
        if (get_bits(bytes, r, d->s_bit)) {
          inst.flags |= INST_FLAG_S_BIT;
        }
      }
      if (d->v_bit.mask != 0) {
        inst.flags |= INST_FLAG_USE_V_BIT;
        if (get_bits(bytes, r, d->v_bit)) {
          inst.flags |= INST_FLAG_V_BIT;
        }
      }
      if (d->z_bit.mask != 0) {
        if (get_bits(bytes, r, d->z_bit)) {
          inst.flags |= INST_FLAG_Z_BIT;
        }
      }
      if (d->reg.mask != 0 || d->reg.overriden) {
        u8 wide_source = (d->flags & FLAG_IS_ACCUMULATOR) ? 1 : GET_W_BIT(inst);
        u8 reg_bits = get_bits(bytes, r, d->reg);
        inst.source_reg_id = (RegisterId)(reg_bits + 8 * wide_source);
        inst.flags |= INST_FLAG_HAS_SOURCE_REG | INST_FLAG_SOURCE_IS_REGISTER;
      }
      if (d->mod.mask != 0) {
        inst.mod = get_bits(bytes, r, d->mod);
      }
      if (d->rm.mask != 0) {
        inst.rm = get_bits(bytes, r, d->rm);
        inst.effective_address = effective_address[inst.rm];
      }

      if (inst.type == Register_Immediate) {
        if (inst.mod == 0b11) {
          inst.source_reg_id = (RegisterId)(inst.rm + 8 * GET_W_BIT(inst));
          inst.flags |= INST_FLAG_HAS_SOURCE_REG | INST_FLAG_SOURCE_IS_REGISTER;
        } else if (is_direct_memory_addressing(inst.mod, inst.rm)) {
          inst.displacement = get_immediate(GET_W_BIT(inst), 0, r);
        } else {
          handle_mod_rm_displacement(&inst, d, r);
        }

        u8 wide_imm = d->reg.overriden ? 0 : GET_W_BIT(inst);
        inst.immediate = get_immediate(wide_imm && !GET_S_BIT(inst), GET_S_BIT(inst), r);
      }
      if (inst.type == Memory) {
        if (inst.mod == 0b11) {
          inst.source_reg_id = (RegisterId)(inst.rm + 8 * GET_W_BIT(inst));
          inst.flags |= INST_FLAG_HAS_SOURCE_REG | INST_FLAG_SOURCE_IS_REGISTER;
        } else if (is_direct_memory_addressing(inst.mod, inst.rm)) {
          inst.displacement = (i16)read_u16(r);
          inst.effective_address = nullptr;
        } else if (inst.mod == 0b01) {
          inst.address_offset = get_displacement(0, r);
        } else if (inst.mod == 0b10) {
          inst.address_offset = get_displacement(1, r);
        }
      }
      if (d->type == ConditionalJump) {
        if (d->min_byte_count == 3) {
          inst.address_offset = (i16)((bytes[2] << 8) | bytes[1]);
        } else {
          inst.address_offset = bytes[1];
        }
      }
      if (d->type == Call) {
        if (d->min_byte_count == 3) {
          inst.address_offset = (i16)((bytes[2] << 8) | bytes[1]);
        } else {
          if (inst.mod == 0b11) {
            inst.source_reg_id = (RegisterId)(inst.rm + 8);
            inst.flags |= INST_FLAG_HAS_SOURCE_REG | INST_FLAG_SOURCE_IS_REGISTER;
          } else {
            inst.effective_address = effective_address[inst.rm];
            handle_mod_rm_displacement(&inst, d, r);
          }
        }
      }
      if (d->type == Return) {
        inst.immediate = (u16)((bytes[2] << 8) | bytes[1]);
      }
      if (d->type == Interrupt) {
        inst.immediate = (u16)bytes[1];
      }
      if (d->type == FarCall) {
        inst.immediate = (u16)((bytes[2] << 8) | bytes[1]);
        inst.segment = (u16)((bytes[4] << 8) | bytes[3]);
      }
      if (inst.type == Register_Memory) {
        if (inst.mod == 0b11) {
          inst.dest_reg_id = (RegisterId)(inst.rm + 8 * GET_W_BIT(inst));
          inst.flags |= INST_FLAG_HAS_DEST_REG | INST_FLAG_DEST_IS_REGISTER;
          if (!GET_D_BIT(inst)) {
            swap_source_dest_registers(&inst);
          }
        } else {
          handle_mod_rm_displacement(&inst, d, r);
        }
      }
      if (d->flags & FLAG_IS_ACCUMULATOR) {
        inst.flags |= INST_FLAG_IS_ACCUMULATOR;
      }
      if (inst.type == Memory_Immediate) {
        inst.immediate = get_immediate(GET_W_BIT(inst), 0, r);
      }

      inst.byte_len = (u8)(r->ip - start_ip);
      inst.next_ip  = (u16)(start_ip + inst.byte_len);
      return inst;
    }
  }


  return {
    .instruction_address = start_ip,
      .operation = OP_NOP,
      .byte_len = 1,
      .next_ip  = (u16)(start_ip + 1)
  };
}
