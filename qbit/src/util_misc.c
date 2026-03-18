#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "util.h"

char* str_buf(const char* str, size_t len)
{
	if(!len) return NULL;
	char* retval = malloc(len);
	if(retval == NULL) return NULL;
	memcpy(retval, str, len);
	return retval;
}

int tricomp(char* a, char* b, char* c)
{
	int comp a = strcmp(a, b);
	int comp b = c == NULL ? comp a : strcmp(a, c);
	
	comp a = comp a ? ((comp a < 0) ? -1 : 1) : 0;
	comp b = comp b ? ((comp b < 0) ? -1 : 1) : 0;
	
	return 3*comp a + comp b;
}

void kvpair_zero(kvpair_t* pair)
{
	memset(pair, 0, sizeof(kvpair_t));
}

void kvpair_swap(kvpair_t* a, kvpair_t* b)
{
	kvpair_t tmp = *a;
	*a = *b;
	*b = tmp;
}
/*
void ptr_swap(void** a, void** b)
{
	*a = (void*)((intptr_t)*a ^ (intptr_t)*b);
	*b = (void*)((intptr_t)*b ^ (intptr_t)*a);
	*a = (void*)((intptr_t)*a ^ (intptr_t)*b);
}
*/

u_int32_t quick_hash(const char* input, size_t len)
{
	size_t bufflen = len + 4 - (len & 3); //len rounded to next 32 bit word
	size_t buffcount = bufflen/4;
	u_int32_t* buffer = malloc(bufflen);
	if(buffer == NULL) return 0;
	memset(buffer, 0, bufflen);
	memcpy(buffer, input, len);
	u_int32_t hash = 0;

	for(size_t i = 0; i < 32 * buffcount; i++)
		hash ^= (buffer[i % buffcount] & (1 << (i/buffcount))) << (i % 32);	
	free(buffer);
	return hash;
}

void foo_clear(foo_t* obj)
{
	switch(obj->type) {
		case TYPE_LIST:
			list_destroy(obj->value);
			break;
		case TYPE_DICT:
			dict_destroy(obj->value);
			break;
		default:
			free(obj->value);
			break;
	}
}
