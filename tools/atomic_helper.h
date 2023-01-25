//-----------------------------------------------------------------------------//
// Helper - Spare time development for fun                                     //
// (c) 2023 Laurent Lardinois https://be.linkedin.com/in/laurentlardinois      //
//                                                                             //
// https://github.com/type-one/CSideTone                                       //
//                                                                             //
// This software is provided 'as-is', without any express or implied           //
// warranty.In no event will the authors be held liable for any damages        //
// arising from the use of this software.                                      //
//                                                                             //
// Permission is granted to anyone to use this software for any purpose,       //
// including commercial applications, and to alter itand redistribute it       //
// freely, subject to the following restrictions :                             //
//                                                                             //
// 1. The origin of this software must not be misrepresented; you must not     //
// claim that you wrote the original software.If you use this software         //
// in a product, an acknowledgment in the product documentation would be       //
// appreciated but is not required.                                            //
// 2. Altered source versions must be plainly marked as such, and must not be  //
// misrepresented as being the original software.                              //
// 3. This notice may not be removed or altered from any source distribution.  //
//-----------------------------------------------------------------------------//

#pragma once

#if !defined(__ATOMIC_HELPER_H__)
#define __ATOMIC_HELPER_H__

#if !defined(__STDC_NO_ATOMICS__)
#include <stdatomic.h>
#endif

#include <stdbool.h>
#include <string.h>

#if defined(_WIN32)
#include <intrin.h>
#include <windows.h>
#else
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#endif

#if defined(__cplusplus)
extern "C"
{
#endif

#if !defined(__STDC_NO_ATOMICS__)
#define _atomic_bool atomic_bool
#define _atomic_ulong atomic_ulong
#else
#define _atomic_bool volatile bool
#define _atomic_ulong volatile unsigned long
#endif

#if !defined(__STDC_NO_ATOMICS__)
#define sync_read_acquire()
#define sync_write_release()
#define sync_read_write()
#define sync_atomic_inc(ref) atomic_fetch_add(&(ref), 1)
#define sync_atomic_load(ref) atomic_load(&(ref))
#define sync_atomic_store(ref, val) atomic_store(&ref, val)
#elif defined(_WIN32)
#define sync_read_acquire() _ReadBarrier()
#define sync_write_release() _WriteBarrier()
#define sync_read_write() _ReadWriteBarrier()
#define sync_atomic_inc(ref) InterlockedIncrementAcquire(&(ref))
#define sync_atomic_load(ref) (ref)
#define sync_atomic_store(ref, val) (ref = val)
#else
// fallback: assuming GCC/Clang
#define sync_read_acquire() __sync_synchronize()
#define sync_write_release() __sync_synchronize()
#define sync_read_write() __sync_synchronize()
#define sync_atomic_inc(ref) __sync_fetch_and_add(&(ref), 1)
#define sync_atomic_load(ref) (ref)
#define sync_atomic_store(ref, val) (ref = val)
#endif

#if defined(__cplusplus)
};
#endif

#endif //  __ATOMIC_HELPER_H__
