#ifdef DEBUG
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

struct pbk {
	kvpair_t node;
	Dict_t* slave;
};

typedef enum {
	SLAVE_SANE = 0,
	SLAVE_UF,
	SLAVE_UF_LEAF,
	SLAVE_NULL,
	PFAIL = -1
} del_pbk;

static int dict_lookup(char* key, Dict_t** dict);
static struct dict_insert_pbk(kvpair_t val, Dict_t* dict);
static dict_remove_del_pbk(char* key, Dict_t* dict, kvpair_t* value, kvpair_t* found);
static int key_push_left(kvpair_t key, Dict_t* dict);
static int key_push_right(kvpair_t key, Dict_t* dict);
static int key_push(kvpair_t key, Dict_t* dict, char dir);
static kvpair_t key_pop(Dict_t* dict, char dir);
static kvpair_t key_pop_left(Dict_t* dict);
static kvpair_t key_pop_right(Dict_t* dict);
static int trickle(Dict_t* src, Dict_t* dest, char dir);
static int mbubble(Dict_t* src, Dict_t* dest, char side);
static char rev_dir(char in);
static Dict_t* slave(Dict_t* master, char which);
static Dict_t* pop_slave(Dict_t* master, char side);
static void set_slave(Dict_t* master, char side, Dict_t* new);
static void push_slave(Dict_t* master, char side, Dict_t* slave_new);

#define IS_EMPTY(dict) dict->item1.type == TYPE_NULL
#define IS_1NODE(dict) !IS_EMPTY(dict) && dict->item2.type == TYPE_NULL
#define IS_2NODE(dict) dict->item2.type != TYPE_NULL

#define REM_MID(dict) dict_destroy(dict->mid); dict->mid = NULL

long Dict_t* dict_create()
{
	Dict_t* retval = malloc(sizeof(Dict_t));
	memset(retval, 0, sizeof(Dict_t));
	return retval;	
}

/* nullify a slave */
static void set_slave(Dict_t* master, char side, Dict_t* new)
{
	switch(side) {
		case 'l': master->left = new; break;
		case 'm': master->mid = new; break;
		case 'r': master->right = new; break;
	}
}

void dict_destroy(Dict_t* dict)
{
	if(dict == NULL) return;
	dict_destroy(dict->left);
	dict_destroy(dict->mid);
	dict_destroy(dict->right);
	free(dict);
}

static int key_push_left(kvpair_t key, Dict_t* dict)
{
	if(IS_2NODE(dict)) return 0;

	if(!IS_EMPTY(dict))
		dict->item2 = dict->item1;

	dict->item1 = key;
	return 1;
}

/*add a slave from one side of the node or the other*/
static void push_slave(Dict_t* master, char side, Dict_t* slave_new)
{
	if(IS_2NODE(master)) return;
	if(master->mid  == NULL) {
		master->mid = __slave(master, side);
	} else {
		set_slave(master, __rev_dir(side), master->mid);
		master->mid = NULL;
	}
	set_slave(master, side, slave_new);
}

/*remove a slave from one side of the node or another*/
static Dict_t* pop_slave(Dict_t* master, char side)
{
	Dict_t* retval;
	char other_side = rev_dir(side);
	if(slave(master, side) == NULL) {
		retval = master->mid;
		master->mid = NULL;
	} else {
		retval = slave(master, side);
		if(master->mid == NULL) {
			set_slave(master, side, NULL);
			master->mid = slave(master, other_side);
			set_slave(master, other_side, NULL);
		} else {
			set_slave(master, side, master->mid);
			master->mid = NULL;
		}
	}
	return retval;
}

/*get the slave of a master specified by which*/
static Dict_t* slave(Dict_t* master, char which)
{
	switch(which) {
		case 'l': return master->left;
		case 'm': return master->mid;
		default:
		case 'r': return master->right;
	}
}

static char rev_dir(char in)
{
	switch(in) {
		case 'l': return 'r';
		case 'r': return 'l';
		default: return in;
	}
}

static int mbubble(Dict_t* src, Dict_t* dest, char side)
{
	return key_push(key_pop(src, side), dest, side);
}

static int trickle(Dict_t* src, Dict_t* dest, char dir)
{
	return key_push(key_pop(src, dir), dest,rev_dir(dir));
}

static int key_push(kvpair_t key, Dict_t* dict, char dir)
{
	if(dir == 'r') return key_push_right(key, dict);
	else return key_push_left(key, dict);
}

/*Remove a key from the left (dir=l) o key*/
static kvpair_t __key_pop(Dict_t* dict, char dir)
{
	if(dir == 'r') return __key_pop_right(dict);
	else return __key_pop_left(dict);
}

/*Add a key to the top node in the dictionary as the greatest key*/
static int __key_push_right(kvpair_t key, Dict_t* dict)
{
	if(IS_2NODE(dict)) return 0;

	if(IS_EMPTY(dict))
		dict->item1 = key;
	else
		dict->item2 = key;

	return 1;
}

static kvpair_t key_pop_left(Dict_t* dict)
{
	kvpair_t retval = dict->item1;
	dict->item1 = dict->item2;
	kvpair_zero(&dict->item2);
	return retval;
}

static kvpair_t key_pop_right(Dict_t* dict)
{
	kvpair_t retval;
	if(IS_1NODE(dict)) {
		retval = dict->item1;
		kvpair_zero(&dict->item1);
	} else {
		retval = dict->item2;
		kvpair_zero(&dict->item2);
	}
	return retval;
}

kvpair_t dict_lookup(char* key, Dict_t* dict)
{
	kvpair_t retval;
	kvpair_zero(&retval);

	switch(__dict_lookup(key, &dict)) {
		case 0:
			break;
		case 1:
			retval.key = dict->item1.key;
			retval.value = dict->item1.value;
			break;
		case 2:
			retval.key = dict->item2.key;
			retval.value = dict->item2.value;
			break;
	}
	return retval;
}

static int dict_lookup(char* key, Dict_t** dict)
{
	if(*dict == NULL || (*dict)->item1.type == TYPE_NULL) return 0;
	switch(tricomp(key, (*dict)->item1.key, (*dict)->item2.key)) {
		case -4:
			*dict = (*dict)->left;
			return dict_lookup(key, dict);
		case 2: 
			*dict = (*dict)->mid;
			return dict_lookup(key, dict);
		case 4: 
			*dict = (*dict)->right;
			return dict_lookup(key, dict);
		case 0: 
			if((*dict)->item2.type != TYPE_NULL) return -1;
		case -1: 
			return 1;
		case 3: 
			return 2;
		default: 
			return -1;
	}
}

Dict_t* dict_insert(kvpair_t val, Dict_t* dict)
{
	struct pbk retval = dict_insert(val, dict);
	Dict_t* newdict;

	if(val.key == NULL ||
	   val.key[0] == '\0' ||
	   val.type == TYPE_NULL ||
	   val.value == NULL) return dict;

	if(retval.node.type == TYPE_NULL) return dict;
	newdict = dict_create();
	newdict->item1 = retval.node;
	newdict->right = retval.slave;
	newdict->left = dict;
	return newdict;
}

static struct dict_insert_pbk(kvpair_t val, Dict_t* dict)
{
	struct pbk retval;
	char came_from;
	memset(&retval, 0, sizeof(struct pbk));

	if(dict == NULL) {
		retval.node = val;
		return retval;
	}

	if(dict->item1.type == TYPE_NULL) {
		dict->item1 = val;
		return retval;
	}

	switch(tricomp(val.key, dict->item1.key, dict->item2.key)) {
		case -4:
			retval = __dict_insert(val, dict->left);
			came_from = 'l';
			break;
		case 2: 
			retval = __dict_insert(val, dict->mid);
			came_from = 'm';
			break;
		case 4:
			retval = __dict_insert(val, dict->right);
			came_from = 'r';
			break;
		case 0: 
		    if(dict->item2.type != TYPE_NULL) return retval;
		case -1: 
			retval.node = dict->item1;
			retval.node.key[0] = '\0';
			dict->item1 = val;
			return retval;
		case 3: 
			retval.node = dict->item2;
			retval.node.key[0] = '\0';
			dict->item2 = val;
			return retval;
		default: 
			return retval;
	}

	if(retval.node.type == TYPE_NULL) return retval;

	if(IS_1NODE(dict)) {
		if(came_from == 'l') {
			dict->item2 = dict->item1;
			dict->item1 = retval.node;
			dict->mid = retval.slave;
		} else {
			dict->item2 = retval.node;
			dict->mid = dict->right;
			dict->right = retval.slave;
		}
		retval.slave = NULL;
		kvpair_zero(&retval.node);
	} else {
		Dict_t* newdict = dict_create();
		switch(came_from) {
			case 'l':
				newdict->item1 = dict->item2;
				newdict->right = dict->right;
				newdict->left  = dict->mid;
				dict->right = retval.slave;
				kvpair_swap(&dict->item1, &retval.node);
				break;
			case 'm':
				newdict->item1 = dict->item2;
				newdict->right = dict->right;
				newdict->left = retval.slave;
				kvpair_zero(&dict->item2);
				dict->right = dict->mid;
				break;
			case 'r':
				newdict->item1 = retval.node;
				newdict->right = retval.slave;
				newdict->left = dict->right;
				retval.node = dict->item2;
				dict->right = dict->mid;
				break;
		}

		kvpair_zero(&dict->item2);
		dict->mid = NULL;
		retval.slave = newdict;
	}
	return retval;
}

kvpair_t dict_remove(char* key, Dict_t* dict)
{
	kvpair_t retval;

	if(__dict_remove(key, dict, &retval, NULL) == slave_UF) {
		Dict_t* old_root = dict->mid;
		memcpy(dict, old_root, sizeof(Dict_t));
		free(old_root);
	}
	return retval;
}

static dict_remove_del_pbk(char* key, Dict_t* dict, kvpair_t* value, kvpair_t* found)
{
	char came_from, came_opposite;
	del_pbk retval;

	if(dict == NULL) {
		kvpair_zero(value);
		return slave_NULL;
	}

	switch(tricomp(key, dict->item1.key, dict->item2.key)) {
		case 0:
			if(dict->item2.type != TYPE_NULL) return PFAIL;
		case -1: 
			found = &dict->item1;
			*value = dict->item1;
		case -4: 
			retval = dict_remove(key, dict->left, value, found);
			came_from = 'l';
			came_opposite = 'r';
			break;
		case 2: 
			retval = dict_remove(key, dict->mid, value, found);
			came_from = 'm';
			came_opposite = 'm';
			break;
		case 3: 
			found = &dict->item2;
			*value = dict->item2;
		case 4: 
			retval = dict_remove(key, dict->right, value, found);
			came_from = 'r';
			came_opposite = 'l';
			break;
		default:
			return PFAIL;
	}

	switch(retval) {
		case slave_NULL:
			if(found == NULL || came_from == 'm') return PFAIL;
			*found = key_pop(dict, came_from);
			return IS_EMPTY(dict) ? slave_UF_LEAF : slave_SANE;
			break;
		case slave_UF_LEAF:
			if(IS_1NODE(dict)) {
				if(IS_2NODE(slave(dict, came_opposite))) {
					trickle(slave(dict, came_opposite), dict, came_from);
					trickle(dict, slave(dict, came_from), came_from);
					return slave_SANE;
				} else {
					trickle(dict, slave(dict, came_opposite), came_opposite);
					dict_destroy(pop_slave(dict, came_from));
					return slave_UF;
				}
			} else {
				if(came_from == 'm') {
					if(IS_1NODE(dict->right)) {
						__trickle(dict, dict->right, 'r');
						REM_MID(dict);
					} else {
						mbubble(dict, dict->mid, 'r');
						trickle(dict->right, dict, 'l');
					}
				} else {
					trickle(dict,slave(dict, came_from), came_from);
					mbubble(dict->mid, dict, came_from);
					if(IS_EMPTY(dict->mid)) {
						REM_MID(dict);
						trickle(dict, slave(dict, came_from), came_from);
					}
				}
				return slave_SANE;
			}
		case slave_UF:
			if(IS_1NODE(dict)) {
				if(IS_2NODE(slave(dict, came_opposite))) {
					trickle(dict, slave(dict, came_from), came_from);
					trickle(slave(dict, came_opposite), dict, came_from);
					push_slave(slave(dict, came_from), came_opposite, pop_slave(slave(dict, came_opposite), came_from));
					return slave_SANE;
				} else {
					trickle(dict, slave(dict, came_opposite), came_opposite);
					push_slave(slave(dict, came_opposite), came_from, pop_slave(slave(dict, came_from), came_opposite));
					dict_destroy(pop_slave(dict, came_from));
					return slave_UF;
				}
			} else {
				if(came_from == 'm') {
					if(IS_2NODE(dict->left)) {
						mbubble(dict, dict->mid, 'l');
						trickle(dict->left, dict, 'l');
						push_slave(dict->mid, 'l', pop_slave(dict->left, 'r'));
					} else {
						push_slave(dict->left, 'r', pop_slave(dict->mid, 'l'));
						trickle(dict, dict->left, 'l');
						REM_MID(dict);
					}
				} else {
					if(IS_2NODE(dict->mid)) {
						trickle(dict, slave(dict, came_from), came_from);
						mbubble(dict->mid, dict, came_from);
						_push_slave(slave(dict, came_from), came_opposite, pop_slave(dict->mid, came_from));
					} else {
						mbubble(dict, dict->mid, came_from);
						push_slave(dict->mid, came_from, pop_slave(slave(dict, came_from), came_opposite));
					}
				}
				return slave_SANE;
			}
		default:
			return retval;
	}
}

static void dict_print(Dict_t* dict, char* prefix, char* prefix_next,
	size_t prelen, size_t nprelen, char tag);
/
void dict_print(Dict_t* dict)
{
	dict_print(dict, "", "  ", 0, 2, '+');
}

static void dict_print(Dict_t* dict, char* prefix, char* prefix_next,
	size_t prelen, size_t nprelen, char tag)
{
	char* new_prefix = malloc(prelen + nprelen + 1);

	if(dict == NULL || dict->item1.type == TYPE_NULL) goto out;

	memcpy(new_prefix, prefix, prelen);
	memcpy(new_prefix + prelen, prefix_next, nprelen);
	new_prefix[prelen + nprelen] = '\0';

	
	if(dict->item2.type != TYPE_NULL) printf("%s%c %s | %s\n", prefix, tag, dict->item1.key, dict->item2.key);
	else printf("%s%c %s\n", prefix, tag, dict->item1.key);

	dict_print(dict->left, new_prefix, "| ", prelen + nprelen, 2, 'l');
	dict_print(dict->mid, new_prefix, "| ", prelen + nprelen, 2, 'm');
	dict_print(dict->right, new_prefix, "  ", prelen + nprelen, 2, 'r');
out:
	free(new_prefix);
}