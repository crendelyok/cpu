#include "stack.h"

long StackHash(struct Stack_t* st) {
	if (!st) {
		return -1;
	}
	unsigned long hash = 0;
	char* buf = (char *)st;
	for (unsigned long i = 0; i < sizeof(st); ++i) {
		if ((i <= sizeof(st -> name) + sizeof(st -> canary1)
		   + sizeof(st -> size) + sizeof (elem_t) * st -> maxsize
		   + sizeof(st -> maxsize)) ||
		   (i >= sizeof(st -> name) + sizeof(st -> canary1)
		   + sizeof(st -> size) + sizeof (elem_t) * st -> maxsize
		   + sizeof(st -> maxsize) + sizeof(hash))) 
			hash += buf[i]; 
	}
	return hash;
}

int StackInit(struct Stack_t* st) {
	if (!st) {
		st -> errcode = STPOINTERERR;
		return 0;
	}
	SEC st -> canary1 = CANARY;
	st -> size = 0;
	st -> data = calloc(sizeof(elem_t), STACK_SIZE);
	if (st -> data == NULL) {
		DBG StackDump(st);
		st -> errcode = 1;
		return 0;
	}
	st -> maxsize = STACK_SIZE;
	st -> errcode = 0;
	SEC st -> canary2 = CANARY;
	SEC st -> hash = StackHash(st);
	SEC assert(!StackOk(st));
	return 1;
}

int StackDestroy(struct Stack_t* st) {
	SEC assert(!StackOk(st));
	st -> maxsize = 0;
	free(st -> data);
	st -> size = 0;
	st -> errcode = 0;
	SEC st -> canary1 = CANARY;
	SEC st -> canary2 = CANARY;
	SEC st -> hash = StackHash(st);
	SEC assert(!StackOk(st));
	return 1;
}

int StackPush(struct Stack_t* st, elem_t elem) {
	SEC assert(!StackOk(st));
	long is_newhash = 0;
	if (st -> size >= st -> maxsize) {
		is_newhash = 1;
		elem_t* tmp = realloc(st -> data, (st -> maxsize) * 2 * sizeof(elem_t));
		if(!tmp) {
			st -> errcode = STREALLOCERR;
			return 0;
		}
		st -> data = tmp;
		st -> maxsize *= 2;
	}
	/*
	if ((st -> size * 4 < st -> maxsize) && (st -> maxsize > STACK_SIZE)) {
		is_newhash = 1;
		int newsize = st -> maxsize / 2;	
		elem_t* tmp = realloc(st -> data, newsize);
		if (tmp) {
			st -> data = tmp;
			st -> maxsize /= 2;
		}
		else {
			st -> errcode = STREALLOCERR;
		}
	}
	*/
	(st -> data)[st -> size ++] = elem;
	SEC {
		if (is_newhash)
			st -> hash = StackHash(st);
	}
	SEC assert(!StackOk(st));
	return 1;
}

elem_t StackPop(struct Stack_t* st) {
	SEC assert(!StackOk(st));
	if (st -> size <= 0) {
		st -> errcode = STUNDERFLOW;
		return -1;//-1  if error
	}
	elem_t ret = (st -> data)[--(st -> size)];
	SEC st -> hash = StackHash(st);
	SEC assert(!StackOk(st));
	return ret;
}

int StackOk(struct Stack_t* st) {
	if (st == NULL) {
		st -> errcode = STPOINTERERR;
		goto retanddump;
	}
	if (st -> data == NULL) {
		st -> errcode = STDATAERR;
		goto retanddump;
	}
	if (st -> canary1 != CANARY || st -> canary2 != CANARY) {
		st -> errcode = STCANARYERR;
		goto retanddump;
	}
	if (st -> size < 0) {
		st -> errcode = STUNDERFLOW;
		goto retanddump;
	}
	if (st -> size > st -> maxsize) {
		st -> errcode = STOVERFLOW;
		goto retanddump;
	}
	elem_t hash = StackHash(st);
	if (hash != st -> hash) {
		st -> errcode = STHASHERR;
		goto retanddump;
	}
	return 0;
retanddump:
	DBG StackDump(st);
	SEC return 1;
	DBG return 0;
	return 1;
}

void StackDump(struct Stack_t* st) {//*file
	printf("\nStack_t \"%s\" [%p]", st -> name, st);//fprinrf
	if (st -> errcode)
		printf(" (ERROR)\n");
	else
		printf(" (ok)\n");
	printf("{\n");
	SEC printf("\tcanary1 = %ld\n", st -> canary1);
	printf("\tsize = %d\n", st -> size);
	printf("\tmax size = %d\n", st -> maxsize);
	printf("\tdata[%d] = %p\n\t{\n", st -> maxsize, st -> data);
	if (!(st -> data))
		printf("\t\tDATA POINTER IS NULL\n");
	else {
		for (int i = 0; i < (st -> maxsize); ++i) {
			if (i < (st -> size)) {
				printf("\t\t[%3d] = %11d\n", i, (int)(st -> data)[i]);
			}
			else{
				printf("\t\t*[%2d] = %11d", i, (int)(st -> data)[i]);
				printf(" NOT IN STACK\n");
			}
		}
	}
	printf("\t}\n");
	SEC printf("\thash = %d\n", (int)(st -> hash));
	SEC printf("\terrcode = %d ", st -> errcode);
	SEC {
		if (st -> errcode == STREALLOCERR) 
			printf("(REALLOC ERROR)\n");
		else if (st -> errcode == STUNDERFLOW) 
			printf("(STACK UNDERFLOW)\n");
		else if (st -> errcode == STCANARYERR) 
			printf("(CANARY ERROR)\n");
		else if (st -> errcode == STPOINTERERR) 
			printf("(STACKPOINTER NULL)\n");
		else if (st -> errcode == STOVERFLOW) 
			printf("(STACKOVERFLOW)\n");
		else if (st -> errcode == STHASHERR) 
			printf("(HASH ERROR)\n");
		else if (st -> errcode == STDATAERR) 
			printf("(DATA POINTER IS NULL)\n");
		else
			printf("(no error)\n");
	}
	SEC printf("\tcanary2 = %ld\n", st -> canary2);
	printf("}\n");
}

