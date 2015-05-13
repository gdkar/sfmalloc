/*****************************************************************************/
/*                                                                           */
/* Copyright (c) 2011, Seoul National University.                            */
/* All rights reserved.                                                      */
/*                                                                           */
/* Redistribution and use in source and binary forms, with or without        */
/* modification, are permitted provided that the following conditions        */
/* are met:                                                                  */
/*   1. Redistributions of source code must retain the above copyright       */
/*      notice, this list of conditions and the following disclaimer.        */
/*   2. Redistributions in binary form must reproduce the above copyright    */
/*      notice, this list of conditions and the following disclaimer in the  */
/*      documentation and/or other materials provided with the distribution. */
/*   3. Neither the name of Seoul National University nor the names of its   */
/*      contributors may be used to endorse or promote products derived      */
/*      from this software without specific prior written permission.        */
/*                                                                           */
/* THIS SOFTWARE IS PROVIDED BY SEOUL NATIONAL UNIVERSITY "AS IS" AND ANY    */
/* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED */
/* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE    */
/* DISCLAIMED. IN NO EVENT SHALL SEOUL NATIONAL UNIVERSITY BE LIABLE FOR ANY */
/* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL        */
/* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS   */
/* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)     */
/* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,       */
/* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN  */
/* ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE           */
/* POSSIBILITY OF SUCH DAMAGE.                                               */
/*                                                                           */
/* Contact information:                                                      */
/*   Center for Manycore Programming                                         */
/*   School of Computer Science and Engineering                              */
/*   Seoul National University, Seoul 151-744, Korea                         */
/*   http://aces.snu.ac.kr                                                   */
/*                                                                           */
/* Contributors:                                                             */
/*   Sangmin Seo, Junghyun Kim, and Jaejin Lee                               */
/*                                                                           */
/*****************************************************************************/

#ifndef __SF_MALLOC_ATOMIC_H__
#define __SF_MALLOC_ATOMIC_H__

#include <stdint.h>
#include <stdbool.h>

static inline int atomic_xchg_int(volatile int *addr, int val) {
  asm volatile (
      /*lock*/ "xchgl %0, %1"
      : "=r" (val)
      : "m" (*addr), "0" (val)
      : "memory"
      );
  return val;
}
static inline int atomic_xchg_int64(volatile int64_t *addr, int64_t val) {
  asm volatile (
      /*lock*/ "xchgq %0, %1"
      : "=r" (val)
      : "m" (*addr), "0" (val)
      : "memory"
      );
  return val;
}
static inline unsigned atomic_xchg_uint(volatile unsigned *addr, 
                                        unsigned val) {
  asm volatile (
      /*lock*/"xchgl %0, %1"
      : "=r" (val)
      : "m" (*addr), "0" (val)
      : "memory"
      );
  return val;
}
static inline uint64_t atomic_xchg_uint64(volatile uint64_t  *addr, 
                                                   uint64_t  val) {
  asm volatile (
      /*lock*/"xchgq %0, %1"
      : "=q" (val)
      : "m" (*addr), "0" (val)
      : "memory"
      );
  return val;
}
static inline void* _atomic_xchg_ptr(volatile void **addr, void*  val) {
  uint64_t uival = (uint64_t )val;
  asm volatile (
      /*lock*/"xchgq %0, %1"
      : "=q" (uival)
      : "m" (*(uint64_t*)addr), "0" (uival)
      : "memory"
      );
  return (void*)uival;
}
#define atomic_xchg_ptr(_addr,_val) ((typeof(*_addr))_atomic_xchg_ptr((volatile void**)(_addr),(void*)(_val)))
/*
   This function atomically increment the value of *addr and returns the 
   value of *addr before the increment.
 */
static inline int atomic_inc_int(volatile int *addr) {
  int val = 1;
  asm volatile (
      "lock; xaddl %0, %1"
      : "=r" (val)
      : "m" (*addr), "0" (val)
      : "memory"
      );
  return val;
}
static inline unsigned atomic_inc_uint(volatile unsigned *addr) {
  unsigned val = 1;
  asm volatile (
      "lock; xaddl %0, %1"
      : "=r" (val)
      : "m" (*addr), "0" (val)
      : "memory"
      );
  return val;
}
static inline int atomic_dec_int(volatile int *addr) {
  int val = -1;
  asm volatile (
      "lock; xaddl %0, %1"
      : "=r" (val)
      : "m" (*addr), "0" (val)
      : "memory"
      );
  return val;
}
static inline int atomic_add_int(volatile int *addr, int val) {
  asm volatile (
      "lock; xaddl %0, %1"
      : "=r" (val)
      : "m" (*addr), "0" (val)
      );
  return val;
}
static inline unsigned atomic_add_uint(volatile unsigned *addr,
                                       unsigned val) {
  asm volatile (
      "lock; xaddl %0, %1"
      : "=r" (val)
      : "m" (*addr), "0" (val)
      );
  return val;
}
static inline int64_t atomic_add_int64(volatile int64_t *addr,
                                                int64_t val) {
  asm volatile (
      "lock; xaddq %0, %1"
      : "=q" (val)
      : "m" (*addr), "0" (val)
      );
  return val;
}
static inline uint64_t atomic_add_uint64(volatile uint64_t *addr,
                                                  uint64_t val) {
  asm volatile (
      "lock; xaddq %0, %1"
      : "=q" (val)
      : "m" (*addr), "0" (val)
      );
  return val;
}
static inline void* _atomic_add_ptr(volatile void **addr, ptrdiff_t diff){
  return (void*)atomic_add_int64((volatile int64_t*)addr, (int64_t)diff);
}
#define atomic_add_ptr(_addr,_val) ((typeof(*_addr))_atomic_add_ptr((volatile void**)(_addr),(ptrdiff_t)(_val)))
static inline bool cas32(volatile unsigned *addr, unsigned old_val,
                                                  unsigned new_val) {
	unsigned prev = 0;
  asm volatile (
      "lock; cmpxchgl %1, %2"
      : "=a" (prev)
      : "r" (new_val), "m" (*addr), "0" (old_val)
      : "memory");

	return prev == old_val;
}
static inline bool cas64(volatile uint64_t *addr, 
                                  uint64_t old_val,
                                  uint64_t new_val) {
#if 0
  return __sync_bool_compare_and_swap(addr, old_val, new_val);
#else
	uint64_t prev = 0;
  asm volatile (
      "lock; cmpxchgq %1, %2"
      : "=a" (prev)
      : "r" (new_val), "m" (*addr), "0" (old_val)
      : "memory");

	return prev == old_val;
#endif
}
static inline bool _cas_ptr(volatile void **addr, void *old_ptr, 
                                                void *new_ptr) {
	return cas64((volatile uint64_t *)addr, (uint64_t)old_ptr,
                                          (uint64_t)new_ptr); 
}
#define cas_ptr(_addr,_old_ptr,_new_ptr)  _cas_ptr((volatile void**)(_addr),(void*)(_old_ptr),(void*)(_new_ptr))
#endif //__SF_MALLOC_ATOMIC_H__

