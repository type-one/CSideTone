//-----------------------------------------------------------------------------//
// Audio Playback - Spare time development for fun                             //
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

#if !defined(__AUDIO_PLAYBACK_H__)
#define __AUDIO_PLAYBACK_H__

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

    struct audio_playback
    {
        struct audio_context* m_shared_context;
        struct double_buffer* m_shared_buffer;
        struct ma_device* m_playback_device;
    };


#if defined(AUDIO_PLAYBACK_IMPLEM)
#define EXTERN_AUDIO_PLAYBACK
#else
#define EXTERN_AUDIO_PLAYBACK extern
#endif

    EXTERN_AUDIO_PLAYBACK int init_audio_playback(
        struct audio_playback* audio_play, struct double_buffer* shared_buffer, struct audio_context* shared_context);
    EXTERN_AUDIO_PLAYBACK int deinit_audio_playback(struct audio_playback* audio_play);
    EXTERN_AUDIO_PLAYBACK void audio_playback_start(struct audio_playback* audio_play);
    EXTERN_AUDIO_PLAYBACK void audio_playback_stop(struct audio_playback* audio_play);
    EXTERN_AUDIO_PLAYBACK void audio_playback_on_playback(struct audio_playback* audio_play, sample_t* output, const size_t frames);

#if defined(__cplusplus)
};
#endif

#endif //  __AUDIO_PLAYBACK_H__
