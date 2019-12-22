#include "cpu.h"

long FileSize(FILE* in) 
{
	long file_size = fseek(in, 0L, SEEK_END);
	file_size = ftell(in);
	rewind(in);
	return file_size;
}

int Execution(const char* filename) 
{
	FILE* in = fopen(filename, "r");
	assert(in != NULL);

	char* bytecode = (char*)calloc(FileSize(in) + 1, 1);	
	assert(bytecode != NULL);
	fread(bytecode, 1, FileSize(in), in);

	cpu_t* cpu = calloc(sizeof(cpu_t), 1);
	cpu -> reg = calloc(sizeof(val_t), 16);
	cpu -> RAM = calloc(sizeof(val_t), 256);
	cpu -> rip = bytecode;	
	val_t* cpu_stack = calloc(sizeof(val_t), STACK_SIZE);	
	
	int counter = 0;
	while(1) {
		if (0) {}  
		#define WRITE_BYTE(sym, act) else if (*(cpu->rip) == sym) { (cpu->rip)++; act}
		#include "write_byte.h"
		#undef WRITE_BYTE

		//CpuDump(cpu, cpu_stack);
	
		if (counter == 100)
			//break;
		counter++;
	}

	//CpuDump(cpu);

	free(cpu -> reg);
	free(cpu_stack);
	free(cpu -> RAM);
	free(cpu);
	free(bytecode);
	fclose(in);

	return 0;	
}

void CpuDump(cpu_t* cpu, val_t* cpu_stack) 
{
	printf("\nCPU DUMP\n");
	for (int i = 0; i < 16; ++i) {
		printf("reg %d = %lu\n", i + 1, (cpu -> reg)[i]);
	}
	printf("STACK: {\n");
	for (val_t i = 0; i < cpu -> reg[CPU_REG_rsp]; ++i) {
		printf("\t[%lu] = %lu\n", i, cpu_stack[i]);
	}
	printf("STACK: }\n\n");
	printf("RIP = %c\n", *(cpu -> rip));
	printf("\n\n\n\n");
	//printf("\nRAM\n");
	//printf("[ ");
	//for (int i = 0; i < RAM_SIZE; ++i) {
	//	printf("%lu, ", (cpu -> RAM)[i]);
	//}
	//printf("]\n");
}
