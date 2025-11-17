#include <AL/al.h>
#include <AL/alc.h>
#include <vector>
#include <iostream>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>


void testOpenAL()
{
    // Open default device
    ALCdevice* device = alcOpenDevice(nullptr);
    if (!device) {
        std::cerr << "Failed to open OpenAL device" << std::endl;
        return;
    }

    // Create context
    ALCcontext* context = alcCreateContext(device, nullptr);
    if (!context) {
        std::cerr << "Failed to create OpenAL context" << std::endl;
        alcCloseDevice(device);
        return;
    }
    alcMakeContextCurrent(context);

    // Generate a simple 1-second sine wave at 440Hz (A4)
    const int sampleRate = 44100;
    const int duration = 1; // seconds
    const int numSamples = sampleRate * duration;
    std::vector<short> samples(numSamples);

    for (int i = 0; i < numSamples; i++) {
        samples[i] = static_cast<short>(
            32760 * std::sin((2.0 * glm::pi<double>() * 440.0 * i) / sampleRate)
            );
    }

    // Create buffer and source
    ALuint buffer, source;
    alGenBuffers(1, &buffer);
    alBufferData(buffer, AL_FORMAT_MONO16, samples.data(), samples.size() * sizeof(short), sampleRate);

    alGenSources(1, &source);
    alSourcei(source, AL_BUFFER, buffer);

    // Play sound
    alSourcePlay(source);
    std::cout << "Playing test tone..." << std::endl;

    // Wait until it finishes
    ALint state;
    do {
        alGetSourcei(source, AL_SOURCE_STATE, &state);
    } while (state == AL_PLAYING);

    // Cleanup
    alDeleteSources(1, &source);
    alDeleteBuffers(1, &buffer);
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(context);
    alcCloseDevice(device);
}
