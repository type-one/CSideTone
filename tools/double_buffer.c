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

#define DOUBLE_BUFFER_IMPLEM
#include "double_buffer.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#include <intrin.h>
#include <windows.h>
#else
#include <pthread.h>
#include <unistd.h>
#endif

int init_double_buffer(struct double_buffer* buf, size_t frame_size, bool sync)
{
    if (!buf)
    {
        return -1;
    }

    memset(buf, 0, sizeof(struct double_buffer));

    buf->m_frame_size = frame_size;
    buf->m_sync_enabled = sync;

    buf->m_internal_buffer = (uint8_t*)calloc(frame_size * 2, sizeof(uint8_t));
    if (!buf->m_internal_buffer)
    {
        return -1;
    }

    buf->m_buffers[0] = buf->m_internal_buffer;
    buf->m_buffers[1] = buf->m_buffers[0] + frame_size;

    if (init_sync_object(&(buf->m_frame_sync), true) < 0)
    {
        goto error_free_buf;
    }

    return 0;

error_free_buf:
    free(buf->m_internal_buffer);
    memset(buf, 0, sizeof(struct double_buffer));
    return -1;
}

int deinit_double_buffer(struct double_buffer* buf)
{
    if (!buf)
    {
        return -1;
    }

    (void)deinit_sync_object(&(buf->m_frame_sync));

    free(buf->m_internal_buffer);
    memset(buf, 0, sizeof(struct double_buffer));

    return 0;
}

uint8_t* double_buffer_get_front(struct double_buffer* buf)
{
    if (!buf)
    {
        return NULL;
    }

    sync_read_acquire();
    unsigned long index = sync_atomic_load(buf->m_index);
    return buf->m_buffers[index & 1];
}

uint8_t* double_buffer_get_back(struct double_buffer* buf)
{
    if (!buf)
    {
        return NULL;
    }

    sync_read_acquire();
    unsigned long index = sync_atomic_load(buf->m_index);
    return buf->m_buffers[(index + 1) & 1];
}

int double_buffer_swap(struct double_buffer* buf)
{
    if (!buf)
    {
        return -1;
    }

    sync_atomic_inc(buf->m_index);
    sync_write_release();

    if (buf->m_sync_enabled)
    {
        if (sync_object_signal(&(buf->m_frame_sync)) < 0)
        {
            return -1;
        }
    }

    return 0;
}

int double_buffer_sync(struct double_buffer* buf)
{
    if (!buf)
    {
        return -1;
    }

    if (buf->m_sync_enabled)
    {
        if (sync_object_wait_for_signal(&(buf->m_frame_sync)) < 0)
        {
            return -1;
        }
    }

    return 0;
}

int double_buffer_sync_timed(struct double_buffer* buf, unsigned long timeout_us)
{
    if (!buf)
    {
        return -1;
    }

    if (buf->m_sync_enabled)
    {
        if (sync_object_wait_for_signal_timed(&(buf->m_frame_sync), timeout_us) < 0)
        {
            return -1;
        }
    }

    return 0;
}

size_t double_buffer_get_frame_size(struct double_buffer* buf)
{
    if (!buf)
    {
        return 0;
    }

    return buf->m_frame_size;
}
