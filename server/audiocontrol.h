#pragma once

#include <vector>
#include <string>
#include <optional>
#include <cstdint>

struct AudioStream {
    uint8_t volume;
    std::string name;
};

// TODO: Use this class as top class, with pulsestreams as implementation, or the other way around?
class AudioControl {
    public:
        AudioControl() = default;
        AudioControl(const AudioControl &other) = default;
        AudioControl(AudioControl &&other) = default;
        ~AudioControl() = default;

        AudioControl &operator=(const AudioControl &other) = default;
        AudioControl &operator=(AudioControl &&other) = default;

        void setVolume(std::string &streamName);
        std::optional<uint8_t> getVolume(std::string &streamName);
        void addNewStream(AudioStream streams);
        void removeStream(AudioStream streams);
        void addNewStreams(std::vector<AudioStream> streams);
        void removeStreams(std::vector<AudioStream> streams);
    private:
        std::vector<AudioStream> streams;
};