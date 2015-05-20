#ifndef LOL_ASM_H
#define LOL_ASM_H
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
static inline int bts(void *base, int bit){
  int ret;
  asm volatile(
      "bts %2, (%1);\n\t"
      "setc %0;\n\t"
    : "=r"(ret)
    :  "q"(base)
    ,  "Ir"(bit)
    : "cc", "memory");
  return ret;
}
static inline int btr(void *base, int bit){
  int ret;
  asm volatile(
      "bts %2, (%1);\n\t"
      "setc %0;\n\t"
    : "=r"(ret)
    :  "q"(base)
    ,  "Ir"(bit)
    : "cc", "memory");
  return ret;
}
static inline int btc(void *base, int bit){
  int ret;
  asm volatile(
      "btc %2, (%1);\n\t"
      "setc %0;\n\t"
    : "=r"(ret)
    :  "q"(base)
    ,  "Ir"(bit)
    : "cc", "memory");
  return ret;
}
static inline void atomic_inc(int64_t *addr){
  asm volatile(
      "lock; incq (%0);\n\t"
     :
     : "q"(addr)
     : "cc", "memory"); 
}
static inline void atomic_dec(int64_t *addr){
  asm volatile(
      "lock; decq (%0);\n\t"
     :
     : "q"(addr)
     : "cc", "memory"); 
}
static inline void atomic_add(int64_t *addr,int64_t val){
  asm volatile(
      "lock; addq %1, (%0);\n\t"
     :
     : "q"(addr)
     , "q"(val)
     : "cc", "memory"); 
}
static inline void atomic_sub(int64_t *addr,int64_t val){
  asm volatile(
      "lock; subq %1, (%0);\n\t"
     :
     : "q"(addr)
     , "q"(val)
     : "cc", "memory"); 
}
static inline void atomic_and(int64_t *addr,int64_t val){
  asm volatile(
      "lock; andq %1, (%0);\n\t"
     :
     : "q"(addr)
     , "q"(val)
     : "cc", "memory"); 
}
static inline void atomic_or(int64_t *addr,int64_t val){
  asm volatile(
      "lock; orq %1, (%0);\n\t"
     :
     : "q"(addr)
     , "q"(val)
     : "cc", "memory"); 
}
static inline void atomic_xor(int64_t *addr,int64_t val){
  asm volatile(
      "lock; xorq %1, (%0);\n\t"
     :
     : "q"(addr)
     , "q"(val)
     : "cc", "memory"); 
}
static inline int64_t atomic_xchg(int64_t *addr, int64_t with){
  asm volatile(
      "xchgq %0, (%1);\n\t"
    : "+q"(with)
    :  "q"(addr)
    : "cc", "memory");
  return with;
}
#endif
