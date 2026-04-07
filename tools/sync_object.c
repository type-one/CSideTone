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

#include "atomic_helper.h"

#define SYNC_OBJECT_IMPLEM
#include "sync_object.h"

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#if defined(_WIN32)
#include <windows.h>
#include <intrin.h>
#else
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#endif


int init_sync_object(struct sync_object* sync, bool initial_state)
{ 
    if (!sync)
    {
        return -1;
    }

    memset(sync, 0, sizeof(struct sync_object));

    sync->m_signaled = initial_state;
    sync->m_stop = false;

#if defined(_WIN32)

    InitializeCriticalSection(&(sync->m_mutex));
    InitializeConditionVariable(&(sync->m_cond));

    return 0;
#else 
    if (0 != pthread_mutex_init(&(sync->m_mutex), NULL))
    {
        return -1;
    }

    if (0 != pthread_condattr_init(&(sync->m_cond_attr)))
    {
        goto destroy_mutex;
    }

    if (0 != pthread_condattr_setclock(&(sync->m_cond_attr), CLOCK_MONOTONIC))
    {
        goto destroy_cond_attr;
    }

    if (0 != pthread_cond_init(&(sync->m_cond), &(sync->m_cond_attr)))
    {
        goto destroy_cond_attr;
    }

    (void)pthread_condattr_destroy(&(sync->m_cond_attr));

    return 0;

destroy_cond_attr:
    (void)pthread_condattr_destroy(&(sync->m_cond_attr));

destroy_mutex:
    (void)pthread_mutex_destroy(&(sync->m_mutex));

    return -1;

#endif
}

int deinit_sync_object(struct sync_object* sync)
{
    if (!sync)
    {
        return -1;
    }

#if defined(_WIN32)
    EnterCriticalSection(&(sync->m_mutex));
#else
    if (0 != pthread_mutex_lock(&(sync->m_mutex)))
    {
        return -1;
    }
#endif

    sync->m_signaled = true;
    sync->m_stop = true;

#if defined(_WIN32)
    LeaveCriticalSection(&(sync->m_mutex));
    WakeAllConditionVariable(&(sync->m_cond));
#else
    (void)pthread_mutex_unlock(&(sync->m_mutex));
    (void)pthread_cond_broadcast(&(sync->m_cond));
#endif

#if defined(_WIN32)
    DeleteCriticalSection(&(sync->m_mutex));
#else
    if (0 != pthread_cond_destroy(&(sync->m_cond)))
    {
        return -1;
    }
    if (0 != pthread_mutex_destroy(&(sync->m_mutex)))
    {
        return -1;
    }
#endif

    return 0;
}

int sync_object_signal(struct sync_object* sync)
{ 
    if (!sync)
    {
        return -1;
    }

#if defined(_WIN32)
    EnterCriticalSection(&(sync->m_mutex));
#else
    if (0 != pthread_mutex_lock(&(sync->m_mutex)))
    {
        return -1;
    }
#endif

    sync->m_signaled = true;

#if defined(_WIN32)
    LeaveCriticalSection(&(sync->m_mutex));
#else
    (void)pthread_mutex_unlock(&(sync->m_mutex));
#endif

#if defined(_WIN32)
    WakeConditionVariable(&(sync->m_cond));
#else
    if (0 != pthread_cond_signal(&(sync->m_cond)))
    {
        return -1;
    }
#endif

    return 0;
}

int sync_object_wait_for_signal(struct sync_object* sync)
{ 
    if (!sync)
    {
        return -1;
    }

#if defined(_WIN32)
    int wait_result = 0;
    EnterCriticalSection(&(sync->m_mutex));
    while (!sync->m_signaled) /* loop to detect spurious wakes */
    {
        if (!SleepConditionVariableCS(&(sync->m_cond), &(sync->m_mutex), INFINITE))
        {
            wait_result = -1;
            break; // don't loop
        }
    }
    sync->m_signaled = sync->m_stop;
    LeaveCriticalSection(&(sync->m_mutex));
    if (0 != wait_result)
    {
        return -1;
    }
#else
    if (0 != pthread_mutex_lock(&(sync->m_mutex)))
    {
        return -1;
    }
    int wait_result = 0;
    while (!sync->m_signaled) /* loop to detect spurious wakes */
    {
        if (0 != pthread_cond_wait(&(sync->m_cond), &(sync->m_mutex)))
        {
            wait_result = -1;
            break; // exit loop in case of error
        }
    }
    sync->m_signaled = sync->m_stop;
    (void)pthread_mutex_unlock(&(sync->m_mutex));
    if (0 != wait_result)
    {
        return -1;
    }
#endif

    return 0;
}

int sync_object_wait_for_signal_timed(struct sync_object* sync, unsigned long timeout_us)
{
    if (!sync)
    {
        return -1;
    }

#if defined(_WIN32)
    const unsigned long timeout_ms = timeout_us / 1000;
    int timed_wait_result = 0;
    EnterCriticalSection(&(sync->m_mutex));
    while (!sync->m_signaled) /* loop to detect spurious wakes */
    {
        if (!SleepConditionVariableCS(&(sync->m_cond), &(sync->m_mutex), timeout_ms))
        {
            timed_wait_result = -1;
            break; // timeout
        }
    }
    sync->m_signaled = sync->m_stop;
    LeaveCriticalSection(&(sync->m_mutex));
    if (0 != timed_wait_result)
    {
        return -1;
    }
#else
    struct timespec timeout;
    if (0 != clock_gettime(CLOCK_MONOTONIC, &timeout))
    {
        return -1;
    }
    timeout.tv_sec += (time_t)(timeout_us / 1000000UL);
    timeout.tv_nsec += (long)((timeout_us % 1000000UL) * 1000UL);
    if (timeout.tv_nsec >= 1000000000L)
    {
        timeout.tv_sec += 1;
        timeout.tv_nsec -= 1000000000L;
    }

    if (0 != pthread_mutex_lock(&(sync->m_mutex)))
    {
        return -1;
    }
    int timed_wait_result = 0;
    while (!sync->m_signaled) /* loop to detect spurious wakes */
    {
        const int wait_res = pthread_cond_timedwait(&(sync->m_cond), &(sync->m_mutex), &timeout);
        if (0 != wait_res)
        {
            if (ETIMEDOUT == wait_res)
            {
                sync->m_signaled = false;
                (void)pthread_mutex_unlock(&(sync->m_mutex));
                return -1;
            }
            timed_wait_result = -1;
            break; // other error
        }
    }
    sync->m_signaled = sync->m_stop;
    (void)pthread_mutex_unlock(&(sync->m_mutex));
    if (0 != timed_wait_result)
    {
        return -1;
    }
#endif

    return 0;
}
