//
// Created by lalit on 11/5/2025.
//

#include "Sounds.h"
#include "Game.h"
#include <filesystem>
#include <iostream>
#include <raymath.h>

void SoundEffect::set_properties(float distance)
{
    float DistanceMultiplier = (1000.0f - distance) / 1000.0f;
    DistanceMultiplier += GetRandomValue(-20, 20) / 100.0f;

    SoundVolume *= DistanceMultiplier,
        1.0f + GetRandomValue(-20, 20) / 100.0f;
}

Sounds::Sounds(GameClient *game) {
    this->game = game;

    MaxSoundPoolSize = 10;

    std::string path = "assets/";
    for (const auto & entry : std::filesystem::directory_iterator(path)) {
        std::string p = entry.path().filename().string();
        if (p.ends_with(".mp3") || p.ends_with(".wav"))
        {
            Sound sound = LoadSound(entry.path().string().c_str());
            string d = GetFileNameWithoutExt(("assets/"+p).c_str());
            SFX.insert({d, sound});
            std::vector<Sound> s = std::vector<Sound>();
            CachedAliases[d] = s;
        } else if (p.ends_with("music.mp3") || p.ends_with("music.wav"))
        {
            Music m = LoadMusicStream(entry.path().string().c_str());
            Musics.insert({p, m});
        }
    }
}

Sounds::Sounds() {
}

Sounds::~Sounds() {
}

void Sounds::Clear() {
    for (auto& [name,value] : CachedAliases) {
        for (Sound& sound : value) {
            StopSound(sound);
        }
    }
    for (auto& [name,value] : SFX) {
        StopSound(value);
    }
    for (auto& [name,value] : Musics) {
        StopMusicStream(value);
    }
    MusicTransitions.clear();
}

void Sounds::Update() {

    std::erase_if(MusicTransitions, [&](tuple<string, float, float>& value) {
        std::string& s = std::get<0>(value);
        float& f1 = std::get<1>(value);
        float& f2 = std::get<2>(value);
        if (!Musics.count(s))
            return true;
        if (abs(f1 - f2) <= 0.05f)
        {
            if (f2 <= 0.05f)
                StopMusicStream(Musics[s]);
            f2 = f1;
            return true;
        }
        return false;
        });

    for (tuple<string, float, float>& value : MusicTransitions)
    {
        std::string& s = std::get<0>(value);
        float& f1 = std::get<1>(value);
        float& f2 = std::get<2>(value);

        f2 = Lerp(f2, f1, 6.5f * GetFrameTime());
        SetMusicVolume(Musics[s],f2);
    }

    for (auto& [name,val]: Musics)
    {
        if (IsMusicStreamPlaying(val))
            UpdateMusicStream(val);
    }

    int i = 0;
    for (auto& [name,value] : CachedAliases) {
        std::erase_if(value, [&](Sound& sound) {
            bool SoundCondition = false;
            SoundCondition = ((IsSoundValid(sound) && !IsSoundPlaying(sound)) || !IsSoundValid(sound));

            if (SoundCondition) {
                if (IsSoundValid(sound))
                    UnloadSoundAlias(sound);
            }
            return SoundCondition;
        });
    }

    for (auto &[name, val] : CachedAliases)
    {
        for (Sound& CachedSound : val)
        {
            if (IsSoundValid(CachedSound) && IsSoundPlaying(CachedSound))
            {
                ResumeSound(CachedSound);
            }
        }
    }
}

void Sounds::ClearCache()
{
    for (auto& [name,value] : CachedAliases)
    {
        for (Sound& sound : value)
        {
            if (IsSoundValid(sound))
                UnloadSoundAlias(sound);
        }
        value.clear();
    }
}

bool Sounds::IsGameMusicPlaying(std::string MusicName)
{
    if (!Musics.count(MusicName))
        return false;
    return IsMusicStreamPlaying(Musics[MusicName]);
}

void Sounds::PlayGameMusic(std::string MusicName, bool Transition)
{
    if (GetMasterVolume() <= 0.0f)
        return;
    if (!Musics.count(MusicName))
        return;
    PlayMusicStream(Musics[MusicName]);
    if (!Transition)
        SetMusicVolume(Musics[MusicName], 1.0f);
    else
    {
        MusicTransitions.push_back(make_tuple(MusicName, 1.0f, 0.0f));
        SetMusicVolume(Musics[MusicName], 0);
    }
}

void Sounds::StopGameMusic(std::string MusicName, bool Transition)
{
    if (!Musics.count(MusicName))
        return;
    if (!Transition)
        StopMusicStream(Musics[MusicName]);
    else
    {
        MusicTransitions.push_back(make_tuple(MusicName, 0.0f, 1.0f));
        SetMusicVolume(Musics[MusicName], 1.0f);
    }
}

void Sounds::PlayGameSound(SoundEffect effect)
{
    PlayGameSound(effect.SoundName, effect.SoundVolume, effect.SoundPitch);
}

void Sounds::PlayGameSound(std::string SoundName, float SoundVolume, float SoundPitch) {
    if (GetMasterVolume() <= 0.0f)
        return;
    if (SFX.count(SoundName) && IsSoundValid(SFX[SoundName])) {
        for (Sound& CachedSound : CachedAliases[SoundName]) {
            if (IsSoundValid(CachedSound) && !IsSoundPlaying(CachedSound)) {
                SetSoundVolume(CachedSound, SoundVolume);
                SetSoundPitch(CachedSound, SoundPitch);
                PlaySound(CachedSound);
                return;
            }
        }

        Sound CachedSound = LoadSoundAlias(SFX[SoundName]);
        SetSoundVolume(CachedSound, SoundVolume);
        SetSoundPitch(CachedSound, SoundPitch);
        PlaySound(CachedSound);
        CachedAliases[SoundName].push_back(CachedSound);
    }
}

void Sounds::Quit() {
    for (auto& [name,value] : CachedAliases) {
        for (Sound& sound : value) {
            if (IsSoundValid(sound)) {
                UnloadSoundAlias(sound);
            }
        }
        value.clear();
    }
    for (auto& [name,value] : SFX) {
        if (IsSoundValid(value))
            UnloadSound(value);
    }
    for (auto& [name,value] : Musics) {
        if (IsMusicValid(value))
            UnloadMusicStream(value);
    }
    SFX.clear();
    Musics.clear();
    CachedAliases.clear();
    MusicTransitions.clear();
}
