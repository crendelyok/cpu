WRITE_BYTE(BYTE_END,
	break;
)
WRITE_BYTE(NULL_ADD,
	printf("NULL PTR DETECTED\n");
	break;
)

WRITE_BYTE(BYTE_PUSH_Q,
	cpu_stack[cpu -> reg[CPU_REG_rsp]++] = *(val_t*)(cpu -> rip);
	cpu -> rip += 8;
)

WRITE_BYTE(BYTE_PUSH_R,
	cpu_stack[cpu -> reg[CPU_REG_rsp]] = (cpu -> reg)[*(cpu -> rip) - 'k'];
	cpu -> reg[CPU_REG_rsp] ++;
	cpu -> rip ++;
)
WRITE_BYTE(BYTE_POP,
	cpu -> reg[*(cpu -> rip) - 'k'] = cpu_stack[--cpu -> reg[CPU_REG_rsp]];
	cpu -> rip ++;
)
WRITE_BYTE(BYTE_POP_MR,
	val_t size = cpu -> reg[CPU_REG_rsp];

	if (size > cpu -> reg[*(cpu -> rip) - 'k'])
		cpu_stack[cpu -> reg[*(cpu -> rip) - 'k']] = cpu_stack[--cpu -> reg[CPU_REG_rsp]];

	else if (size == cpu -> reg[*(cpu -> rip) - 'k'])
		cpu_stack[cpu -> reg[*(cpu -> rip) - 'k']] = cpu_stack[cpu -> reg[CPU_REG_rsp]];

	else if (size < cpu -> reg[*(cpu -> rip) - 'k']) {
		cpu_stack[cpu -> reg[*(cpu -> rip) - 'k']] = cpu_stack[cpu -> reg[CPU_REG_rsp]];
		cpu -> reg[CPU_REG_rsp] = cpu -> reg[*(cpu -> rip) - 'k'];
	}

	cpu -> rip ++;
)
WRITE_BYTE(BYTE_PUSH_MR,
	cpu_stack[cpu -> reg[CPU_REG_rsp]++] = cpu_stack[cpu -> reg[*(cpu -> rip) - 'k']];
	cpu -> rip ++;
)
WRITE_BYTE(BYTE_PUSH_MRQ,
	int address = cpu_stack[cpu -> reg[*(cpu -> rip) - 'k']];
	cpu -> rip ++;
	cpu_stack[cpu -> reg[CPU_REG_rsp]++] = cpu_stack[*(val_t*)(cpu -> rip) + address];
	cpu -> rip += 8;

)
WRITE_BYTE(BYTE_IN,
	val_t val;
	scanf("%lu", &val);
	(cpu -> reg)[CPU_REG_rax] = val;
)
WRITE_BYTE(BYTE_OUT,
	val_t val = (cpu -> reg)[CPU_REG_rax];
	printf("%lu\n", val);
)

WRITE_BYTE(BYTE_JMP,
	cpu -> rip = bytecode + *((val_t*)(cpu -> rip));
)
WRITE_BYTE(BYTE_JA,
	if (cpu_stack[cpu -> reg[CPU_REG_rsp] - 2] > cpu_stack[cpu -> reg[CPU_REG_rsp] - 1])
		cpu -> rip = bytecode + *((val_t*)(cpu -> rip));
	else 
		cpu -> rip += 8;
)
WRITE_BYTE(BYTE_JB,
	if (cpu_stack[cpu -> reg[CPU_REG_rsp] - 2] < cpu_stack[cpu -> reg[CPU_REG_rsp] - 1])
		cpu -> rip = bytecode + *((val_t*)(cpu -> rip));
	else 
		cpu -> rip += 8;
)
WRITE_BYTE(BYTE_JE,
	if (cpu_stack[cpu -> reg[CPU_REG_rsp] - 2] == cpu_stack[cpu -> reg[CPU_REG_rsp] - 1])
		cpu -> rip = bytecode + *((val_t*)(cpu -> rip));
	else 
		cpu -> rip += 8;
)
WRITE_BYTE(BYTE_SUB,
	cpu -> reg[CPU_REG_rax] = cpu_stack[cpu -> reg[CPU_REG_rsp] - 2] -
				  cpu_stack[cpu -> reg[CPU_REG_rsp] - 1];
)
WRITE_BYTE(BYTE_MUL,
	cpu -> reg[CPU_REG_rax] = cpu_stack[cpu -> reg[CPU_REG_rsp] - 2] *
				  cpu_stack[cpu -> reg[CPU_REG_rsp] - 1];
)
WRITE_BYTE(BYTE_ADD,
	cpu -> reg[CPU_REG_rax] = cpu_stack[cpu -> reg[CPU_REG_rsp] - 2] +
				  cpu_stack[cpu -> reg[CPU_REG_rsp] - 1];
)
WRITE_BYTE(BYTE_DIV,
	cpu -> reg[CPU_REG_rax] = cpu_stack[cpu -> reg[CPU_REG_rsp] - 2] /
				  cpu_stack[cpu -> reg[CPU_REG_rsp] - 1];
)
WRITE_BYTE(BYTE_CALL,
	int offset = *((val_t*)(cpu -> rip));
	(cpu -> rip) += 8;
	cpu_stack[cpu -> reg[CPU_REG_rsp]++] = (cpu -> rip) - bytecode;
	cpu -> rip = bytecode + offset;	
)
WRITE_BYTE(BYTE_RET,
	(cpu -> rip) = bytecode + cpu_stack[--cpu -> reg[CPU_REG_rsp]];
)
WRITE_BYTE(BYTE_VMCALL,
	val_t elem = 0;
	val_t address = cpu -> reg[CPU_REG_rsi];
	switch (cpu -> reg[CPU_REG_rax]) {
	case 0:
		cpu -> reg[CPU_REG_rax] = (val_t)scanf("%lu", &elem);
		cpu_stack[address] = elem;
		break;
	case 1:
		cpu -> reg[CPU_REG_rax] = (val_t)printf("%lu", cpu_stack[cpu -> reg[CPU_REG_rsi]]);
		break;

	case 2:
		exit(cpu -> reg[CPU_REG_rsi]);		
		break;
	default: 
		printf("No vmcall with this number\n");
		break;
	}
)
