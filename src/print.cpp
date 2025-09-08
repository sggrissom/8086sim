
static void print_segment_prefix(const char* segment_override) {
    if (segment_override) {
        printf("%s:", segment_override);
    }
}

static void print_lock_prefix(CpuInstruction inst) {
    if (HAS_LOCK(inst)) {
        printf("lock ");
    }
}

static void print_memory_operand(const char* base, i16 displacement, const char* segment_override) {
    print_segment_prefix(segment_override);
    if (displacement > 0) {
        printf("[%s + %d]", base, displacement);
    } else if (displacement < 0) {
        printf("[%s - %d]", base, -displacement);
    } else {
        printf("[%s]", base);
    }
}

static void print_direct_memory(u16 address, const char* segment_override) {
    print_segment_prefix(segment_override);
    printf("[%d]", address);
}

static void print_relative_jump(CpuInstruction inst) {
    i16 rel = (i8)inst.address_offset;
    u16 target = (u16)(inst.next_ip + rel);
    i16 delta = (i16)(target - inst.instruction_address);

    if (delta > 0) {
        printf("$+%d+0", delta);
    } else if (delta == 0) {
        printf("$+0");
    } else {
        printf("$%d+0", delta);
    }
}

static void print_width_prefix(bool is_word) {
    printf("%s ", is_word ? "word" : "byte");
}

void print_address(CpuInstruction inst) {
    print_memory_operand(inst.dest, inst.displacement, inst.segment_override);
}

static void print_v_bit_clause(CpuInstruction inst) {
    if (!(inst.flags & INST_FLAG_USE_V_BIT)) {
        printf("\n");
        return;
    }
    printf(", %s\n", GET_V_BIT(inst) == 0 ? "1" : "cl");
}

void print_instruction(CpuInstruction inst) {
  switch (inst.type) {
    case Solo: {
        bool is_string = (inst.operation == OP_MOVS) ||
                        (inst.operation == OP_CMPS) ||
                        (inst.operation == OP_SCAS) ||
                        (inst.operation == OP_LODS) ||
                        (inst.operation == OP_STOS);

        if (is_string) {
            const char* suffix = GET_W_BIT(inst) ? "w" : "b";
            printf("rep %s%s\n", operation_to_string(inst.operation), suffix);
        } else {
            printf("%s\n", operation_to_string(inst.operation));
        }
        break;
    }
    case Register: {
        if (inst.segment_reg) {
            printf("%s %s\n", operation_to_string(inst.operation), inst.segment_reg);
        } else if (inst.source && (inst.flags & INST_FLAG_IS_ACCUMULATOR)) {
            const char* accumulator = GET_W_BIT(inst) ? "ax" : "al";
            if (GET_D_BIT(inst)) {
                printf("%s %s, %s\n", operation_to_string(inst.operation), inst.source, accumulator);
            } else {
                printf("%s %s, %s\n", operation_to_string(inst.operation), accumulator, inst.source);
            }
        } else if (inst.source) {
            printf("%s %s\n", operation_to_string(inst.operation), inst.source);
        }
        break;
    }
    case Memory: {
        print_lock_prefix(inst);
        printf("%s ", operation_to_string(inst.operation));

        if (inst.source) {
            printf("%s", inst.source);
        } else {
            if (inst.operation != OP_JMP) {
                print_width_prefix(GET_W_BIT(inst));
            }

            if (inst.displacement && !inst.effective_address) {
                print_direct_memory(inst.displacement, inst.segment_override);
            } else if (inst.effective_address) {
                print_memory_operand(inst.effective_address, inst.address_offset, inst.segment_override);
            }
        }
        print_v_bit_clause(inst);
        break;
    }
    case ConditionalJump: {
        printf("%s ", operation_to_string(inst.operation));
        print_relative_jump(inst);
        printf("\n");
        break;
    }
    case Register_Memory: {
        print_lock_prefix(inst);
        printf("%s ", operation_to_string(inst.operation));

        bool is_test = (inst.operation == OP_TEST);
        bool is_xchg = (inst.operation == OP_XCHG);

        if (inst.mod == 0b11) {
            // Register to register
            if (is_xchg && GET_D_BIT(inst) == 0) {
                printf("%s, %s\n", inst.dest, inst.source);
            } else {
                printf("%s, %s\n", inst.source, inst.dest);
            }
        } else if (inst.rm == 0b110 && inst.mod == 0b00) {
            // Direct memory addressing
            if (is_test) {
                print_direct_memory((u16)inst.displacement, inst.segment_override);
                printf(", %s\n", inst.source);
            } else {
                printf("%s, ", inst.source);
                print_direct_memory((u16)inst.displacement, inst.segment_override);
                printf("\n");
            }
        } else {
            // Memory with effective address
            if (is_test) {
                print_address(inst);
                printf(", %s\n", inst.source);
            } else if (is_xchg && GET_D_BIT(inst) == 0) {
                printf("%s, ", inst.dest);
                print_memory_operand(inst.source, inst.displacement, inst.segment_override);
                printf("\n");
            } else if (GET_D_BIT(inst) == 0b0) {
                print_address(inst);
                printf(", %s\n", inst.source);
            } else {
                printf("%s, ", inst.source);
                print_address(inst);
                printf("\n");
            }
        }
        break;
    }
    case Register_Immediate: {
        print_lock_prefix(inst);
        printf("%s ", operation_to_string(inst.operation));

        bool needs_mem_imm_order = (inst.operation == OP_TEST) || 
                                   (inst.operation == OP_CMP) ||
                                   (inst.operation == OP_AND) ||
                                   (inst.operation == OP_OR) ||
                                   (inst.operation == OP_XOR);

        if (inst.rm == 0b110 && inst.mod == 0b00) {
            // Direct memory addressing
            if (needs_mem_imm_order) {
                print_direct_memory(inst.displacement, inst.segment_override);
                printf(", ");
                print_width_prefix(GET_W_BIT(inst));
                printf("%d\n", inst.immediate);
            } else {
                print_direct_memory(inst.displacement, inst.segment_override);
                printf(", ");
                print_width_prefix(GET_W_BIT(inst));
                printf("%d\n", inst.immediate);
            }
        } else if (inst.source) {
            // Register operand
            if (GET_D_BIT(inst)) {
                printf("%d, %s\n", inst.immediate, inst.source);
            } else {
                if (GET_W_BIT(inst)) {
                    printf("%s, %d\n", inst.source, (i16)inst.immediate);
                } else {
                    printf("%s, %d\n", inst.source, (i8)inst.immediate);
                }
            }
        } else if (inst.effective_address) {
            // Memory with effective address
            if (needs_mem_imm_order) {
                print_memory_operand(inst.effective_address, inst.displacement, inst.segment_override);
                printf(", ");
                print_width_prefix(GET_W_BIT(inst));
                printf("%d\n", inst.immediate);
            } else {
                print_memory_operand(inst.effective_address, inst.displacement, inst.segment_override);
                printf(", ");
                print_width_prefix(GET_W_BIT(inst));
                printf("%d\n", inst.immediate);
            }
        } else if (inst.flags & INST_FLAG_IS_ACCUMULATOR) {
            const char* accumulator = GET_W_BIT(inst) ? "ax" : "al";
            if (GET_W_BIT(inst)) {
                printf("%s, %d\n", accumulator, (i16)inst.immediate);
            } else {
                printf("%s, %d\n", accumulator, (i8)inst.immediate);
            }
        }
        break;
    }
    case Memory_Immediate: {
        if (inst.flags & INST_FLAG_IS_ACCUMULATOR) {
            printf("%s ", operation_to_string(inst.operation));
            if (GET_D_BIT(inst)) {
                printf("ax, ");
                print_direct_memory(inst.immediate, inst.segment_override);
            } else {
                print_direct_memory(inst.immediate, inst.segment_override);
                printf(", ax");
            }
            printf("\n");
        }
        break;
    }
    case Call: {
        printf("%s ", operation_to_string(inst.operation));

        if (inst.address_offset != 0) {
            print_relative_jump(inst);
        } else if (inst.source) {
            printf("%s", inst.source);
        } else if (inst.effective_address) {
            print_memory_operand(inst.effective_address, (i16)inst.displacement, inst.segment_override);
        } else if (inst.displacement) {
            print_direct_memory((u16)inst.displacement, inst.segment_override);
        }
        printf("\n");
        break;
    }
    case Return: {
        printf("%s", operation_to_string(inst.operation));
        if (inst.immediate != 0) {
            printf(" %d", (i16)inst.immediate);
        }
        printf("\n");
        break;
    }
    case Interrupt: {
        printf("%s %d\n", operation_to_string(inst.operation), inst.immediate);
        break;
    }
    case FarCall: {
        printf("%s %d:%d\n", operation_to_string(inst.operation), inst.segment, inst.immediate);
        break;
    }
    default:
      break;
  }
}
