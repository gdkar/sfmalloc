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
#include <stdatomic.h>

#define atomic_xchg_int(addr, val) atomic_exchange(addr,val)
#define atomic_xchg_uint(addr, val) atomic_exchange(addr,val)
#define atomic_xchg_ptr(addr, val) atomic_exchange(addr,val)
#define atomic_xchg(addr, val) atomic_exchange(addr,val)
#define atomic_inc_int(addr) atomic_fetch_add(addr,1)
#define atomic_inc_uint(addr) atomic_fetch_add(addr,1)
#define atomic_inc_ptr(addr) atomic_fetch_add(addr,1)
#define atomic_inc(addr) atomic_fetch_add(addr,1)
#define atomic_dec_int(addr) atomic_fetch_add(addr,-1)
#define atomic_dec_uint(addr) atomic_fetch_add(addr,-1)
#define atomic_dec_int64(addr) atomic_fetch_add(addr,-1)
#define atomic_dec_uint64(addr) atomic_fetch_add(addr,-1)
#define atomic_dec(addr) atomic_fetch_add(addr,-1)
#define CAS32(addr, expected, desired) atomic_compare_exchange_strong(addr,expected,desired)
#define CAS64(addr, expected, desired) atomic_compare_exchange_strong(addr,expected,desired)
#define CAS_ptr(addr, expected, desired) atomic_compare_exchange_strong(addr,expected,desired)

#endif //__SF_MALLOC_ATOMIC_H__

