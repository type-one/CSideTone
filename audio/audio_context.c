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

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio/miniaudio.h"

#define AUDIO_CONTEXT_IMPLEM
#include "audio_context.h"

#include <stdio.h>
#include <stdlib.h>

int init_audio_context(struct audio_context* context)
{
    context->m_context = (struct ma_context*)malloc(sizeof(struct ma_context));
    if (!context->m_context)
    {
        return -1;
    }
    ma_context_config context_config = ma_context_config_init();

    if (MA_SUCCESS != ma_context_init(NULL, 0, &context_config, context->m_context))
    {
        fprintf(stderr, "Failed to initialize context\n");
        free(context->m_context);
        context->m_context = NULL;
        return -1;
    }

    printf("MAL context initialized, backend is %s\n", ma_get_backend_name(context->m_context->backend));
    return 0;
}

int deinit_audio_context(struct audio_context* context)
{
    if (MA_SUCCESS != ma_context_uninit(context->m_context))
    {
        return -1;
    }
    return 0;
}
