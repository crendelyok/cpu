#ifndef _TRANSL_H
#define _TRANSL_H
#include "cvector.h"
#include <stdint.h>
#include <stdio.h>

typedef uint64_t val_t;
typedef struct label label_t;
typedef struct token token_t;
typedef vector_define(token_t) tvec_t;
typedef vector_define(label_t) lvec_t;

#define MAX_LITERALS 2048
#define MAX_LABELS 32
#define MAX_LABEL_NAME 16

#define UNKNOWN_LABEL -1

struct label
{
	char name[MAX_LABEL_NAME];
	int offset; //offset from the beg of the binary vector 
};

struct token
{
        enum token_type {
		#define TOKEN_TYPE(name, kword) name,
		#include "token_type.h"
		#undef TOKEN_TYPE
	}token_type;
	enum is_kword {
		UNDEF,
		KWORD
	}is_kword;
	
        union {
                val_t token_val; 
                char token_label[MAX_LABEL_NAME]; 
                enum token_kword {
                        #define DEF_KWORD(name) TOKEN_KWORD_##name, 
                        #include "def_kword.h"
                        #undef DEF_KWORD
                }token_kword;
		enum token_register {
                        #define DEF_REG(name, byte) TOKEN_REG_##name, 
                        #include "def_reg.h"
                        #undef DEF_REG
                }token_register;
                enum token_synt {
                        #define SYNTAX(name, symbol) TOKEN_SYNT_##name,  
                        #include "def_syntax.h"   
                        #undef SYNTAX
                }token_synt;
        };
	char* byte_pointer;
};

#define TOKEN_END -1

/*
enum write_byte {
	#define WRITE_BYTE(name, byte) BYTE_##name,
	#include "write_byte.h"
	#undef WRITE_BYTE
}
*/
char* GetBinaryFromFile(const char*);
void WriteNum(char**, val_t);
int FirstPass(char*, char**, tvec_t*, lvec_t*);
int SecondPass(char**, tvec_t*, lvec_t*);
long FileSize(FILE*);
void TokenDump(token_t);
int GetToken(char**, tvec_t*, lvec_t*);
#endif
