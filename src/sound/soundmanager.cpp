#if 0
#include <alut.h>

#include "utils/logger.h"
#include "soundmanager.h"

Sound::SoundManager::SoundManager() :
    m_pDevice(alcOpenDevice(nullptr))
{
    if(!m_pDevice)
        LogError() << "Could not open audio device";

    //m_Enumeration = alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT");
    ALCenum error;

    error = alGetError();
    if (error != AL_NO_ERROR)
        LogError() << error;
    m_pContext = alcCreateContext(m_pDevice, NULL);
    if (!alcMakeContextCurrent(m_pContext))
        LogError() << "Could not make current context";

    ALfloat listenerOri[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };

    alListener3f(AL_POSITION, 0, 0, 1.0f);
    error = alGetError();
    if (error != AL_NO_ERROR)
        LogError() << error;
    alListener3f(AL_VELOCITY, 0, 0, 0);
    error = alGetError();
    if (error != AL_NO_ERROR)
        LogError() << error;
    alListenerfv(AL_ORIENTATION, listenerOri);
    error = alGetError();
    if (error != AL_NO_ERROR)
        LogError() << error;

    ALuint source;

    alGenSources((ALuint)1, &source);
    error = alGetError();
    if (error != AL_NO_ERROR)
        LogError() << error;

    alSourcef(source, AL_PITCH, 1);
    error = alGetError();
    if (error != AL_NO_ERROR)
        LogError() << error;
    alSourcef(source, AL_GAIN, 1);
    error = alGetError();
    if (error != AL_NO_ERROR)
        LogError() << error;
    alSource3f(source, AL_POSITION, 0, 0, 0);
    error = alGetError();
    if (error != AL_NO_ERROR)
        LogError() << error;
    alSource3f(source, AL_VELOCITY, 0, 0, 0);
    error = alGetError();
    if (error != AL_NO_ERROR)
        LogError() << error;
    alSourcei(source, AL_LOOPING, AL_FALSE);
    error = alGetError();
    if (error != AL_NO_ERROR)
        LogError() << error;

    ALuint buffer;

    alGenBuffers((ALuint)1, &buffer);
    error = alGetError();
    if (error != AL_NO_ERROR)
        LogError() << error;

    ALsizei size, freq;
    ALenum format;
    ALvoid *data;
    ALboolean loop = AL_FALSE;

    alutLoadWAVFile(reinterpret_cast<ALbyte *>(const_cast<char *>("test.wav")), &format, &data, &size, &freq, &loop);
    error = alGetError();
    if (error != AL_NO_ERROR)
        LogError() << error;
    alBufferData(buffer, format, data, size, freq);
    alSourcei(source, AL_BUFFER, buffer);alSourcePlay(source);

    alSourcePlay(source);
    error = alGetError();
    if (error != AL_NO_ERROR)
        LogError() << error;

    ALint source_state;
    alGetSourcei(source, AL_SOURCE_STATE, &source_state);
    LogInfo() << "Play sound...";
    while (source_state == AL_PLAYING)
    {
            alGetSourcei(source, AL_SOURCE_STATE, &source_state);
            error = alGetError();
            if (error != AL_NO_ERROR)
                LogError() << error;
            LogInfo() << "Play sound...";
    }
    // cleanup context
    alDeleteSources(1, &source);
    alDeleteBuffers(1, &buffer);
    m_pDevice = alcGetContextsDevice(m_pContext);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(m_pContext);
}

Sound::SoundManager::~SoundManager()
{
    if(m_pDevice)
        alcCloseDevice(m_pDevice);
}
#endif