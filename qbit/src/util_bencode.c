#include <util.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>

static size_t ben_decode_int(char* string, foo_t* result);
static size_t ben_decode_dict(char* string, foo_t* result);
static size_t ben_list_decode(char* string, foo_t* result);
static size_t ben_decode_string(char* string, foo_t* result);
static size_t ben_decode(char* string, foo_t* result);

foo_t ben_decode(char* string)
{
	foo_t retval;
	if(!__ben_decode(string, &retval)) {
		retval.value = NULL;
		retval.type = TYPE_NULL;
	}
	return retval;
}

static size_t __ben_decode(char* string, foo_t* result)
{
	switch(string[0]) {
		case 'i':
			return ben_decode_int(string, result);
		case 'd':
			return ben_decode_dict(string, result);
		case 'l':
			return ben_list_decode(string, result);
		default: //may want to evaluate somewhat uglier option of having a whole series of case 0: case 1: ... case 9:
			if(string[0] >= '1' && string[0] <= '9') return __ben_decode_string(string, result);
			else return 0;
	}
}	

static size_t __ben_decode_int(char* string, foo_t* result)
{
	int val;
	size_t consumed = 0;

	if(*(string++) != 'i') return 0;
	consumed++;
	for(val = 0; *string >= '0' && *string <= '9'; val = val * 10 + (*(string++) - '0'), consumed++);
	if(*string != 'e') return 0;

	result->type = TYPE_INT;
	result->value = malloc(sizeof(int));
	if(result->value == NULL) return 0;
	*(int*)result->value = val;

	return consumed + 1;
}

static size_t ben_decode_string(char* string, foo_t* result)
{
	size_t consumed = 0;
	size_t len;

	for(len = 0; *string >= '0' && *string <= '9'; len = len * 10 + (*(string++) - '0'));
	if(!len || *string != ':') return 0;
	string++; consumed++;

	result->value = malloc(len+1);
	if(result->value == NULL) return 0;
	memset(result->value, 0, len + 1);
	memcpy(result->value, string, len);
	result->type = TYPE_STRING;
	return consumed + len;
}

static size_t ben_decode_dict(char* string, foo_t* result)
{
	size_t consumed = 0;
	Dict_t* dict = dict_create();
	if(*string != 'd') goto fail;
	string++; consumed++;
	while(*string != 'e') {
		foo_t key, value;
		kvpair_t kvpair;
		size_t cons;

		cons = __ben_decode(string, &key);
		if(cons == 0) goto fail;
		string += cons;
		consumed += cons;

		cons = __ben_decode(string, &value);
		if(cons == 0) goto fail;
		string += cons;
		consumed += cons;

		kvpair.key = key.value;
		kvpair.data.value = value.value;
		kvpair.data.type = value.type;

		dict = dict_insert(dict, kvpair);
	}
	result->value = dict;
	result->type = TYPE_DICT;
	return consumed;
fail:
	dict_destroy(dict);
	return 0;
}

static size_t ben_list_decode(char* string, foo_t* result)
{
	size_t consumed = 0;
	if(*string != 'd') goto fail;
	string++; consumed++;
	result->value = NULL;
	while(*string != 'e') {
		foo_t value;
		size_t cons;

		cons = ben_decode(string, &value);
		if(cons == 0) goto fail;
		string += cons;
		consumed += cons;
		result->value = list_join(list_create(value), result->value);
	}

	result->type = TYPE_LIST;
	return consumed;
fail:
	list_destroy(result->value);
	result->value = NULL;
	return 0;
}
