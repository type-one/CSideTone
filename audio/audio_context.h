//-----------------------------------------------------------------------------//
// Audio Context - Spare time development for fun                              //
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

#if !defined(__AUDIO_CONTEXT_H__)
#define __AUDIO_CONTEXT_H__

#include "audio_config.h"

#include <stdbool.h>
#include <stdint.h>

#if defined(__cplusplus)
extern "C"
{
#endif

    struct ma_context;

/**
 * @brief Compile-time type check helper.
 *
 * Evaluates to `true` when `x` has the exact `type`, otherwise `false`.
 */
#define is_of_type(x, type) _Generic(x, type : true, default : false)

    /**
     * @brief Shared audio backend context wrapper.
     */
    struct audio_context
    {
        /** @brief Pointer to the miniaudio context instance. */
        struct ma_context* m_context;
    };

#if defined(AUDIO_CONTEXT_IMPLEM)
#define EXTERN_AUDIO_CONTEXT
#else
#define EXTERN_AUDIO_CONTEXT extern
#endif

    /**
     * @brief Initialize the shared audio context.
     * @param context Context object to initialize.
     * @return `0` on success, `-1` on failure.
     */
    EXTERN_AUDIO_CONTEXT int init_audio_context(struct audio_context* context);

    /**
     * @brief Deinitialize the shared audio context.
     * @param context Context object to deinitialize.
     * @return `0` on success, `-1` on failure.
     */
    EXTERN_AUDIO_CONTEXT int deinit_audio_context(struct audio_context* context);


#if defined(__cplusplus)
};
#endif

#endif //  __AUDIO_CONTEXT_H__
