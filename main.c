//-----------------------------------------------------------------------------//
// CSide Tone - Spare time development for fun                                 //
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

#include "audio/audio_capture.h"
#include "audio/audio_context.h"
#include "audio/audio_playback.h"
#include "tools/double_buffer.h"

#include <stdio.h>
#include <stdlib.h>
#if defined(_WIN32)
#include <windows.h>
#else
#include <pthread.h>
#endif

#if defined(_WIN32)
static DWORD WINAPI play_thread(LPVOID arg)
#else
static void* play_thread(void* arg)
#endif
{
    struct audio_playback* playback = (struct audio_playback*)arg;

    audio_playback_start(playback);

#if defined(_WIN32)
    return 0;
#else
    return NULL;
#endif
}

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    struct double_buffer buffer;
    init_double_buffer(&buffer, audio_frame_size * audio_channels * sizeof(sample_t), audio_frame_sync);

    struct audio_context context;
    init_audio_context(&context);

    struct audio_capture capture;
    init_audio_capture(&capture, &buffer, &context);

    struct audio_playback playback;
    init_audio_playback(&playback, &buffer, &context);

#if defined(_WIN32)
    DWORD playback_tid;
    HANDLE playback_hnd = CreateThread(0, 4096, play_thread, &playback, 0, &playback_tid);
#else
    pthread_t playback_tid;
    if (0 != pthread_create(&playback_tid, NULL, play_thread, &playback))
    {
        goto error_stop;
    }
#endif

    audio_capture_start(&capture);

    (void)getchar();

    audio_playback_stop(&playback);
    audio_capture_stop(&capture);

#if defined(_WIN32)
    if (playback_hnd)
    {
        WaitForSingleObject(playback_hnd, INFINITE);
        CloseHandle(playback_hnd);
    }
#else
    void* ret;
    pthread_join(playback_tid, &ret);
#endif

    deinit_audio_playback(&playback);
    deinit_audio_capture(&capture);
    deinit_audio_context(&context);
    deinit_double_buffer(&buffer);

    return 0;

error_stop:
    audio_playback_stop(&playback);
    audio_capture_stop(&capture);

    deinit_audio_playback(&playback);
    deinit_audio_capture(&capture);
    deinit_audio_context(&context);
    deinit_double_buffer(&buffer);

    return -1;
}
