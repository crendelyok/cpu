#include "transl.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

long FileSize(FILE* in) 
{
	long file_size = fseek(in, 0L, SEEK_END);
	file_size = ftell(in);
	rewind(in);
	return file_size;
}

void TokenDump(token_t token)
{
	enum token_type type = token.token_type;

	#define TOKEN_TYPE(name, kword) \
	if (type == name) { \
		printf("TYPE = %s", #name); \
	}
	#include "token_type.h"
	#undef TOKEN_TYPE
	if (token.token_kword == TOKEN_KWORD_pop)
		printf(" POP");	
	if (type == LABEL || type == LABEL_DEF || type == LABEL_VAL) {
		printf(" LABEL = %s\n", token.token_label);
	}
	printf("\n");
}

char* GetBinaryFromFile(const char* file_name) 
{
	FILE* in = fopen(file_name, "r");
	assert(in != NULL);

	char* str = (char*)calloc(FileSize(in) + 1, 1);
	assert(str != NULL);
	fread(str, 1, FileSize(in), in); 

	vector_spec(token_t) token_vec;
	vector_init(&token_vec);
	vector_reserve(&token_vec, MAX_LITERALS * sizeof(token_t));
	
	vector_spec(label_t) label_vec;
	vector_init(&label_vec);
	vector_reserve(&label_vec, MAX_LABELS * sizeof(label_t));
	

	char* cur_char = str;

	GetToken(&cur_char, &token_vec, &label_vec); //tokenizer
	token_t end_elem;
	end_elem.token_type = UNDEF_TOKEN_TYPE;
	vector_push_back(&token_vec, end_elem);



	char* bytecode = (char*)calloc(FileSize(in) * 8, 1);
	char* byte_ptr = bytecode;

	/*
	foreach_in_vec(&token_vec, token_t){
		TokenDump(*__ptr);
	}
	*/

	FirstPass(bytecode, &byte_ptr, &token_vec, &label_vec);
	
	/*
	foreach_in_vec(&label_vec, label_t){
		printf("name = %s\noffset = %d\n\n", __ptr -> name, __ptr -> offset);
	}
	*/
	long byte_size = byte_ptr - bytecode + 1;
	byte_ptr = bytecode;

	SecondPass(&byte_ptr, &token_vec, &label_vec);

	/*
	int i = 0;
	
	foreach_in_vec(&token_vec, token_t) {
		TokenDump(vector_elem(&token_vec, i));
		++i;
	}
	*/

	FILE* out = fopen("binary", "w");
	fwrite(bytecode, 1, byte_size, out);
	fclose(out);

	
	free(bytecode);
	vector_destroy(&token_vec);
	vector_destroy(&label_vec);
	fclose(in);
	free(str);
	return NULL;
}	

/*
#define WRITE_BYTE(name, sym) DEF_BYTE_##name

enum write_bytes {
	#include "write_byte.h"
}
*/
#include "byte_code.h"


int SecondPass(char** byte_ptr, tvec_t* token_vec, lvec_t* label_vec)
{
	for(token_t* ptr = vector_beg(token_vec); ptr < vector_end(token_vec); ptr++) {
		if (ptr -> token_type == UNDEF_TOKEN_TYPE) {
			(*byte_ptr)++;
			break;
		}
		
		if (ptr -> token_type == LABEL_VAL) {
			foreach_in_vec(label_vec, label_t) {
				if (strcmp(ptr -> token_label, __ptr -> name) == 0) {
					*((val_t*)(ptr -> byte_pointer)) = __ptr -> offset;
				}
                        }
		}
		
	}	
	return 0;
}

int FirstPass(char* bytecode, char** byte_ptr, tvec_t* token_vec, lvec_t* label_vec)
{
	
	
	for (token_t* ptr = vector_beg(token_vec); ptr < vector_end(token_vec); ptr++) {
		//TokenDump (*ptr);
		if (ptr -> token_type == UNDEF_TOKEN_TYPE) {
			**byte_ptr = BYTE_END;
			break;
		}
		
		if (ptr -> token_type == CMD) {
			ptr ++;
			if (ptr -> token_type == NUM) {
				ptr--;
				ptr -> token_type = CMD_Q;
				if (ptr -> token_kword == TOKEN_KWORD_push) {
					ptr++;
					**byte_ptr = BYTE_PUSH_Q;
					(*byte_ptr)++;
				}

				**((val_t**)byte_ptr) = ptr -> token_val;
				*((char**)(byte_ptr)) += 8;
			}
			if (ptr -> token_type == REG) {
				ptr--;
				ptr -> token_type = CMD_R;
				
				if (ptr -> token_kword == TOKEN_KWORD_push)
					**byte_ptr = BYTE_PUSH_R;
				if (ptr -> token_kword == TOKEN_KWORD_pop)
					**byte_ptr = BYTE_POP;
				ptr++;
				(*byte_ptr)++;

				#define DEF_REG(name, byte) \
					if (TOKEN_REG_##name == (ptr -> token_register)) \
						**byte_ptr = byte;
				#include "def_reg.h"
				#undef DEF_REG
				(*byte_ptr)++;
			}
			if (ptr -> token_type == SYNT && ptr -> token_synt == TOKEN_SYNT_LBR) {
				ptr++;
				if (ptr -> token_type == REG) {
					ptr++;
				 	if (ptr -> token_type == SYNT && 
					    ptr -> token_synt == TOKEN_SYNT_RBR) {
						ptr -= 3;
						ptr -> token_type = CMD_MR;

						if (ptr -> token_kword == TOKEN_KWORD_push) {
							ptr += 3;
							**byte_ptr = BYTE_PUSH_MR;
							(*byte_ptr)++;
						}
						else if (ptr -> token_kword == TOKEN_KWORD_pop) {
							ptr += 3;
							**byte_ptr = BYTE_POP_MR;
							(*byte_ptr)++;
						}
						ptr--;
						#define DEF_REG(name, byte) \
							if (TOKEN_REG_##name == (ptr -> token_register)) \
								**byte_ptr = byte;
						#include "def_reg.h"
						#undef DEF_REG
						ptr++;

						(*byte_ptr)++;
					}
					else if (ptr -> token_type == SYNT &&
						 ptr -> token_synt == TOKEN_SYNT_PLUS) {
						ptr -= 3;
						ptr -> token_type = CMD_MRQ;
						//ptr -> token_arg = ARG_MRQ;
						//ptr -> arg_r = (ptr + 2) -> token_register;
						//ptr -> arg_q = (ptr + 4) -> token_val;
						ptr += 3;

						**byte_ptr = BYTE_PUSH_MRQ;
						(*byte_ptr)++;
						
						ptr--;
						#define DEF_REG(name, byte) \
							if (TOKEN_REG_##name == (ptr -> token_register)) \
								**byte_ptr = byte;
						#include "def_reg.h"
						#undef DEF_REG
						ptr++;
						(*byte_ptr)++;
						ptr++;
						**((val_t**)byte_ptr) = ptr -> token_val;
						*((char**)(byte_ptr)) += 8;
					}
					ptr--;
				}
				ptr--;
			}
			ptr--;
			
			if (ptr -> token_type == CMD) {
				if (ptr -> token_kword == TOKEN_KWORD_vmcall) {
					**byte_ptr = BYTE_VMCALL;
					(*byte_ptr)++;
				}			
				if (ptr -> token_kword == TOKEN_KWORD_in) {
					**byte_ptr = BYTE_IN;
					(*byte_ptr)++;
				}			
				if (ptr -> token_kword == TOKEN_KWORD_out) {
					**byte_ptr = BYTE_OUT;
					(*byte_ptr)++;
				}			
				if (ptr -> token_kword == TOKEN_KWORD_ret) {
					**byte_ptr = BYTE_RET;
					(*byte_ptr)++;
				}			
				if (ptr -> token_kword == TOKEN_KWORD_end) {
					**byte_ptr = BYTE_END;
					(*byte_ptr)++;
				}			
				if (ptr -> token_kword == TOKEN_KWORD_add) {
					**byte_ptr = BYTE_ADD;
					(*byte_ptr)++;
				}			
				if (ptr -> token_kword == TOKEN_KWORD_sub) {
					**byte_ptr = BYTE_SUB;
					(*byte_ptr)++;
				}			
				if (ptr -> token_kword == TOKEN_KWORD_mul) {
					**byte_ptr = BYTE_MUL;
					(*byte_ptr)++;
				}			
				if (ptr -> token_kword == TOKEN_KWORD_div) {
					**byte_ptr = BYTE_DIV;
					(*byte_ptr)++;
				}			
			}	
			enum token_kword kword = ptr -> token_kword;
			if (kword == TOKEN_KWORD_jmp || kword == TOKEN_KWORD_ja || 
			    kword == TOKEN_KWORD_jb || kword == TOKEN_KWORD_je ||
			    kword == TOKEN_KWORD_call) {
				ptr++;
				if (ptr -> token_type == LABEL) {
					ptr -> token_type = LABEL_VAL;

					ptr--;
					ptr -> token_type = CMD_L;
					ptr++;
					

					int is_new_label = 1;
					int pos_in_label = 0;
					foreach_in_vec(label_vec, label_t) {
						if (strcmp(ptr -> token_label, __ptr -> name) == 0) {
							is_new_label = 0;
						}
						pos_in_label++;
					}
					
					if (is_new_label) {
						label_t elem;
						strcpy(elem.name, ptr -> token_label);
						elem.offset = UNKNOWN_LABEL;
						vector_push_back(label_vec, elem);
					}
					
					ptr -> byte_pointer = (*byte_ptr + 1);
					*((val_t*)(*byte_ptr + 1)) = pos_in_label; 

					if (kword == TOKEN_KWORD_ja) {
						**byte_ptr = BYTE_JA;
						//*((val_t*)(*byte_ptr + 1)) = NULL_ADD;
						(*byte_ptr) += 9;
					}
					else if (kword == TOKEN_KWORD_jb) {
						**byte_ptr = BYTE_JB;
						//*(*byte_ptr + 1) = NULL_ADD;
						(*byte_ptr) += 9;
					}
					else if (kword == TOKEN_KWORD_jmp) {
						**byte_ptr = BYTE_JMP;
						//*(*byte_ptr + 1) = NULL_ADD;
						(*byte_ptr) += 9;
					}
					else if (kword == TOKEN_KWORD_je) {
						**byte_ptr = BYTE_JE;
						//*(*byte_ptr + 1) = NULL_ADD;
						(*byte_ptr) += 9;
					}
					else if (kword == TOKEN_KWORD_call) {
						**byte_ptr = BYTE_CALL;
						//*(*byte_ptr + 1) = NULL_ADD;
						(*byte_ptr) += 9;
					}
				}	
			}
		}
		
		if (ptr -> token_type == LABEL) {	

			ptr -> token_type = LABEL_DEF;

			label_t new_label;
			new_label.offset = (*byte_ptr) - bytecode;
			
			int is_new_label = 1;
			int pos_in_label = 0;
			foreach_in_vec(label_vec, label_t) {
				if(strcmp(ptr -> token_label, __ptr -> name) == 0) {
					assert(__ptr -> offset == UNKNOWN_LABEL);
					is_new_label = 0;
					break;
				}	
				pos_in_label++;
			}
			if (is_new_label) {
				strcpy(new_label.name, ptr -> token_label); 
				vector_push_back(label_vec, new_label); 
			}
			else {
				(vector_elem(label_vec, pos_in_label)).offset = new_label.offset;
			}
		}
		
	}
	return 0;
}


int GetToken(char** cur_char, tvec_t* token_vec, lvec_t* label_vec)
{
	while(**cur_char == ' ' || **cur_char == '\n' || **cur_char == '\t')
		(*cur_char)++;
	token_t elem;
	
	if (**cur_char == '\0')
		return TOKEN_END;

	int is_num = 0;
	val_t val = 0;
	while (**cur_char >= '0' && **cur_char <= '9') {
		is_num = 1;
		val *= 10;
		val += (**cur_char) - '0';
		(*cur_char)++;
	}
	
	if (is_num) {
		elem.token_type = NUM;
		elem.token_val = val;
		vector_push_back(token_vec, elem);	
		GetToken(cur_char, token_vec, label_vec);
		return NUM;
	}
		
	
	#define DEF_KWORD(name)                                      \
		if (strncmp(*cur_char, #name, strlen(#name)) == 0) { \
			*cur_char += strlen(#name);                  \
			elem.token_type = CMD;		             \
			elem.token_kword = TOKEN_KWORD_##name;       \
			vector_push_back(token_vec, elem);           \
			GetToken(cur_char, token_vec, label_vec );   \
			return CMD;			             \
		}
	#include "def_kword.h"
	#undef DEF_KWORD
	
	#define DEF_REG(name, byte)                                  \
		if (strncmp(*cur_char, #name, strlen(#name)) == 0) { \
			*cur_char += strlen(#name);                  \
			elem.token_type = REG;		             \
			elem.token_register = TOKEN_REG_##name;      \
			vector_push_back(token_vec, elem);           \
			GetToken(cur_char, token_vec, label_vec);    \
			return REG;			             \
		}
	#include "def_reg.h"
	#undef DEF_REG
	
	#define SYNTAX(name, sym)                                    \
		if (strncmp(*cur_char, sym, strlen(sym)) == 0) {     \
			*cur_char += strlen(sym);                    \
			elem.token_type = SYNT;		             \
			elem.token_synt = TOKEN_SYNT_##name;         \
			vector_push_back(token_vec, elem);           \
			GetToken(cur_char, token_vec, label_vec);    \
			return SYNT;			             \
		}
	#include "def_syntax.h"
	#undef SYNTAX
	
	int is_label = 0;
	char label_name[MAX_LABEL_NAME] = "";
	while(**cur_char >= 'A' && **cur_char <= 'z') {
		is_label = 1;
		char src[2];
		src[0] = **cur_char;
		src[1] = '\0';
		strcat(label_name, src);
		(*cur_char)++;
	}
	
	if (is_label) {
		elem.token_type = LABEL;
		strcpy(elem.token_label, label_name);
		vector_push_back(token_vec, elem);
		GetToken(cur_char, token_vec, label_vec);
		return LABEL;
	}

	return UNDEF_TOKEN_TYPE;
}
