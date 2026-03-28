#pragma once
#include <AL/al.h>
#include <AL/alc.h>
#include <sndfile.h>
#include <iostream>
#include <vector>

class Sound {
public:
    ALCdevice* audioDevice = nullptr;
    ALCcontext* audioContext = nullptr;

    ALuint tankBuffer = 0;
    ALuint explosionBuffer = 0;
    ALuint shotBuffer = 0;
    ALuint mgunBuffer = 0;
    ALuint artVolleyBuffer = 0;
    ALuint artExplosionBuffer = 0;
    ALuint tankSource = 0;
    ALuint explosionSource = 0;
    ALuint artExplosionSource = 0;
    ALuint artVolleySource = 0;
    ALuint shotSource = 0;
    ALuint mgunSource = 0;

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
    ALuint LoadWav(const char* filename)
    {
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

        tankBuffer = LoadWav("sounds/tank.wav");
        explosionBuffer = LoadWav("sounds/explosion.wav");
        artExplosionBuffer = LoadWav("sounds/artExplosion.wav");
        shotBuffer = LoadWav("sounds/shot.wav");
        mgunBuffer = LoadWav("sounds/mgun.wav");
        artVolleyBuffer = LoadWav("sounds/artVolley.wav");
    }
    void createSources() {
        alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);

        alGenSources(1, &tankSource);
        alSourcei(tankSource, AL_BUFFER, tankBuffer);
        alSourcei(tankSource, AL_LOOPING, AL_TRUE);
        alSourcef(tankSource, AL_GAIN, 0.5f);
        alSourcePlay(tankSource);

        alGenSources(1, &explosionSource);
        alSourcei(explosionSource, AL_BUFFER, explosionBuffer);
        alSourcef(explosionSource, AL_GAIN, 1.0f);

        alGenSources(1, &artExplosionSource);
        alSourcei(artExplosionSource, AL_BUFFER, artExplosionBuffer);
        alSourcef(artExplosionSource, AL_GAIN, 1.0f);

        alGenSources(1, &shotSource);
        alSourcei(shotSource, AL_BUFFER, shotBuffer);
        alSourcef(shotSource, AL_GAIN, 1.0f);

        alGenSources(1, &artVolleySource);
        alSourcei(artVolleySource, AL_BUFFER, artVolleyBuffer);
        alSourcef(artVolleySource, AL_GAIN, 1.0f);

        alGenSources(1, &mgunSource);
        alSourcei(mgunSource, AL_BUFFER, mgunBuffer);
        alSourcef(mgunSource, AL_GAIN, 0.8f);

        alSourcef(mgunSource, AL_REFERENCE_DISTANCE, 3.0f);
        alSourcef(mgunSource, AL_MAX_DISTANCE, 120.0f);
        alSourcef(mgunSource, AL_ROLLOFF_FACTOR, 1.5f);

        alSourcef(shotSource, AL_REFERENCE_DISTANCE, 10.0f);
        alSourcef(shotSource, AL_MAX_DISTANCE, 400.0f);
        alSourcef(shotSource, AL_ROLLOFF_FACTOR, 0.7f);

        alSourcef(explosionSource, AL_REFERENCE_DISTANCE, 15.0f);
        alSourcef(explosionSource, AL_MAX_DISTANCE, 600.0f);
        alSourcef(explosionSource, AL_ROLLOFF_FACTOR, 0.6f);

        alSourcef(artExplosionSource, AL_REFERENCE_DISTANCE, 15.0f);
        alSourcef(artExplosionSource, AL_MAX_DISTANCE, 600.0f);
        alSourcef(artExplosionSource, AL_ROLLOFF_FACTOR, 0.6f);

        alSourcef(artVolleySource, AL_REFERENCE_DISTANCE, 15.0f);
        alSourcef(artVolleySource, AL_MAX_DISTANCE, 600.0f);
        alSourcef(artVolleySource, AL_ROLLOFF_FACTOR, 0.6f);

        alSourcef(tankSource, AL_REFERENCE_DISTANCE, 4.0f);
        alSourcef(tankSource, AL_MAX_DISTANCE, 80.0f);
        alSourcef(tankSource, AL_ROLLOFF_FACTOR, 2.0f);
    }
};
