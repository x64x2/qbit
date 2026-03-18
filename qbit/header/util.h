#ifndef UTIL_H_
#define UTIL_H_
#include <sys/types.h>
#include <semaphore.h>

typedef enum {
	TYPE_NULL=0,
	TYPE_INT=1,
	TYPE_STRING=2,
	TYPE_DICT=3,
	TYPE_LIST=4
} ben_val_t;

typedef struct {
	void* value;
	ben_val_t type;
} foo_t;

typedef struct {
	void* first;
	void* second;
} pair_t; 

typedef struct {
	char* key;
	foo_t data;
} kvpair_t;

void kvpair_zero(kvpair_t* pair);
void kvpair_swap(kvpair_t* a, kvpair_t* b);

typedef struct {
	void** items;
	size_t len;
	size_t max;
	size_t start;
	size_t end;
	sem_t sem_write;
} Queue_t;

int queue_enqueue(Queue_t* queue, void* item);
void* queue_dequeue(Queue_t* queue);
void* queue_peek_last(Queue_t* queue);
void* queue_peek_next(Queue_t* queue);
int queue_init(Queue_t* queue, size_t max);
void queue_destroy(Queue_t* queue);
int queue_lock(Queue_t* queue);
void queue_unlock(Queue_t* queue);

#define QUEUE_FULL(x) (x.len == x.max)
#define QUEUE_EMPTY(x) (x.len == 0)

typedef struct __Dict {
	kvpair_t item1;
	kvpair_t item2;
	struct __Dict* left;
	struct __Dict* mid;
	struct __Dict* right;
} Dict_t;

Dict_t* dict_create();
void dict_destroy(Dict_t* dict);
kvpair_t dict_lookup(Dict_t* dict, char* key);
Dict_t* dict_insert(Dict_t* dict, kvpair_t val);
kvpair_t dict_remove(Dict_t* dict, char* key);

void dict_print(Dict_t* dict);

foo_t ben_decode(char* str);
typedef struct List {
	struct List* next;
	foo_t data;
} List_t;

List_t* list_create(foo_t data);
List_t* list_join(List_t* lhs, List_t* rhs);
void list_destroy(List_t* list);

char* str_buf(const char* str, size_t len);
u_int32_t quick_hash(const char* input, size_t len);

typedef enum {
	GR_BOTH = 4,
	GR_2_EQ_3 = 3,
	GR_2_LESS_3 = 2,
	EQ_2_GR_3 = 1,
	EQ_ALL = 0,
	EQ_2_LESS_3 = -1,
	LESS_2_GR_3 = -2,
	LESS_2_EQ_3 = -3,
	LESS_BOTH = -4
} tricomp_return_t;

int tricomp(char* a, char* b, char* c);
void ptr_swap(void** a, void** b);
void bobj_clear(foo_t* obj);


