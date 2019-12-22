WRITE_BYTE(BYTE_END,
	break;
)
WRITE_BYTE(NULL_ADD,
	printf("NULL PTR DETECTED\n");
	break;
)

WRITE_BYTE(BYTE_PUSH_Q,
	(cpu -> rip)++;
	val_t val = *((val_t*)(cpu -> rip));
	(cpu -> stack) -> size = (elem_t)(cpu -> reg)[CPU_REG_rsp] ++; 
	StackPush(cpu -> stack, val); 	
	(cpu -> rip) += 8;
)

WRITE_BYTE(BYTE_PUSH_R,
	(cpu -> rip)++;
	(cpu -> stack) -> size = (cpu -> reg)[CPU_REG_rsp] ++; 
	StackPush(cpu -> stack, (elem_t)(cpu -> reg)[(*(cpu -> rip)) - 'k']);
	(cpu -> rip)++;
)
WRITE_BYTE(BYTE_POP,
	(cpu -> rip)++;
	(cpu -> stack) -> size = (cpu -> reg)[CPU_REG_rsp]--; 
	(cpu -> reg)[(*(cpu -> rip)) - 'k'] = StackPop(cpu -> stack);
	if ((*(cpu -> rip)) - 'k' == CPU_REG_rsp) {
		if (cpu -> stack -> size < (cpu -> reg)[CPU_REG_rsp]) {
			while((val_t)(cpu -> stack -> size) < (cpu -> reg)[CPU_REG_rsp])
				StackPush(cpu -> stack, 0);
		}
		else 
			cpu -> stack -> size = (cpu -> reg)[CPU_REG_rsp];
	}	
	(cpu -> rip)++;
)
WRITE_BYTE(BYTE_POP_MR,
	(cpu -> rip)++;
	int address = (cpu -> reg)[(*(cpu -> rip)) - 'k'];
	(cpu -> stack) -> size = (cpu -> reg)[CPU_REG_rsp]--; 
	(cpu -> stack -> data)[address] = StackPop(cpu -> stack);
	//(cpu -> RAM)[address] = StackPop(cpu -> stack); 
	(cpu -> rip)++;
)
WRITE_BYTE(BYTE_PUSH_MR,
	(cpu -> rip)++;
	int address = (cpu -> reg)[(*(cpu -> rip)) - 'k'];
	(cpu -> stack) -> size = (cpu -> reg)[CPU_REG_rsp] ++; 
	
	StackPush(cpu -> stack, (cpu -> stack -> data)[address]);
	//StackPush(cpu -> stack, (cpu -> RAM)[address]);
	(cpu -> rip)++;
)
WRITE_BYTE(BYTE_PUSH_MRQ,
	(cpu -> rip)++;
	int address = (cpu -> reg)[(*(cpu -> rip)) - 'k'];
	(cpu -> rip)++;
	int offset = *((elem_t*)(cpu -> rip));
	(cpu -> stack) -> size = (cpu -> reg)[CPU_REG_rsp] ++; 
	StackPush(cpu -> stack, (cpu -> stack -> data)[address + offset]);
	//StackPush(cpu -> stack, (cpu -> RAM)[address + offset]);
	//(cpu -> rip)++;
	(cpu -> rip) += 8;

)
WRITE_BYTE(BYTE_IN,
	(cpu -> rip)++;
	val_t val;
	scanf("%lu", &val);
	(cpu -> reg)[CPU_REG_rax] = val;
)
WRITE_BYTE(BYTE_OUT,
	(cpu -> rip)++;
	val_t val = (cpu -> reg)[CPU_REG_rax];
	printf("%lu\n", val);
)

WRITE_BYTE(BYTE_JMP,
	(cpu -> rip)++;
	int offset = *((elem_t*)(cpu -> rip));
	cpu -> rip = bytecode + offset;
)
WRITE_BYTE(BYTE_JA,
	(cpu -> rip)++;
	val_t rval = StackPop(cpu -> stack);
	val_t lval = StackPop(cpu -> stack);
	StackPush(cpu -> stack, lval);
	StackPush(cpu -> stack, rval);
	if (lval > rval) {
		int offset = *((elem_t*)(cpu -> rip));
		cpu -> rip = bytecode + offset;
	}
	else {
		(cpu -> rip) += 8;
	}
)
WRITE_BYTE(BYTE_JB,
	(cpu -> rip)++;
	val_t rval = StackPop(cpu -> stack);
	val_t lval = StackPop(cpu -> stack);
	StackPush(cpu -> stack, lval);
	StackPush(cpu -> stack, rval);
	if (lval < rval) {
		int offset = *((elem_t*)(cpu -> rip));
		cpu -> rip = bytecode + offset;
	}
	else {
		(cpu -> rip) += 8;
	}
)
WRITE_BYTE(BYTE_JE,
	(cpu -> rip)++;
	val_t rval = StackPop(cpu -> stack);
	val_t lval = StackPop(cpu -> stack);
	StackPush(cpu -> stack, lval);
	StackPush(cpu -> stack, rval);
	if (lval == rval) {
		int offset = *((elem_t*)(cpu -> rip));
		cpu -> rip = bytecode + offset;
	}
	else {
		(cpu -> rip) += 8;
	}
)
WRITE_BYTE(BYTE_SUB,
	(cpu -> rip)++;
	val_t operated = StackPop(cpu -> stack);
	val_t val = StackPop(cpu -> stack);
	StackPush(cpu -> stack, val);
	StackPush(cpu -> stack, operated);
	val -= operated;
	(cpu -> reg)[CPU_REG_rax] = val;
)
WRITE_BYTE(BYTE_MUL,
	(cpu -> rip)++;
	val_t operated = StackPop(cpu -> stack);
	val_t val = StackPop(cpu -> stack);
	StackPush(cpu -> stack, val);
	StackPush(cpu -> stack, operated);
	val *= operated;
	(cpu -> reg)[CPU_REG_rax] = val;
)
WRITE_BYTE(BYTE_ADD,
	(cpu -> rip)++;
	val_t operated = StackPop(cpu -> stack);
	val_t val = StackPop(cpu -> stack);
	StackPush(cpu -> stack, val);
	StackPush(cpu -> stack, operated);
	val += operated;
	(cpu -> reg)[CPU_REG_rax] = val;
)
WRITE_BYTE(BYTE_DIV,
	(cpu -> rip)++;
	val_t operated = StackPop(cpu -> stack);
	val_t val = StackPop(cpu -> stack);
	StackPush(cpu -> stack, val);
	StackPush(cpu -> stack, operated);
	val /= operated;
	(cpu -> reg)[CPU_REG_rax] = val;
)
WRITE_BYTE(BYTE_CALL,
	(cpu -> rip)++;
	int offset = *((elem_t*)(cpu -> rip));
	(cpu -> rip) += 8;
	(cpu -> stack) -> size = (cpu -> reg)[CPU_REG_rsp] ++; 
	StackPush(cpu -> stack, ((cpu -> rip) - bytecode));
	cpu -> rip = bytecode + offset;	
)
WRITE_BYTE(BYTE_RET,
	(cpu -> rip) = bytecode + StackPop(cpu -> stack);
	(cpu -> stack) -> size = (cpu -> reg)[CPU_REG_rsp] --; 
	StackPop(cpu -> stack);
)
