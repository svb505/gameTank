#pragma once
#include <AL/al.h>
#include <AL/alc.h>
#include <sndfile.h>
#include <iostream>
#include <vector>
#include "map"
#include "svbmath.h"

struct SourceConfig {
    std::string name;
    float gain;
    bool looping = false;

    float refDist = -1.0f;
    float maxDist = -1.0f;
    float rolloff = -1.0f;
};

class Sound {
public:
    bool rainPlayed = false;

    ALCdevice* audioDevice = nullptr;
    ALCcontext* audioContext = nullptr;

    std::map<std::string, ALuint> buffers = {
        {"Tank", 0},
        {"Explosion", 0},
        {"ArtExplosion", 0},
        {"ArtVolley", 0},
        {"Shot", 0},
        {"MGun", 0},
        {"Collision", 0},
        {"Rain", 0},
        {"Smoke", 0},
        {"Kill", 0},
    };
    std::map<std::string, ALuint> sources = {
        {"Tank", 0},
        {"Explosion", 0},
        {"ArtExplosion", 0},
        {"ArtVolley", 0},
        {"Shot", 0},
        {"Collision", 0},
        {"MGun", 0},
        {"Rain", 0},
        {"Smoke", 0},
        {"Kill", 0},
    };

    void setSourcePosition(ALuint source, svbmath::Vec3 posit) {
        ALfloat pos[] = { posit.x, posit.y, posit.z };
        ALfloat vel[] = { 0.0f, 0.0f, 0.0f };

        alSourcefv(source, AL_POSITION, pos);
        alSourcefv(source, AL_VELOCITY, vel);
    }
    void setListener(float x, float y, float z, svbmath::Vec3 forwardVector) {
        ALfloat pos[] = { x, y, z };
        ALfloat vel[] = { 0.0f, 0.0f, 0.0f };

        ALfloat ori[] = {
            forwardVector.x, forwardVector.y, forwardVector.z,
            0.0f, 1.0f, 0.0f
        };

        alListenerfv(AL_POSITION, pos);
        alListenerfv(AL_VELOCITY, vel);
        alListenerfv(AL_ORIENTATION, ori);
    }
    ALuint LoadWav(const char* filename){
        SF_INFO info{};
        SNDFILE* file = sf_open(filename, SFM_READ, &info);
        if (!file) {
            std::cerr << "Failed to load sound: " << filename << "\n";
            return 0;
        }

        std::vector<short> samples(info.frames * info.channels);
        sf_readf_short(file, samples.data(), info.frames);
        sf_close(file);

        ALuint buffer;
        alGenBuffers(1, &buffer);

        ALenum format =
            (info.channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

        alBufferData(
            buffer,
            format,
            samples.data(),
            (ALsizei)(samples.size() * sizeof(short)),
            info.samplerate
        );

        return buffer;
    }

    void setupBuffers() {
        audioDevice = alcOpenDevice(nullptr);
        audioContext = alcCreateContext(audioDevice, nullptr);
        alcMakeContextCurrent(audioContext);

        buffers["Tank"] = LoadWav("sounds/tank.wav");
        buffers["Explosion"] = LoadWav("sounds/explosion.wav");
        buffers["ArtExplosion"] = LoadWav("sounds/artExplosion.wav");
        buffers["Shot"] = LoadWav("sounds/shot.wav");
        buffers["MGun"] = LoadWav("sounds/mgun.wav");
        buffers["ArtVolley"] = LoadWav("sounds/artVolley.wav");
        buffers["Collision"] = LoadWav("sounds/collision.wav");
        buffers["Rain"] = LoadWav("sounds/rain.wav");
        buffers["Smoke"] = LoadWav("sounds/smoke.wav");
        buffers["Kill"] = LoadWav("sounds/kill.wav");
    }
    void createSources() {
        alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);

        std::vector<SourceConfig> configs = {
            {"Tank", 0.5f, true, 4.0f, 80.0f, 2.0f},
            {"Explosion", 1.0f, false, 15.0f, 600.0f, 0.6f},
            {"Rain", 1.0f},
            {"Kill", 1.0f},
            {"Collision", 0.7f, false, 4.0f, 80.0f, 2.0f},
            {"ArtExplosion", 1.0f, false, 15.0f, 600.0f, 0.6f},
            {"Shot", 1.0f, false, 10.0f, 400.0f, 0.7f},
            {"ArtVolley", 1.0f, false, 15.0f, 600.0f, 0.6f},
            {"Smoke", 1.0f, false, 4.0f, 80.0f, 2.0f},
            {"MGun", 0.8f, false, 3.0f, 120.0f, 1.5f}
        };

        for (const auto& cfg : configs) {
            alGenSources(1, &sources[cfg.name]);

            alSourcei(sources[cfg.name], AL_BUFFER, buffers[cfg.name]);
            alSourcef(sources[cfg.name], AL_GAIN, cfg.gain);

            if (cfg.looping)
                alSourcei(sources[cfg.name], AL_LOOPING, AL_TRUE);

            if (cfg.refDist > 0.0f)
                alSourcef(sources[cfg.name], AL_REFERENCE_DISTANCE, cfg.refDist);

            if (cfg.maxDist > 0.0f)
                alSourcef(sources[cfg.name], AL_MAX_DISTANCE, cfg.maxDist);

            if (cfg.rolloff > 0.0f)
                alSourcef(sources[cfg.name], AL_ROLLOFF_FACTOR, cfg.rolloff);
        }

        alSourcePlay(sources["Tank"]);
    }
};
