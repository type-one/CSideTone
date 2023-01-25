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

#include "audio_context.h"
#include "miniaudio/miniaudio.h"
#include "tools/double_buffer.h"

#define AUDIO_CAPTURE_IMPLEM
#include "audio_capture.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#include <windows.h>
#else
#include <pthread.h>
#include <unistd.h>
#endif

static void frame_capture(struct ma_device* device, void* output, const void* input, ma_uint32 frame_count)
{
    (void)output;

    const sample_t* samples = (const sample_t*)input;
    struct audio_capture* instance = (struct audio_capture*)(device->pUserData);
    if (instance)
    {
        audio_capture_on_capture(instance, samples, (size_t)(frame_count));
    }
}

int init_audio_capture(struct audio_capture* audio_cap, struct double_buffer* shared_buffer, struct audio_context* shared_context)
{
    audio_cap->m_shared_context = shared_context;
    audio_cap->m_shared_buffer = shared_buffer;
    audio_cap->m_capture_device = (struct ma_device*)malloc(sizeof(struct ma_device));

    if (!audio_cap->m_capture_device)
    {
        return -1;
    }

    struct ma_device_config config = ma_device_config_init(ma_device_type_capture);
    config.capture.pDeviceID = NULL;

    sample_t dummy;
    if (is_of_type(dummy, float))
    {
        config.capture.format = ma_format_f32;
    }
    else if (is_of_type(dummy, int16_t))
    {
        config.capture.format = ma_format_s16;
    }
    else if (is_of_type(dummy, int32_t))
    {
        config.capture.format = ma_format_s32;
    }
    else
    {
        config.capture.format = ma_format_unknown;
    }

    config.capture.channels = audio_channels;
    config.sampleRate = audio_frequency;
    config.periodSizeInMilliseconds = audio_period_ms;
    config.dataCallback = frame_capture;
    config.pUserData = audio_cap;
    config.performanceProfile = ma_performance_profile_low_latency;

    if (MA_SUCCESS != ma_device_init(audio_cap->m_shared_context->m_context, &config, audio_cap->m_capture_device))
    {
        fprintf(stderr, "Failed to initialize capture device\n");
        free(audio_cap->m_capture_device);
        audio_cap->m_capture_device = NULL;
        return -1;
    }

    printf("Capture Device %s\n", audio_cap->m_capture_device->capture.name);

    return 0;
}

int deinit_audio_capture(struct audio_capture* audio_cap)
{
    ma_device_uninit(audio_cap->m_capture_device);
    free(audio_cap->m_capture_device);
    audio_cap->m_capture_device = NULL;

    return 0;
}

void audio_capture_start(struct audio_capture* audio_cap)
{
    if (MA_SUCCESS != ma_device_start(audio_cap->m_capture_device))
    {
        fprintf(stderr, "Failed to start capture device\n");
        return;
    }
}

void audio_capture_stop(struct audio_capture* audio_cap)
{
    if (MA_SUCCESS != ma_device_stop(audio_cap->m_capture_device))
    {
        fprintf(stderr, "Failed to stop capture device\n");
        return;
    }
}

void audio_capture_on_capture(struct audio_capture* audio_cap, const sample_t* input, const size_t frames)
{
    sample_t* output = (sample_t*)double_buffer_get_back(audio_cap->m_shared_buffer);

    const size_t nb_samples_in_buffer = double_buffer_get_frame_size(audio_cap->m_shared_buffer) / (audio_channels * sizeof(sample_t));
    const size_t nb_frames = (frames < nb_samples_in_buffer) ? frames : nb_samples_in_buffer;

    memcpy(output, input, nb_frames * audio_channels * sizeof(sample_t));

    (void)double_buffer_swap(audio_cap->m_shared_buffer);

#if defined(DISPLAY_PROGRESS)
    printf(".");
#endif
}
