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

#if !defined(__DOUBLE_BUFFER_H__)
#define __DOUBLE_BUFFER_H__

#include "atomic_helper.h"

#include <stddef.h>
#include <stdint.h>

#if defined(_WIN32)
#include <windows.h>
#else
#include <pthread.h>
#include <unistd.h>
#endif

#include "sync_object.h"

#if defined(__cplusplus)
extern "C"
{
#endif

    struct double_buffer
    {
        size_t m_frame_size;
        uint8_t* m_internal_buffer;
        uint8_t* m_buffers[2];

        _atomic_ulong m_index;

        bool m_sync_enabled;
        struct sync_object m_frame_sync;
    };

#if defined(DOUBLE_BUFFER_IMPLEM)
#define EXTERN_DOUBLE_BUFFER
#else
#define EXTERN_DOUBLE_BUFFER extern
#endif

    EXTERN_DOUBLE_BUFFER int init_double_buffer(struct double_buffer* buf, size_t frame_size, bool sync);
    EXTERN_DOUBLE_BUFFER int deinit_double_buffer(struct double_buffer* buf);

    EXTERN_DOUBLE_BUFFER uint8_t* double_buffer_get_front(struct double_buffer* buf);
    EXTERN_DOUBLE_BUFFER uint8_t* double_buffer_get_back(struct double_buffer* buf);
    EXTERN_DOUBLE_BUFFER int double_buffer_swap(struct double_buffer* buf);
    EXTERN_DOUBLE_BUFFER int double_buffer_sync(struct double_buffer* buf);
    EXTERN_DOUBLE_BUFFER int double_buffer_sync_timed(struct double_buffer* buf, unsigned long timeout_us);
    EXTERN_DOUBLE_BUFFER size_t double_buffer_get_frame_size(struct double_buffer* buf);

#if defined(__cplusplus)
};
#endif

#endif //  __DOUBLE_BUFFER_H__
