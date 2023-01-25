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

#include "audio_context.h"
#include "miniaudio/miniaudio.h"
#include "tools/double_buffer.h"

#define AUDIO_PLAYBACK_IMPLEM
#include "audio_playback.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#include <windows.h>
#else
#include <unistd.h>
#endif

static void frame_playback(struct ma_device* device, void* output, const void* input, ma_uint32 frame_count)
{
    (void)input;

    sample_t* samples = (sample_t*)output;
    struct audio_playback* instance = (struct audio_playback*)(device->pUserData);
    if (instance)
    {
        audio_playback_on_playback(instance, samples, (size_t)frame_count);
    }
}

int init_audio_playback(struct audio_playback* audio_play, struct double_buffer* shared_buffer, struct audio_context* shared_context)
{
    audio_play->m_shared_context = shared_context;
    audio_play->m_shared_buffer = shared_buffer;
    audio_play->m_playback_device = (struct ma_device*)malloc(sizeof(ma_device));

    if (!audio_play->m_playback_device)
    {
        return -1;
    }

    ma_device_config config = ma_device_config_init(ma_device_type_playback);
    config.playback.pDeviceID = NULL;

    sample_t dummy;
    if (is_of_type(dummy, float))
    {
        config.playback.format = ma_format_f32;
    }
    else if (is_of_type(dummy, int16_t))
    {
        config.playback.format = ma_format_s16;
    }
    else if (is_of_type(dummy, int32_t))
    {
        config.playback.format = ma_format_s32;
    }
    else
    {
        config.playback.format = ma_format_unknown;
    }

    config.playback.channels = audio_channels;
    config.sampleRate = audio_frequency;
    config.periodSizeInMilliseconds = audio_period_ms;
    config.dataCallback = frame_playback;
    config.pUserData = audio_play;
    config.performanceProfile = ma_performance_profile_low_latency;

    if (MA_SUCCESS != ma_device_init(audio_play->m_shared_context->m_context, &config, audio_play->m_playback_device))
    {
        fprintf(stderr, "Failed to initialize playback device\n");
        free(audio_play->m_playback_device);
        audio_play->m_playback_device = NULL;
        return -1;
    }

    printf("Playback Device %s\n", audio_play->m_playback_device->playback.name);

    return 0;
}

int deinit_audio_playback(struct audio_playback* audio_play)
{
    ma_device_uninit(audio_play->m_playback_device);
    free(audio_play->m_playback_device);
    audio_play->m_playback_device = NULL;

    return 0;
}

void audio_playback_start(struct audio_playback* audio_play)
{
    if (MA_SUCCESS != ma_device_start(audio_play->m_playback_device))
    {
        fprintf(stderr, "Failed to start playback device\n");
        return;
    }
}

void audio_playback_stop(struct audio_playback* audio_play)
{
    (void)double_buffer_swap(audio_play->m_shared_buffer); // wakeup

    if (MA_SUCCESS != ma_device_stop(audio_play->m_playback_device))
    {
        fprintf(stderr, "Failed to stop playback device\n");
        return;
    }
}

void audio_playback_on_playback(struct audio_playback* audio_play, sample_t* output, const size_t frames)
{
    const unsigned long timeout = audio_period_ms * 1000;
    (void)double_buffer_sync_timed(audio_play->m_shared_buffer, timeout); // wait for an available frame

    const sample_t* input = (const sample_t*)double_buffer_get_front(audio_play->m_shared_buffer);

    const size_t nb_samples_in_buffer = double_buffer_get_frame_size(audio_play->m_shared_buffer) / (audio_channels * sizeof(sample_t));
    const size_t nb_frames = (frames < nb_samples_in_buffer) ? frames : nb_samples_in_buffer;

    // compute attenuated sidetone (-6dB)
    sample_t dummy;
    if (is_of_type(dummy, float))
    {
        for (size_t i = 0; i < nb_frames * audio_channels; ++i)
        {
            *output++ = 0.5f * *input++;
        }
    }
    else if (is_of_type(dummy, int16_t) || is_of_type(dummy, int32_t))
    {
        for (size_t i = 0; i < nb_frames * audio_channels; ++i)
        {
            *output++ = *input++ / 2;
        }
    }

#if defined(DISPLAY_PROGRESS)
    printf("-");
#endif
}
