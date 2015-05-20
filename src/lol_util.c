#ifndef __USE_GNU
#define __USE_GNU 
#endif
#ifdef __cplusplus
extern "C" {
#endif
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include <errno.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "include/lol_util.h"
#define LT(n) n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n
static const char log_table[256] = {
  -1, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
  LT(4), LT(5), LT(5), LT(6), LT(6), LT(6), LT(6),
  LT(7), LT(7), LT(7), LT(7), LT(7), LT(7), LT(7), LT(7)
};
#undef LT
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

uint64_t threadrand_128(){
  static __thread xorshift128_state_t  state = {.s={0,0}};
  if(UNLIKELY(!state.s[0]&&!state.s[1])){
    state.s[0] = (uint64_t)&state;
    state.s[1] = get_timestamp();
    xorshift128plus(&state);
  }
  return xorshift128plus(&state);
}
uint64_t threadrand_1024(){
  static __thread xorshift1024_state_t state = {.s={0}};
  static __thread bool initialized = false;
  if(UNLIKELY(!initialized)){
    for(int i = 0; i< 16; i++)
      state.s[i] = threadrand_128();
  }
  return xorshift1024star(&state);
}
uint32_t threadrand_minstd(){
  static __thread uint64_t state = 0;
  if(UNLIKELY(!state))state = (uint64_t)&state ^ get_timestamp();
  uint64_t tmp = state * 48271UL;
  tmp = (tmp&0x7FFFFFFF)+(tmp>>31);
  state = (tmp >0x7FFFFFFF)?(tmp&0x7FFFFFFF)+(tmp>>31):tmp;
  return (uint32_t)state;
};

// base-2 logarithm of 32-bit integers
int Log2(size_t v) {
  unsigned int t, tt, r;  // temp vars
  if ((tt = (v >> 16))) {
    r =  (t = (tt >> 8)) ? 24 + log_table[t] : 16 + log_table[tt];
  } else {
    r =  (t = (v >> 8)) ? 8 + log_table[t] : log_table[v];
  }
  return r;
}


int32_t CpusOnline() {
  static __attribute__((aligned(64))) int32_t cpus_online = -1;
  if (cpus_online == -1) {
    cpus_online = (int32_t)sysconf(_SC_NPROCESSORS_ONLN);
    if ((cpus_online == - 1) ||  /* sysconf failed */
        (cpus_online == 0)) {    /* this should not happen */
      HANDLE_ERROR("CpusOnline failed\n");
    }
  }
  return cpus_online;
}
#ifdef __cplusplus
};
#endif
