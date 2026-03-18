#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

int dict_count(Dict_t* dict) {
	if(dict == NULL) return 0;
	return
		dict_count(dict->left) +
		dict_count(dict->mid) +
		dict_count(dict->right) +
		((dict->item2.type == TYPE_NULL) ? 1 : 2);
}

int main(int argc, char** argv)
{
	Dict_t* mydict = dict_create();
	char* buffer = malloc(50);
	kvpair_t next;

	next.key = NULL;
	next.type = TYPE_STRING;
	next.value = "VALUE";

	while(fgets(buffer, 50, stdin)) {
		buffer[strlen(buffer)-1] = '\0';
		buffer = realloc(buffer, strlen(buffer)+1);

		next.key = buffer;
		buffer = malloc(50);

		if(next.value != NULL)
			mydict = dict_insert(next, mydict);
	}

	dict_remove("watercolored", mydict);
	printf("%d", dict_count(mydict));
	dict_print(mydict);
	return 0;
}
