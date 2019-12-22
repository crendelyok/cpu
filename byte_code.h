#define BYTE_END '#'
#define NULL_ADD '@'

#define BYTE_PUSH_Q 'A'
#define BYTE_PUSH_R 'B'
#define BYTE_PUSH_MR 'C'
#define BYTE_PUSH_MRQ 'D'
#define BYTE_POP 'E'
#define BYTE_IN 'F'
#define BYTE_OUT 'G'
#define BYTE_CALL 'H'
#define BYTE_POP_MR 'I'

#define BYTE_ADD '+'
#define BYTE_SUB '-'
#define BYTE_MUL '*'
#define BYTE_DIV '/'

#define BYTE_RET 'J'

#define BYTE_JMP 'j'
#define BYTE_JA 'a'
#define BYTE_JB 'b'
#define BYTE_JE 'e'

enum def_reg_bytes
{
	NOT_A_REGISTER = 'j',
	#define DEF_REG(name, sym) BYTE_REG_##name,
	#include "def_reg.h"	
	#undef DEF_REG
};

