#ifndef AUDIO_SYSTEM_H
#define AUDIO_SYSTEM_H

#include "module_manager.h"
#include "resource_handle.h"
#include <string>

class AudioSystem : public IModule {
public:
    AudioSystem();
    ~AudioSystem() override;

    bool ModuleInit() override;
    void ModuleStart() override;
    void ModuleUpdate(float deltaTime) override;
    void ModuleShutdown() override;
    const char* ModuleGetName() const override;

    int AudioPlayEffect(
        const std::string& cacheId, float volume = -1.0f, int channel = -1);
    void AudioPlayMusic(
        const std::string& cacheId, int loop = -1, int fadeInMs = 500);
    void AudioPauseMusic();
    void AudioResumeMusic();
    void AudioStopMusic(int fadeOutMs = 500);

    void AudioSetMasterVolume(float volume);
    void AudioSetEffectVolume(float volume);
    void AudioSetMusicVolume(float volume);
    float AudioGetMasterVolume() const;
    float AudioGetEffectVolume() const;
    float AudioGetMusicVolume() const;

    void AudioStopAllEffects();
    void AudioStopEffect(const std::string& cacheId);
    void AudioStopChannel(int channel);
    void AudioPauseAllEffects();
    void AudioResumeAllEffects();

private:
    float masterVolume;
    float effectVolume;
    float musicVolume;
    int allocatedChannels;
    ResourceId currentMusic;
    bool isMusicPaused;
};

#endif
