#ifndef SF_MALLOC_ASM_H
#define SF_MALLOC_ASM_H
#include <unistd.h>
#include <stddef.h>
#include <stdint.h>

static inline uint64_t get_timestamp() {                                  
  uint32_t  hi,lo;
  asm volatile (                                                      
      "rdtsc"
      : "=d" (hi)
      , "=a" (lo)
      ); 
  return (((uint64_t)hi)<<32)|((uint64_t)lo);
}
static inline int ffs_int(int v) {
  int pos;
  asm volatile (
      "bsf %1, %0"
      : "=r" (pos)
      : "r" (v)
      );
  return pos;
}
static inline int ffs_int64(int v) {
  int pos;
  asm volatile (
      "bsfq %1, %0"
      : "=r" (pos)
      : "q" (v)
      );
  return pos;
}
static inline int fls_int(int v){
  int pos;
  asm volatile(
      "bsr %1, %0"
      : "=r" (pos)
      :  "r" (v)
      );
  return pos;
}
static inline int fls_int64(int64_t v){
  int pos;
  asm volatile(
      "bsrq %1, %0"
      : "=r" (pos)
      :  "q" (v)
      );
  return pos;
}
/*  Written in 2014 by Sebastiano Vigna (vigna@acm.org)

To the extent possible under law, the author has dedicated all copyright
and related and neighboring rights to this software to the public domain
worldwide. This software is distributed without any warranty.

See <http://creativecommons.org/publicdomain/zero/1.0/>. */


#endif
