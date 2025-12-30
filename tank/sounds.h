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
    ALuint shootBuffer = 0;
    ALuint mgunBuffer = 0;
    ALuint tankSource = 0;
    ALuint explosionSource = 0;
    ALuint shootSource = 0;
    ALuint mgunSource = 0;

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
        shootBuffer = LoadWav("sounds/shoot.wav");
        mgunBuffer = LoadWav("sounds/mgun.wav");
    }
    void createSources() {
        alGenSources(1, &tankSource);
        alSourcei(tankSource, AL_BUFFER, tankBuffer);
        alSourcei(tankSource, AL_LOOPING, AL_TRUE);
        alSourcef(tankSource, AL_GAIN, 0.7f);
        alSourcePlay(tankSource);

        alGenSources(1, &explosionSource);
        alSourcei(explosionSource, AL_BUFFER, explosionBuffer);
        alSourcef(explosionSource, AL_GAIN, 1.0f);

        alGenSources(1, &shootSource);
        alSourcei(shootSource, AL_BUFFER, shootBuffer);
        alSourcef(shootSource, AL_GAIN, 1.0f);

        alGenSources(1, &mgunSource);
        alSourcei(mgunSource, AL_BUFFER, mgunBuffer);
        alSourcef(mgunSource, AL_GAIN, 1.0f);
    }
};
