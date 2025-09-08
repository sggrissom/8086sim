
void print_address(CpuInstruction inst) {
    if (inst.displacement > 0) {
      printf("[%s + %hd]", inst.dest, inst.displacement);
    } else if (inst.displacement < 0) {
      printf("[%s - %hd]", inst.dest, -inst.displacement);
    } else {
      printf("[%s]", inst.dest);
    }
}

void print_v_bit_clause(CpuInstruction inst) {
  if (!(inst.flags & INST_FLAG_USE_V_BIT)) {
    printf("\n");
    return;
  }
  if (GET_V_BIT(inst) == 0) {
    printf(", 1\n");
  } else {
    printf(", cl\n");
  }
}

void print_instruction(CpuInstruction inst) {
  switch (inst.type) {
    case Solo: {
      bool is_string =
        (inst.operation == OP_MOVS) ||
        (inst.operation == OP_CMPS) ||
        (inst.operation == OP_SCAS) ||
        (inst.operation == OP_LODS) ||
        (inst.operation == OP_STOS);

      if (is_string) {
        const char* suffix = (GET_W_BIT(inst) ? "w" : "b");
        printf("rep %s%s\n", operation_to_string(inst.operation), suffix);
      } else {
        printf("%s\n", operation_to_string(inst.operation));
      }
      break;
    }
    case Register:
      {
        if (inst.segment_reg) {
          printf("%s %s\n", operation_to_string(inst.operation), inst.segment_reg);
        }
        else if (inst.source && (inst.flags & INST_FLAG_IS_ACCUMULATOR)) {
          const char * accumulator = GET_W_BIT(inst) ? "ax" : "al";
          if (GET_D_BIT(inst)) {
            printf("%s %s, %s\n", operation_to_string(inst.operation), inst.source, accumulator);
          } else {
            printf("%s %s, %s\n", operation_to_string(inst.operation), accumulator, inst.source);
          }
        }
        else if (inst.source) {
          printf("%s %s\n", operation_to_string(inst.operation), inst.source);
        }
        break;
      }
    case Memory:
      {
        const char * width = GET_W_BIT(inst) ? "word " : "byte ";
        if (inst.operation == OP_JMP) {
          width = "";
        }
        if (inst.displacement) {
          printf("%s %s[%d]", operation_to_string(inst.operation), width, inst.displacement);
        } else if (inst.address_offset != 0) {
          printf("%s %s[%s + %hd]", operation_to_string(inst.operation), width, inst.effective_address, inst.address_offset);
        } else if (inst.source) {
          printf("%s %s", operation_to_string(inst.operation), inst.source);
        } else {
          printf("%s %s[%s]", operation_to_string(inst.operation), width, inst.effective_address);
        }
        print_v_bit_clause(inst);
        break;
      }
    case ConditionalJump: {
      i16 rel = (i8)inst.address_offset;
      u16 target = (u16)(inst.next_ip + rel);
      i16 delta  = (i16)(target - inst.instruction_address);

      if (delta > 0) {
        printf("%s $+%d+0\n", operation_to_string(inst.operation), delta);
      } else if (delta == 0) {
        printf("%s $+0\n", operation_to_string(inst.operation));
      } else {
        printf("%s $%d+0\n", operation_to_string(inst.operation), delta);
      }
      break;
    }
    case Register_Memory:
      {
        if (inst.mod == 0b11) {
          printf("%s %s, %s\n", operation_to_string(inst.operation), inst.source, inst.dest);
        } else if (inst.rm == 0b110 && inst.mod == 0b00) {
          printf("%s %s, [%u]\n", operation_to_string(inst.operation), inst.source, (u16)inst.displacement);
        } else if (GET_D_BIT(inst) == 0b0) {
          printf("%s ", operation_to_string(inst.operation));
          print_address(inst);
          printf(", %s\n", inst.source);
        } else {
          printf("%s %s, ", operation_to_string(inst.operation), inst.source);
          print_address(inst);
          printf("\n");
        }
        break;
      }
    case Register_Immediate:
      {
        if (inst.rm == 0b110 && inst.mod == 0b00) {
          const char * width = GET_W_BIT(inst) ? "word" : "byte";
          printf("%s [%d], %s %d\n", operation_to_string(inst.operation), inst.displacement, width, inst.immediate);
        }
        else if (inst.source) {
          if (GET_D_BIT(inst)) {
            printf("%s %d, %s\n", operation_to_string(inst.operation), inst.immediate, inst.source);
          } else {
            printf("%s %s, %d\n", operation_to_string(inst.operation), inst.source, inst.immediate);
          }
        }
        else if (inst.effective_address) {
          char source[20] = {};
          if (inst.displacement != 0) {
            sprintf(source, "[%s + %hd]", inst.effective_address, inst.displacement);
          } else {
            sprintf(source, "[%s]", inst.effective_address);
          }
          const char * width = GET_W_BIT(inst) ? "word" : "byte";
          printf("%s %s, %s %d\n", operation_to_string(inst.operation), source, width, inst.immediate);
        } else if ((inst.flags & INST_FLAG_IS_ACCUMULATOR)) {
          const char * accumulator = GET_W_BIT(inst) ? "ax" : "al";
          printf("%s %s, %d\n", operation_to_string(inst.operation), accumulator, inst.immediate);
        }
        break;
      }
    case Memory_Immediate:
      {
        if ((inst.flags & INST_FLAG_IS_ACCUMULATOR)) {
          if (GET_D_BIT(inst)) {
            printf("%s ax, [%d]\n", operation_to_string(inst.operation), inst.immediate);
          } else {
            printf("%s [%d], ax\n", operation_to_string(inst.operation), inst.immediate);
          }
        }
        break;
      }
    case Call:
      {
        if (inst.address_offset != 0) {
          u16 target = (u16)(inst.next_ip + inst.address_offset);
          i16 delta  = (i16)(target - inst.instruction_address);
          if (delta > 0) {
            printf("%s $+%d+0\n", operation_to_string(inst.operation), delta);
          } else if (delta == 0) {
            printf("%s $+0\n", operation_to_string(inst.operation));
          } else {
            printf("%s $%d+0\n", operation_to_string(inst.operation), delta);
          }
        } else if (inst.source) {
          printf("%s %s\n", operation_to_string(inst.operation), inst.source);
        } else if (inst.displacement && inst.effective_address) {
          if ((i16)inst.displacement > 0) {
            printf("%s [%s + %d]\n", operation_to_string(inst.operation), inst.effective_address, (i16)inst.displacement);
          } else if ((i16)inst.displacement < 0) {
            printf("%s [%s - %d]\n", operation_to_string(inst.operation), inst.effective_address, -(i16)inst.displacement);
          } else {
            printf("%s [%s]\n", operation_to_string(inst.operation), inst.effective_address);
          }
        } else if (inst.displacement) {
          printf("%s [%d]\n", operation_to_string(inst.operation), (u16)inst.displacement);
        } else if (inst.effective_address) {
          printf("%s [%s]\n", operation_to_string(inst.operation), inst.effective_address);
        }
        break;
      }
    case Return:
      {
        if (inst.immediate != 0) {
          printf("%s %d\n", operation_to_string(inst.operation), (i16)inst.immediate);
        } else {
          printf("%s\n", operation_to_string(inst.operation));
        }
        break;
      }
    case Interrupt:
      {
        printf("%s %d\n", operation_to_string(inst.operation), inst.immediate);
        break;
      }
    case FarCall:
      {
        printf("%s %d:%d\n", operation_to_string(inst.operation), inst.segment, inst.immediate);
        break;
      }
    default:
      break;
  }
}
