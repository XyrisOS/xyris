/**
 * @file heap.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief
 * @version 0.3
 * @date 2019-11-22
 *
 * @copyright Copyright the Panix Contributors (c) 2019
 *
 */

#include <mem/heap.hpp>
#include <sys/panic.hpp>
#include <lib/stddef.hpp>
#include <lib/assert.hpp>
#include <lib/linked_list.hpp>

#define MEM_SIZE (1024*1024*2) // 256
#define MAX_BLOCK (1024*1024)
#define NUM_SLOTS 1024

#define DLIST_REMOVE_FROM(h, d, l)					\
    {									            \
        typeof(**h) **h_ = h, *d_ = d;				\
        DList *head = &(*h_)->l;					\
        dlist_remove_from(&head, &d_->l);			\
        if (head == NULL) {						    \
            *h_ = NULL;							    \
        } else {							        \
            *h_ = CONTAINER(typeof(**h), l, head);	\
        }								            \
    }

#define DLIST_PUSH(h, v, l)						    \
    {									            \
        typeof(*v) **h_ = h, *v_ = v;				\
        DList *head = &(*h_)->l;					\
        if (*h_ == NULL) head = NULL;				\
        dlist_push(&head, &v_->l);					\
        *h_ = CONTAINER(typeof(*v), l, head);		\
    }

#define DLIST_POP(h, l)							    \
    ({									            \
	typeof(**h) **h_ = h;						    \
	DList *head = &(*h_)->l;					    \
	DList *res = dlist_pop(&head);					\
	if (head == NULL) {						        \
	    *h_ = NULL;							        \
	} else {							            \
	    *h_ = CONTAINER(typeof(**h), l, head);		\
	}								                \
	CONTAINER(typeof(**h), l, res);					\
    })

#define DLIST_ITERATOR_BEGIN(h, l, it)				\
    {									            \
        typeof(*h) *h_ = h;						    \
	    DList *last_##it = h_->l.prev, *iter_##it = &h_->l, *next_##it;	\
	do {								            \
	    if (iter_##it == last_##it) {				\
		    next_##it = NULL;					    \
	    } else {							        \
		    next_##it = iter_##it->next;			\
	    }								            \
	    typeof(*h)* it = CONTAINER(typeof(*h), l, iter_##it);

#define DLIST_ITERATOR_END(it)						\
	} while((iter_##it = next_##it));				\
    }

#define DLIST_ITERATOR_REMOVE_FROM(h, it, l) DLIST_REMOVE_FROM(h, iter_##it, l)

#define CONTAINER(C, l, v) ((C*)(((char*)v) - (intptr_t)&(((C*)0)->l)))
#define OFFSETOF(TYPE, MEMBER)  __builtin_offsetof (TYPE, MEMBER)
#define DLIST_INIT(v, l) dlist_init(&v->l)

typedef struct Chunk Chunk;
struct Chunk {
    DList all;
    int used;
    union {
	char data[0];
	DList free;
    };
};

enum {
    NUM_SIZES = 32,
    ALIGN = 4,
    MIN_SIZE = sizeof(DList),
    HEADER_SIZE = OFFSETOF(Chunk, data),
};

Chunk* free_chunk[NUM_SIZES] = { NULL };
size_t mem_free = 0;
size_t mem_used = 0;
size_t mem_meta = 0;
Chunk* first = NULL;
Chunk* last = NULL;

char MEM[MEM_SIZE] = { 0 };
void* slot[NUM_SLOTS] = { NULL };

static void memory_chunk_init(Chunk *chunk) {
	//printf("%s(%p)\n", __FUNCTION__, chunk);
    DLIST_INIT(chunk, all);
    chunk->used = 0;
    DLIST_INIT(chunk, free);
}

static size_t memory_chunk_size(const Chunk *chunk) {
	//printf("%s(%p)\n", __FUNCTION__, chunk);
    char *end = (char*)(chunk->all.next);
    char *start = (char*)(&chunk->all);
    return (end - start) - HEADER_SIZE;
}

static int memory_chunk_slot(size_t size) {
    int n = -1;
    while(size > 0) {
	++n;
	size /= 2;
    }
    return n;
}

void mrvn_memory_init(void *mem, size_t size) {
    char *mem_start = (char *)(((uintptr_t)mem + ALIGN - 1) & (~(ALIGN - 1)));
    char *mem_end = (char *)(((uintptr_t)mem + size) & (~(ALIGN - 1)));
    first = (Chunk*)mem_start;
    Chunk *second = first + 1;
    last = ((Chunk*)mem_end) - 1;
    memory_chunk_init(first);
    memory_chunk_init(second);
    memory_chunk_init(last);
    dlist_insert_after(&first->all, &second->all);
    dlist_insert_after(&second->all, &last->all);
    // make first/last as used so they never get merged
    first->used = 1;
    last->used = 1;

    size_t len = memory_chunk_size(second);
    int n = memory_chunk_slot(len);
    //printf("%s(%p, %#lx) : adding chunk %#lx [%d]\n", __FUNCTION__, mem, size, len, n);
    DLIST_PUSH(&free_chunk[n], second, free);
    mem_free = len - HEADER_SIZE;
    mem_meta = sizeof(Chunk) * 2 + HEADER_SIZE;
}

void *mrvn_malloc(size_t size) {
    //printf("%s(%#lx)\n", __FUNCTION__, size);
    size = (size + ALIGN - 1) & (~(ALIGN - 1));

	if (size < MIN_SIZE) size = MIN_SIZE;

	int n = memory_chunk_slot(size - 1) + 1;

	if (n >= NUM_SIZES) return NULL;

	while(!free_chunk[n]) {
		++n;
		if (n >= NUM_SIZES) return NULL;
    }

	Chunk *chunk = DLIST_POP(&free_chunk[n], free);
    size_t size2 = memory_chunk_size(chunk);
	//printf("@ %p [%#lx]\n", chunk, size2);
    size_t len = 0;

	if (size + sizeof(Chunk) <= size2) {
		Chunk *chunk2 = (Chunk*)(((char*)chunk) + HEADER_SIZE + size);
		memory_chunk_init(chunk2);
		dlist_insert_after(&chunk->all, &chunk2->all);
		len = memory_chunk_size(chunk2);
		int n = memory_chunk_slot(len);
		//printf("  adding chunk @ %p %#lx [%d]\n", chunk2, len, n);
		DLIST_PUSH(&free_chunk[n], chunk2, free);
		mem_meta += HEADER_SIZE;
		mem_free += len - HEADER_SIZE;
    }

	chunk->used = 1;
    //memset(chunk->data, 0xAA, size);
	//printf("AAAA\n");
    mem_free -= size2;
    mem_used += size2 - len - HEADER_SIZE;
    //printf("  = %p [%p]\n", chunk->data, chunk);
    return chunk->data;
}

static void remove_free(Chunk *chunk) {
    size_t len = memory_chunk_size(chunk);
    int n = memory_chunk_slot(len);
    //printf("%s(%p) : removing chunk %#lx [%d]\n", __FUNCTION__, chunk, len, n);
    DLIST_REMOVE_FROM(&free_chunk[n], chunk, free);
    mem_free -= len - HEADER_SIZE;
}

static void push_free(Chunk *chunk) {
    size_t len = memory_chunk_size(chunk);
    int n = memory_chunk_slot(len);
    //printf("%s(%p) : adding chunk %#lx [%d]\n", __FUNCTION__, chunk, len, n);
    DLIST_PUSH(&free_chunk[n], chunk, free);
    mem_free += len - HEADER_SIZE;
}

void mrvn_free(void *mem) {
    Chunk *chunk = (Chunk*)((char*)mem - HEADER_SIZE);
    Chunk *next = CONTAINER(Chunk, all, chunk->all.next);
    Chunk *prev = CONTAINER(Chunk, all, chunk->all.prev);

	//printf("%s(%p): @%p %#lx [%d]\n", __FUNCTION__, mem, chunk, memory_chunk_size(chunk), memory_chunk_slot(memory_chunk_size(chunk)));
    mem_used -= memory_chunk_size(chunk);

    if (next->used == 0) {
		// merge in next
		remove_free(next);
		dlist_remove(&next->all);
		//memset(next, 0xDD, sizeof(Chunk));
		mem_meta -= HEADER_SIZE;
		mem_free += HEADER_SIZE;
    }
    if (prev->used == 0) {
		// merge to prev
		remove_free(prev);
		dlist_remove(&chunk->all);
		//memset(chunk, 0xDD, sizeof(Chunk));
		push_free(prev);
		mem_meta -= HEADER_SIZE;
		mem_free += HEADER_SIZE;
    } else {
		// make chunk as free
		chunk->used = 0;
		DLIST_INIT(chunk, free);
		push_free(chunk);
    }
}

void check(void) {
	int	i;
    Chunk *t = last;

	DLIST_ITERATOR_BEGIN(first, all, it) {
		assert(CONTAINER(Chunk, all, it->all.prev) == t);
		t = it;
    } DLIST_ITERATOR_END(it);

    for(i = 0; i < NUM_SIZES; ++i) {
		if (free_chunk[i]) {
			t = CONTAINER(Chunk, free, free_chunk[i]->free.prev);
			DLIST_ITERATOR_BEGIN(free_chunk[i], free, it) {
			assert(CONTAINER(Chunk, free, it->free.prev) == t);
			t = it;
			} DLIST_ITERATOR_END(it);
		}
    }
}
