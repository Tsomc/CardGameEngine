#include "audio_system.h"
#include "core_log.h"
#include "engine.h"
#include "resource_manager.h"
#include <SDL.h>
#include <SDL_mixer.h>

AudioSystem::AudioSystem()
    : masterVolume(1.0f)
    , effectVolume(1.0f)
    , musicVolume(1.0f)
    , allocatedChannels(16)
    , currentMusic(INVALID_RESOURCE_ID)
    , isMusicPaused(false)
{
}

AudioSystem::~AudioSystem()
{
}

bool AudioSystem::ModuleInit()
{
    LOG_INFO("Audio", "音频系统初始化");
    Mix_AllocateChannels(allocatedChannels);
    return true;
}

void AudioSystem::ModuleStart()
{
}

void AudioSystem::ModuleUpdate(float deltaTime)
{
    (void)deltaTime;
}

void AudioSystem::ModuleShutdown()
{
    AudioStopMusic(0);
    AudioStopAllEffects();
    LOG_INFO("Audio", "音频系统关闭");
}

const char* AudioSystem::ModuleGetName() const
{
    return "AudioSystem";
}

int AudioSystem::AudioPlayEffect(
    const std::string& cacheId, float volume, int channel)
{
    ResourceManager* res = Engine::EngineGet().EngineGetModule<ResourceManager>();
    if (res == nullptr) {
        return -1;
    }

    ResourceId id = res->ResourceFind(cacheId);
    if (id == INVALID_RESOURCE_ID) {
        LOG_WARN("Audio", "音效未加载: %s", cacheId.c_str());
        return -1;
    }

    Mix_Chunk* chunk = res->ResourceGet<Mix_Chunk>(id);
    if (chunk == nullptr) {
        return -1;
    }

    float finalVol = volume < 0.0f ? effectVolume : volume;
    finalVol = std::max(0.0f, std::min(1.0f, finalVol));
    finalVol *= masterVolume;

    Mix_VolumeChunk(chunk, (int)(finalVol * 128.0f));

    int ch = Mix_PlayChannel(channel, chunk, 0);
    if (ch < 0) {
        LOG_WARN("Audio", "音效播放失败: %s", cacheId.c_str());
    }

    return ch;
}

void AudioSystem::AudioPlayMusic(
    const std::string& cacheId, int loop, int fadeInMs)
{
    ResourceManager* res = Engine::EngineGet().EngineGetModule<ResourceManager>();
    if (res == nullptr) {
        return;
    }

    ResourceId id = res->ResourceFind(cacheId);
    if (id == INVALID_RESOURCE_ID) {
        LOG_WARN("Audio", "音乐未加载: %s", cacheId.c_str());
        return;
    }

    Mix_Music* music = res->ResourceGet<Mix_Music>(id);
    if (music == nullptr) {
        return;
    }

    AudioStopMusic(200);

    if (fadeInMs > 0) {
        int vol = (int)(musicVolume * masterVolume * 128.0f);
        Mix_VolumeMusic(vol);

        if (Mix_FadeInMusic(music, loop, fadeInMs) < 0) {
            LOG_WARN("Audio", "音乐淡入播放失败: %s (%s)",
                     cacheId.c_str(), Mix_GetError());
        }
    } else {
        int vol = (int)(musicVolume * masterVolume * 128.0f);
        Mix_VolumeMusic(vol);

        if (Mix_PlayMusic(music, loop) < 0) {
            LOG_WARN("Audio", "音乐播放失败: %s (%s)",
                     cacheId.c_str(), Mix_GetError());
        }
    }

    currentMusic = id;
    isMusicPaused = false;
}

void AudioSystem::AudioPauseMusic()
{
    Mix_PauseMusic();
    isMusicPaused = true;
}

void AudioSystem::AudioResumeMusic()
{
    Mix_ResumeMusic();
    isMusicPaused = false;
}

void AudioSystem::AudioStopMusic(int fadeOutMs)
{
    if (fadeOutMs > 0 && Mix_PlayingMusic()) {
        Mix_FadeOutMusic(fadeOutMs);
    } else {
        Mix_HaltMusic();
    }
    currentMusic = INVALID_RESOURCE_ID;
    isMusicPaused = false;
}

void AudioSystem::AudioSetMasterVolume(float volume)
{
    masterVolume = std::max(0.0f, std::min(1.0f, volume));
    Mix_VolumeMusic((int)(musicVolume * masterVolume * 128.0f));
}

void AudioSystem::AudioSetEffectVolume(float volume)
{
    effectVolume = std::max(0.0f, std::min(1.0f, volume));
}

void AudioSystem::AudioSetMusicVolume(float volume)
{
    musicVolume = std::max(0.0f, std::min(1.0f, volume));
    Mix_VolumeMusic((int)(musicVolume * masterVolume * 128.0f));
}

float AudioSystem::AudioGetMasterVolume() const
{
    return masterVolume;
}

float AudioSystem::AudioGetEffectVolume() const
{
    return effectVolume;
}

float AudioSystem::AudioGetMusicVolume() const
{
    return musicVolume;
}

void AudioSystem::AudioStopAllEffects()
{
    Mix_HaltChannel(-1);
}

void AudioSystem::AudioStopEffect(const std::string& cacheId)
{
    ResourceManager* res = Engine::EngineGet().EngineGetModule<ResourceManager>();
    if (res == nullptr) {
        return;
    }

    ResourceId id = res->ResourceFind(cacheId);
    if (id == INVALID_RESOURCE_ID) {
        return;
    }

    Mix_Chunk* chunk = res->ResourceGet<Mix_Chunk>(id);
    if (chunk == nullptr) {
        return;
    }

    for (int i = 0; i < allocatedChannels; ++i) {
        if (Mix_GetChunk(i) == chunk) {
            Mix_HaltChannel(i);
        }
    }
}

void AudioSystem::AudioStopChannel(int channel)
{
    Mix_HaltChannel(channel);
}

void AudioSystem::AudioPauseAllEffects()
{
    Mix_Pause(-1);
}

void AudioSystem::AudioResumeAllEffects()
{
    Mix_Resume(-1);
}
