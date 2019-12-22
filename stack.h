//Stack
//DEBUG_COMPILATION and SECURE_STACK 
//must be defined before including "stack.h"
//DEBUF_COMPILATION shows debug
//SECURE_STACK uses canarys and hash to prevent errors

#ifndef STACK_H
#define STACK_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <limits.h>

#ifdef DEBUG_COMPILATION
#define DBG
#define SECURE_STACK
#else
#define DBG if(0) 
#endif //DEBUG_COMPILATION

#ifdef SECURE_STACK
#define SEC
#else
#define SEC if(0)
#endif //SECURE_STACK

#define STACK_SIZE 16 //UNIT64_MAX 
#define STACK_NAME 16
typedef uint64_t elem_t;
#define CANARY 0x36D736D7

struct Stack_t {
	char name[STACK_NAME];
	long canary1;
	int size;
	elem_t* data;
	int maxsize;
	elem_t hash;
	int errcode;
	long canary2;
};

//errcodes:
#define STREALLOCERR 1 //reallocation error
#define STUNDERFLOW 2  //stack underflow
#define STCANARYERR 3  //canary errror
#define STPOINTERERR 4 //stack pointer is null
#define STOVERFLOW 5   //stack overflow
#define STHASHERR 6    //hash is different
#define STDATAERR 7    //data poiner is null

int StackInit(struct Stack_t* st);
int StackPush(struct Stack_t* st, elem_t elem);
elem_t StackPop(struct Stack_t* st);
int StackDestroy(struct Stack_t* st);
int StackOk(struct Stack_t* st);
void StackDump(struct Stack_t* st); //works well only if elem_t = int
long StackHash(struct Stack_t* st);

#endif //STACK_H
