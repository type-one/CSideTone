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

#if !defined(__SYNC_OBJECT_H__)
#define __SYNC_OBJECT_H__

#include "atomic_helper.h"

#include <stdbool.h>

#if defined(_WIN32)
#include <windows.h>
#else
#include <pthread.h>
#include <unistd.h>
#endif

#if defined(__cplusplus)
extern "C"
{
#endif

    /**
     * @brief Cross-platform signaling primitive for one-shot frame notifications.
     */
    struct sync_object
    {
    /** @brief Requests waiting threads to stop waiting during teardown. */
        bool m_stop;
    /** @brief Current signaled state. */
        bool m_signaled;

#if defined(_WIN32)
    /** @brief Windows mutex protecting internal state. */
        CRITICAL_SECTION m_mutex;
    /** @brief Windows condition variable for wait/signal operations. */
        CONDITION_VARIABLE m_cond;
#else
    /** @brief POSIX mutex protecting internal state. */
        pthread_mutex_t m_mutex;
    /** @brief POSIX condition variable for wait/signal operations. */
        pthread_cond_t m_cond;
    /** @brief POSIX condition variable attributes (monotonic clock). */
        pthread_condattr_t m_cond_attr;
#endif
    };

#if defined(SYNC_OBJECT_IMPLEM)
#define EXTERN_SYNC_OBJECT
#else
#define EXTERN_SYNC_OBJECT extern
#endif

    /**
     * @brief Initialize a synchronization object.
     * @param sync Synchronization object to initialize.
     * @param initial_state Initial signaled state.
     * @return `0` on success, `-1` on failure.
     */
    EXTERN_SYNC_OBJECT int init_sync_object(struct sync_object* sync, bool initial_state);

    /**
     * @brief Deinitialize a synchronization object and wake waiting threads.
     * @param sync Synchronization object to deinitialize.
     * @return `0` on success, `-1` on failure.
     */
    EXTERN_SYNC_OBJECT int deinit_sync_object(struct sync_object* sync);

    /**
     * @brief Signal one waiting thread.
     * @param sync Synchronization object.
     * @return `0` on success, `-1` on failure.
     */
    EXTERN_SYNC_OBJECT int sync_object_signal(struct sync_object* sync);

    /**
     * @brief Wait indefinitely for a signal.
     * @param sync Synchronization object.
     * @return `0` on success, `-1` on failure.
     */
    EXTERN_SYNC_OBJECT int sync_object_wait_for_signal(struct sync_object* sync);

    /**
     * @brief Wait for a signal up to a timeout.
     * @param sync Synchronization object.
     * @param timeout_us Timeout in microseconds.
     * @return `0` on success, `-1` on failure.
     */
    EXTERN_SYNC_OBJECT int sync_object_wait_for_signal_timed(struct sync_object* sync, unsigned long timeout_us);

#if defined(__cplusplus)
};
#endif

#endif //  __SYNC_OBJECT_H__
