/**
 * @file heap.hpp
 * @author Goswin von Brederlow (goswin-v-b@web.de)
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief
 * @version 0.3
 * @date 2019-11-22
 *
 * @copyright Copyright the Panix Contributors (c) 2019
 * Code shamelessly taken from the OSDev Wiki. The article
 * can be found at the link below.
 * 
 * https://wiki.osdev.org/User:Mrvn/LinkedListBucketHeapImplementation
 */

#ifndef PANIX_MEM_HEAP
#define PANIX_MEM_HEAP

#include <stdint.h>
#include <lib/linked_list.hpp>

typedef struct px_heap_chunk {
    DList all;
    int used;
    union {
	    char data[0];
	    DList free;
    };
} px_heap_chunk_t;

enum {
    NUM_SIZES = 32,
    ALIGN = 4,
    MIN_SIZE = sizeof(DList),
    HEADER_SIZE = __builtin_offsetof(px_heap_chunk_t, data)
};

#endif /* PANIX_MEM_HEAP */