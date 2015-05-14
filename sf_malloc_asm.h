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


/* This is the fastest generator passing BigCrush without
   systematic failures, but due to the relatively short period it is
   acceptable only for applications with a mild amount of parallelism;
   otherwise, use a xorshift1024* generator.

   The state must be seeded so that it is not everywhere zero. If you have
   a nonzero 64-bit seed, we suggest to pass it twice through
   MurmurHash3's avalanching function. */
typedef struct {
  uint64_t  s[2];
}xorshift128_state_t;

typedef struct {
  uint64_t    s[16];
  int         index;
}xorshift1024_state_t __attribute__((aligned(CACHE_LINE_SIZE)));

static inline uint64_t xorshift128plus(xorshift128_state_t* state) { 
	uint64_t s1 = state->s[ 0 ];
	const uint64_t s0 = state->s[ 1 ];
	state->s[ 0 ] = s0;
	s1 ^= s1 << 23; // a
	return ( state->s[ 1 ] = ( s1 ^ s0 ^ ( s1 >> 17 ) ^ ( s0 >> 26 ) ) ) + s0; // b, c
}
static inline uint64_t xorshift1024star(xorshift1024_state_t* state){
  uint64_t s0 = state->s[0];
  int p = (state->index = (state->index+1)&15);;
  uint64_t s1 = state->s[p];
  s1 ^=(s1 << 31);
  s1 ^=(s1 >> 11);
  s0 ^=(s0 >> 30);
  return (state->s[ p ] = s0 ^ s1 ) * 1181783497276652981LL;
}

static inline uint64_t threadrand_128(){
  static __thread xorshift128_state_t  state = {.s={0,0}};
  if(UNLIKELY(!state.s[0]&&!state.s[1])){
    state.s[0] = (uint64_t)&state;
    state.s[1] = get_timestamp();
    xorshift128plus(&state);
  }
  return xorshift128plus(&state);
}
static inline uint64_t threadrand_1024(){
  static __thread xorshift1024_state_t state = {.s={0}};
  static __thread bool initialized = false;
  if(UNLIKELY(!initialized)){
    for(int i = 0; i< 16; i++)
      state.s[i] = threadrand_128();
  }
  return xorshift1024star(&state);
}
#endif
