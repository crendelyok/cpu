#include "cpu.h"

long FileSize(FILE* in) 
{
	long file_size = fseek(in, 0L, SEEK_END);
	file_size = ftell(in);
	rewind(in);
	return file_size;
}

int Disassemble(char** bytecode)
{
	char* refresh = *bytecode;

	do{
		printf("%c", **bytecode);
		(*bytecode)++;	
		if (**bytecode == BYTE_END)
			printf("%c", **bytecode);
	}while(**bytecode != BYTE_END);

	*bytecode = refresh;
	printf("\n\nDISASSEMBLE\n\n");
	do{

		if (**bytecode == BYTE_PUSH_Q) {
	
			printf("push_q ");
			(*bytecode)++;
			printf("%lu\n", *((val_t*)(*bytecode)));
		}
		if (**bytecode == BYTE_PUSH_R) {
			printf("push_r ");
			(*bytecode)++;
			printf("register\n");

		}
		if (**bytecode == BYTE_POP) {
			printf("pop ");
			(*bytecode)++;
			printf("register\n");

		}
		if (**bytecode == BYTE_IN) {
			printf("in\n");
		}
		if (**bytecode == BYTE_OUT) {
			printf("out\n");
		}
		(*bytecode)++;

	}while(**bytecode != BYTE_END);
	
	return 0;
}

int Execution(const char* filename) 
{
	FILE* in = fopen(filename, "r");
	assert(in != NULL);

	char* bytecode = (char*)calloc(FileSize(in) + 1, 1);	
	assert(bytecode != NULL);
	fread(bytecode, 1, FileSize(in), in);

	
	//Disassemble(&byte_ptr);
	
	
	#include "byte_code.h"

	cpu_t* cpu = calloc(sizeof(cpu_t), 1);
	cpu -> stack = calloc(sizeof(stack_t), 1);
	StackInit(cpu -> stack);
	cpu -> reg = calloc(sizeof(elem_t), 16);
	cpu -> RAM = calloc(sizeof(elem_t), 256);
	cpu -> rip = bytecode;
	//char* ex = bytecode;
		
	int counter = 0;
	while(1) {
		#define WRITE_BYTE(sym, act) if (*(cpu -> rip) == sym) { act } 	
		#include "write_byte.h"
		#undef WRITE_BYTE
		//CpuDump(cpu);
	
		if (counter == 20)
			//break;
		counter++;
	}
	//CpuDump(cpu);
	#undef DEF_REG

	StackDestroy(cpu -> stack);
	free(cpu -> stack);
	free(cpu -> reg);
	free(cpu -> RAM);
	free(cpu);
	free(bytecode);
	fclose(in);
	return 0;	
}

void CpuDump(cpu_t* cpu) 
{
	printf("\nCPU DUMP\n");
	for (int i = 0; i < 16; ++i) {
		printf("reg %d = %lu\n", i + 1, (cpu -> reg)[i]);
	}
	StackDump(cpu -> stack);
	printf("RIP = %c\n", *(cpu -> rip));
	//printf("\nRAM\n");
	//printf("[ ");
	//for (int i = 0; i < RAM_SIZE; ++i) {
	//	printf("%lu, ", (cpu -> RAM)[i]);
	//}
	//printf("]\n");
}
