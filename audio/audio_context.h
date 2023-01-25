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

#include <stdbool.h>
#include <stdint.h>

#if defined(__cplusplus)
extern "C"
{
#endif

    struct ma_context;

    // uncomment to display producer/consumer progress
    // #define DISPLAY_PROGRESS

    // some values tested on Win10/Corei7/AVX2
    // 16000, 20ms, 1
    // 44100, 16ms, 1
    // 44100, 20ms, 2
    // 48000, 50ms, 1
    // 22050, 20ms, 1 (in debug)
    // 22050, 12ms, 1 (in release)
    // 22050, 8ms, 1  (in release, SIMD/Most optimizations)
    // 22050, 14ms, 2
    // under VMware player 17 (Ubuntu 20 LTS) on Win10/Corei7/AVX2 host
    // 22050, 40ms, 1 (in debug)

    // parameters to tune
#if defined(_WIN32)
#define audio_period_ms 10 // more aggressive/lower latency
#else
#define audio_period_ms 40 // more conservative by default
#endif

#define audio_frame_sync true // sync producer/consumer
#define audio_frequency 22050
#define audio_frame_size ((audio_frequency * audio_period_ms) / 1000)
#define audio_channels 1      // mono (1), stereo (2)
    typedef int16_t sample_t; // S16_LE
    // typedef float sample_t;     // F32

#define is_of_type(x, type) _Generic(x, type : true, default : false)

    struct audio_context
    {
        struct ma_context* m_context;
    };

#if defined(AUDIO_CONTEXT_IMPLEM)
#define EXTERN_AUDIO_CONTEXT
#else
#define EXTERN_AUDIO_CONTEXT extern
#endif

    EXTERN_AUDIO_CONTEXT int init_audio_context(struct audio_context* context);
    EXTERN_AUDIO_CONTEXT int deinit_audio_context(struct audio_context* context);


#if defined(__cplusplus)
};
#endif

#endif //  __AUDIO_CONTEXT_H__
