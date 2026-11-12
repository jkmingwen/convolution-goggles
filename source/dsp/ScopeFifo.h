#pragma once
#include <juce_core/juce_core.h>
#include <array>
#include <span>

class ScopeFifo
{
public:
    static constexpr int capacity = 8192;

    // push samples onto fifo buffer audio thread
    void push (const float* data, int numSamples) noexcept
    {
        const auto w = fifo.write (numSamples);
        std::copy_n (data, w.blockSize1, storage.begin() + w.startIndex1);
        std::copy_n (data + w.blockSize1, w.blockSize2, storage.begin() + w.startIndex2);
    }

    // GUI thread
    int pull (std::span<float> dest) noexcept
    {
        const auto r = fifo.read ((int) dest.size());
        std::copy_n (storage.begin() + r.startIndex1, r.blockSize1, dest.begin());
        std::copy_n (storage.begin() + r.startIndex2, r.blockSize2, dest.begin() + r.blockSize1);
        return r.blockSize1 + r.blockSize2;
    }

private:
    juce::AbstractFifo fifo { capacity };
    std::array<float, capacity> storage {};
};

