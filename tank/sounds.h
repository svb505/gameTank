#pragma once
#include <AL/al.h>
#include <AL/alc.h>
#include <sndfile.h>
#include <iostream>
#include <vector>
#include "map"

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

    void setSourcePosition(ALuint source, float x, float y, float z) {
        ALfloat pos[] = { x, y, z };
        ALfloat vel[] = { 0.0f, 0.0f, 0.0f };

        alSourcefv(source, AL_POSITION, pos);
        alSourcefv(source, AL_VELOCITY, vel);
    }
    void setListener(float x, float y, float z, float fx, float fy, float fz) {
        ALfloat pos[] = { x, y, z };
        ALfloat vel[] = { 0.0f, 0.0f, 0.0f };

        ALfloat ori[] = {
            fx, fy, fz,
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

        alGenSources(1, &sources["Tank"]);
        alSourcei(sources["Tank"], AL_BUFFER, buffers["Tank"]);
        alSourcei(sources["Tank"], AL_LOOPING, AL_TRUE);
        alSourcef(sources["Tank"], AL_GAIN, 0.5f);
        alSourcePlay(sources["Tank"]);

        alGenSources(1, &sources["Explosion"]);
        alSourcei(sources["Explosion"], AL_BUFFER, buffers["Explosion"]);
        alSourcef(sources["Explosion"], AL_GAIN, 1.0f);

        alGenSources(1, &sources["Rain"]);
        alSourcei(sources["Rain"], AL_BUFFER, buffers["Rain"]);
        alSourcef(sources["Rain"], AL_GAIN, 1.0f);

        alGenSources(1, &sources["Kill"]);
        alSourcei(sources["Kill"], AL_BUFFER, buffers["Kill"]);
        alSourcef(sources["Kill"], AL_GAIN, 1.0f);

        alGenSources(1, &sources["Collision"]);
        alSourcei(sources["Collision"], AL_BUFFER, buffers["Collision"]);
        alSourcef(sources["Collision"], AL_GAIN, 0.7f);

        alGenSources(1, &sources["ArtExplosion"]);
        alSourcei(sources["ArtExplosion"], AL_BUFFER, buffers["ArtExplosion"]);
        alSourcef(sources["ArtExplosion"], AL_GAIN, 1.0f);

        alGenSources(1, &sources["Shot"]);
        alSourcei(sources["Shot"], AL_BUFFER, buffers["Shot"]);
        alSourcef(sources["Shot"], AL_GAIN, 1.0f);

        alGenSources(1, &sources["ArtVolley"]);
        alSourcei(sources["ArtVolley"], AL_BUFFER, buffers["ArtVolley"]);
        alSourcef(sources["ArtVolley"], AL_GAIN, 1.0f);

        alGenSources(1, &sources["Smoke"]);
        alSourcei(sources["Smoke"], AL_BUFFER, buffers["Smoke"]);
        alSourcef(sources["Smoke"], AL_GAIN, 1.0f);

        alGenSources(1, &sources["MGun"]);
        alSourcei(sources["MGun"], AL_BUFFER, buffers["MGun"]);
        alSourcef(sources["MGun"], AL_GAIN, 0.8f);
        //-------------------------------------------------------------
        alSourcef(sources["MGun"], AL_REFERENCE_DISTANCE, 3.0f);
        alSourcef(sources["MGun"], AL_MAX_DISTANCE, 120.0f);
        alSourcef(sources["MGun"], AL_ROLLOFF_FACTOR, 1.5f);

        alSourcef(sources["Shot"], AL_REFERENCE_DISTANCE, 10.0f);
        alSourcef(sources["Shot"], AL_MAX_DISTANCE, 400.0f);
        alSourcef(sources["Shot"], AL_ROLLOFF_FACTOR, 0.7f);

        alSourcef(sources["Explosion"], AL_REFERENCE_DISTANCE, 15.0f);
        alSourcef(sources["Explosion"], AL_MAX_DISTANCE, 600.0f);
        alSourcef(sources["Explosion"], AL_ROLLOFF_FACTOR, 0.6f);

        alSourcef(sources["ArtExplosion"], AL_REFERENCE_DISTANCE, 15.0f);
        alSourcef(sources["ArtExplosion"], AL_MAX_DISTANCE, 600.0f);
        alSourcef(sources["ArtExplosion"], AL_ROLLOFF_FACTOR, 0.6f);

        alSourcef(sources["ArtVolley"], AL_REFERENCE_DISTANCE, 15.0f);
        alSourcef(sources["ArtVolley"], AL_MAX_DISTANCE, 600.0f);
        alSourcef(sources["ArtVolley"], AL_ROLLOFF_FACTOR, 0.6f);

        alSourcef(sources["Tank"], AL_REFERENCE_DISTANCE, 4.0f);
        alSourcef(sources["Tank"], AL_MAX_DISTANCE, 80.0f);
        alSourcef(sources["Tank"], AL_ROLLOFF_FACTOR, 2.0f);

        alSourcef(sources["Collision"], AL_REFERENCE_DISTANCE, 4.0f);
        alSourcef(sources["Collision"], AL_MAX_DISTANCE, 80.0f);
        alSourcef(sources["Collision"], AL_ROLLOFF_FACTOR, 2.0f);

        alSourcef(sources["Smoke"], AL_REFERENCE_DISTANCE, 4.0f);
        alSourcef(sources["Smoke"], AL_MAX_DISTANCE, 80.0f);
        alSourcef(sources["Smoke"], AL_ROLLOFF_FACTOR, 2.0f);
    }
};
