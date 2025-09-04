
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
  if (inst.use_v_bit == 0) {
    printf("\n");
    return;
  }
  if (inst.v_bit == 0) {
    printf(", 1\n");
  } else {
    printf(", cl\n");
  }
}

void print_instruction(CpuInstruction inst) {
  switch (inst.type) {
    case Solo:
      {
        printf("%s\n", inst.operation);
        break;
      }
    case Register:
      {
        if (inst.segment_reg) {
          printf("%s %s\n", inst.operation, inst.segment_reg);
        }
        else if (inst.source && inst.is_accumulator) {
          const char * accumulator = inst.w_bit ? "ax" : "al";
          if (inst.d_bit) {
            printf("%s %s, %s\n", inst.operation, inst.source, accumulator);
          } else {
            printf("%s %s, %s\n", inst.operation, accumulator, inst.source);
          }
        }
        else if (inst.source) {
          printf("%s %s\n", inst.operation, inst.source);
        }
        break;
      }
    case Memory:
      {
        const char * width = inst.w_bit ? "word" : "byte";
        if (inst.displacement) {
          printf("%s %s [%d]", inst.operation, width, inst.displacement);
        } else if (inst.address_offset != 0) {
          printf("%s %s [%s + %hd]", inst.operation, width, inst.effective_address, inst.address_offset);
        } else if (inst.source) {
          printf("%s %s", inst.operation, inst.source);
        } else {
          printf("%s %s [%s]", inst.operation, width, inst.effective_address);
        }
        print_v_bit_clause(inst);
        break;
      }
    case ConditionalJump: {
      i16 rel = (i8)inst.address_offset;
      u16 target = (u16)(inst.next_ip + rel);
      i16 delta  = (i16)(target - inst.instruction_address);

      if (delta > 0) {
        printf("%s $+%d+0\n", inst.operation, delta);
      } else if (delta == 0) {
        printf("%s $+0\n", inst.operation);
      } else {
        printf("%s $%d+0\n", inst.operation, delta);
      }
      break;
    }
    case Register_Memory:
      {
        if (inst.mod == 0b11) {
          printf("%s %s, %s\n", inst.operation, inst.source, inst.dest);
        } else if (inst.rm == 0b110 && inst.mod == 0b00) {
          printf("%s %s, [%u]\n", inst.operation, inst.source, (u16)inst.displacement);
        } else if (inst.d_bit == 0b0) {
          printf("%s ", inst.operation);
          print_address(inst);
          printf(", %s\n", inst.source);
        } else {
          printf("%s %s, ", inst.operation, inst.source);
          print_address(inst);
          printf("\n");
        }
        break;
      }
    case Register_Immediate:
      {
        if (inst.rm == 0b110 && inst.mod == 0b00) {
          const char * width = inst.w_bit ? "word" : "byte";
          printf("%s [%d], %s %d\n", inst.operation, inst.displacement, width, inst.immediate);
        }
        else if (inst.source) {
          if (inst.d_bit) {
            printf("%s %d, %s\n", inst.operation, inst.immediate, inst.source);
          } else {
            printf("%s %s, %d\n", inst.operation, inst.source, inst.immediate);
          }
        }
        else if (inst.effective_address) {
          char source[20] = {};
          if (inst.displacement != 0) {
            sprintf(source, "[%s + %hd]", inst.effective_address, inst.displacement);
          } else {
            sprintf(source, "[%s]", inst.effective_address);
          }
          const char * width = inst.w_bit ? "word" : "byte";
          printf("%s %s, %s %d\n", inst.operation, source, width, inst.immediate);
        } else if (inst.is_accumulator) {
          const char * accumulator = inst.w_bit ? "ax" : "al";
          printf("%s %s, %d\n", inst.operation, accumulator, inst.immediate);
        }
        break;
      }
    case Memory_Immediate:
      {
        if (inst.is_accumulator) {
          if (inst.d_bit) {
            printf("%s ax, [%d]\n", inst.operation, inst.immediate);
          } else {
            printf("%s [%d], ax\n", inst.operation, inst.immediate);
          }
        }
        break;
      }
    default:
      break;
  }
}
