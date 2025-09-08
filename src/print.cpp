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
    if ((inst.operation == OP_JMP || inst.operation == OP_CALL) && inst.byte_len == 3) {
        u16 target = (u16)(inst.next_ip + inst.address_offset);
        printf("%d", target);
        return;
    }

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

static bool is_direct_addressing(CpuInstruction inst) {
    return inst.rm == 0b110 && inst.mod == 0b00;
}

static void print_immediate_value(CpuInstruction inst) {
    if (GET_W_BIT(inst)) {
        printf("%d", (i16)inst.immediate);
    } else {
        printf("%d", (i8)inst.immediate);
    }
}

static void print_memory_with_width(CpuInstruction inst) {
    print_width_prefix(GET_W_BIT(inst));
    if (inst.effective_address) {
        print_memory_operand(inst.effective_address, inst.displacement, inst.segment_override);
    } else {
        print_direct_memory((u16)inst.displacement, inst.segment_override);
    }
}

static void print_segment_register_case(CpuInstruction inst) {
    if (inst.mod == 0b11) {
        if (GET_D_BIT(inst) == 0) {
            printf("%s, %s\n", inst.dest, inst.segment_reg);
        } else {
            printf("%s, %s\n", inst.segment_reg, inst.dest);
        }
    } else if (is_direct_addressing(inst)) {
        if (GET_D_BIT(inst) == 0) {
            print_direct_memory((u16)inst.displacement, inst.segment_override);
            printf(", %s\n", inst.segment_reg);
        } else {
            printf("%s, ", inst.segment_reg);
            print_direct_memory((u16)inst.displacement, inst.segment_override);
            printf("\n");
        }
    } else {
        if (GET_D_BIT(inst) == 0) {
            print_address(inst);
            printf(", %s\n", inst.segment_reg);
        } else {
            printf("%s, ", inst.segment_reg);
            print_address(inst);
            printf("\n");
        }
    }
}

static void print_register_to_register(CpuInstruction inst) {
    bool is_xchg = (inst.operation == OP_XCHG);
    if (is_xchg && GET_D_BIT(inst) == 0) {
        printf("%s, %s\n", inst.dest, inst.source);
    } else {
        printf("%s, %s\n", inst.source, inst.dest);
    }
}

static void print_register_memory_case(CpuInstruction inst) {
    bool is_test = (inst.operation == OP_TEST);
    bool is_xchg = (inst.operation == OP_XCHG);

    if (is_direct_addressing(inst)) {
        if (is_test) {
            print_direct_memory((u16)inst.displacement, inst.segment_override);
            printf(", %s\n", inst.source);
        } else {
            printf("%s, ", inst.source);
            print_direct_memory((u16)inst.displacement, inst.segment_override);
            printf("\n");
        }
    } else {
        if (is_test) {
            print_address(inst);
            printf(", %s\n", inst.source);
        } else if (is_xchg && GET_D_BIT(inst) == 0) {
            printf("%s, ", inst.dest);
            print_memory_operand(inst.source, inst.displacement, inst.segment_override);
            printf("\n");
        } else if (GET_D_BIT(inst) == 0) {
            print_address(inst);
            printf(", %s\n", inst.source);
        } else {
            printf("%s, ", inst.source);
            print_address(inst);
            printf("\n");
        }
    }
}

static void print_register_memory(CpuInstruction inst) {
    print_lock_prefix(inst);
    printf("%s ", operation_to_string(inst.operation));

    if (inst.segment_reg != NULL) {
        print_segment_register_case(inst);
    } else if (inst.mod == 0b11) {
        print_register_to_register(inst);
    } else {
        print_register_memory_case(inst);
    }
}

static bool needs_memory_immediate_order(Operation op) {
    return (op == OP_TEST) || (op == OP_CMP) || (op == OP_AND) || (op == OP_OR) || (op == OP_XOR);
}

static void print_memory_immediate(CpuInstruction inst) {
    if (is_direct_addressing(inst)) {
        print_direct_memory(inst.displacement, inst.segment_override);
    } else if (inst.effective_address) {
        print_memory_operand(inst.effective_address, inst.displacement, inst.segment_override);
    }
    printf(", ");
    print_width_prefix(GET_W_BIT(inst));
    print_immediate_value(inst);
    printf("\n");
}

static void print_register_immediate_case(CpuInstruction inst) {
    if (inst.source) {
        if (GET_D_BIT(inst)) {
            printf("%d, %s\n", inst.immediate, inst.source);
        } else {
            printf("%s, ", inst.source);
            print_immediate_value(inst);
            printf("\n");
        }
    } else if (inst.effective_address || is_direct_addressing(inst)) {
        print_memory_immediate(inst);
    } else if (inst.flags & INST_FLAG_IS_ACCUMULATOR) {
        const char* accumulator = GET_W_BIT(inst) ? "ax" : "al";
        printf("%s, ", accumulator);
        print_immediate_value(inst);
        printf("\n");
    }
}

static void print_register_immediate(CpuInstruction inst) {
    print_lock_prefix(inst);
    printf("%s ", operation_to_string(inst.operation));
    print_register_immediate_case(inst);
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
            if (inst.operation == OP_JMPF) {
                printf("jmp far ");
            } else {
                printf("%s ", operation_to_string(inst.operation));
            }

            if (inst.source) {
                printf("%s", inst.source);
            } else {
                if (inst.operation != OP_JMP && inst.operation != OP_JMPF) {
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
            print_register_memory(inst);
            break;
        }
        case Register_Immediate: {
            print_register_immediate(inst);
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
            if (inst.operation == OP_CALLF) {
                printf("call far ");
            } else {
                printf("%s ", operation_to_string(inst.operation));
            }

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
