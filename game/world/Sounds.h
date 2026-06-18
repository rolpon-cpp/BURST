//
// Created by  on 11/5/2025.
//

#ifndef BOUNCINGPLUS_SOUNDMANAGER_H
#define BOUNCINGPLUS_SOUNDMANAGER_H
#include <raylib.h>
#include <unordered_map>
#include <string>
#include <vector>

class GameClient;

#pragma pack(push, 1)
struct SoundEffect
{
    char SoundName[32];
    float SoundVolume = 1.0f;
    float SoundPitch = 1.0f;
    void set_properties(float distance);
};
#pragma pack(pop)

class Sounds {
    public:
    GameClient *game;
    std::unordered_map<std::string, Sound> SFX;
    std::unordered_map<std::string, Music> Musics;
    std::unordered_map<std::string, std::vector<Sound>> CachedAliases;
    std::vector<std::tuple<std::string,float,float>> MusicTransitions;
    int MaxSoundPoolSize;
    Sounds(GameClient *game);
    Sounds();
    ~Sounds();
    void PlayGameSound(std::string SoundName, float SoundVolume = 1.0f, float SoundPitch = 1.0f);
    void PlayGameSound(SoundEffect effect);
    void PlayGameMusic(std::string MusicName, bool Transition = false);
    void StopGameMusic(std::string MusicName, bool Transition = false);
    bool IsGameMusicPlaying(std::string MusicName);
    void ClearCache();
    void Clear();
    void Update();
    void Quit();
};


#endif //BOUNCINGPLUS_SOUNDMANAGER_H