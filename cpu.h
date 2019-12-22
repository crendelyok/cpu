#ifndef _CPU_H
#define _CPU_H

#include "transl.h"
#include "stack.h"
#include "byte_code.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define RAM_SIZE 16
#define REG_NUMBER 16
enum CPU_REG_ENUM
{
	CPU_REG_NUM_DEFAULT = -1,
	#define DEF_REG(name, byte) CPU_REG_##name,
	#include "def_reg.h"
	#undef DEF_REG	
};
typedef struct Stack_t stack_t;
typedef struct cpu cpu_t;
typedef uint64_t val_t;

struct cpu 
{
	stack_t* stack;
	val_t* reg;	
	char* rip;
	val_t* RAM;
};



int Disassemble(char**);
int Execution(const char*);
void CpuDump(cpu_t* cpu);

#endif //_CPU_H
