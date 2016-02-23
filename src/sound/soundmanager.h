#pragma once

#include <AL/al.h>
#include <AL/alc.h>

namespace Sound
{
    class SoundManager
    {
    public:
        SoundManager();
        ~SoundManager();

    private:
        ALCdevice *m_pDevice;
        ALboolean m_Enumeration;
        ALCcontext *m_pContext;
    };
}
