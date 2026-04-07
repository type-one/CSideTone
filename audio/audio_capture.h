//-----------------------------------------------------------------------------//
// Audio Capture - Spare time development for fun                              //
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

#if !defined(__AUDIO_CAPTURE_H__)
#define __AUDIO_CAPTURE_H__

#include "audio/audio_context.h"

#include <stddef.h>
#include <stdint.h>

#if defined(__cplusplus)
extern "C"
{
#endif

    struct ma_context;
    struct ma_device;

    struct double_buffer;

    /**
     * @brief Capture pipeline state and shared resources.
     */
    struct audio_capture
    {
        /** @brief Shared audio backend context. */
        struct audio_context* m_shared_context;
        /** @brief Shared producer/consumer audio frame buffer. */
        struct double_buffer* m_shared_buffer;
        /** @brief Miniaudio capture device handle. */
        struct ma_device* m_capture_device;
    };

#if defined(AUDIO_CAPTURE_IMPLEM)
#define EXTERN_AUDIO_CAPTURE
#else
#define EXTERN_AUDIO_CAPTURE extern
#endif

    /**
     * @brief Initialize the capture device.
     * @param audio_cap Capture object to initialize.
     * @param shared_buffer Shared double buffer used for captured frames.
     * @param shared_context Shared audio backend context.
     * @return `0` on success, `-1` on failure.
     */
    EXTERN_AUDIO_CAPTURE int init_audio_capture(
        struct audio_capture* audio_cap, struct double_buffer* shared_buffer, struct audio_context* shared_context);

    /**
     * @brief Deinitialize the capture device and release resources.
     * @param audio_cap Capture object to deinitialize.
     * @return `0` on success, `-1` on failure.
     */
    EXTERN_AUDIO_CAPTURE int deinit_audio_capture(struct audio_capture* audio_cap);

    /**
     * @brief Start audio capture streaming.
     * @param audio_cap Capture object to start.
     * @return void.
     */
    EXTERN_AUDIO_CAPTURE void audio_capture_start(struct audio_capture* audio_cap);

    /**
     * @brief Stop audio capture streaming.
     * @param audio_cap Capture object to stop.
     * @return void.
     */
    EXTERN_AUDIO_CAPTURE void audio_capture_stop(struct audio_capture* audio_cap);

    /**
     * @brief Handle a newly captured frame block.
     * @param audio_cap Capture object receiving data.
     * @param input Input sample buffer provided by the backend.
     * @param frames Number of frames available in @p input.
     * @return void.
     */
    EXTERN_AUDIO_CAPTURE void audio_capture_on_capture(struct audio_capture* audio_cap, const sample_t* input, const size_t frames);

#if defined(__cplusplus)
};
#endif

#endif //  __AUDIO_CAPTURE_H__
