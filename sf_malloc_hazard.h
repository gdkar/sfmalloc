#ifndef SF_MALLOC_HAZARD_H
#define SF_MALLOC_HAZARD_H
#include <unistd.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "sf_malloc_atomic.h"
#include "sf_malloc_def.h"
void* do_mmap(size_t size);
typedef struct hazard_ptr hazard_ptr_t;
typedef struct hazard_list hazard_list_t;
struct hazard_ptr {
  hazard_ptr_t      *next;
  volatile uint32_t  count;
  volatile uint32_t  active;
  void*              node[(CACHE_LINE_SIZE-(sizeof(void*)+2*sizeof(uint32_t)))/sizeof(void*)];
};
struct hazard_list{
  hazard_ptr_t      *head;
  volatile uint32_t  num_free;
}__attribute__((aligned(CACHE_LINE_SIZE)));
static hazard_list_t g_hazard_list = (struct hazard_list){0,0};
hazard_ptr_t* hazard_ptr_alloc();
hazard_list_t * g_hazard();

void          hazard_ptr_free(hazard_ptr_t*hp);
void*         hazard_ptr_set(hazard_ptr_t*hp, void**pptr);
void          hazard_ptr_clr(hazard_ptr_t*hp, void*ptr);
bool          hazard_ptr_scan_single(void *ptr);
#endif
