// draw one scrolling waveform from one ScopeFifo
#pragma once

#include "../dsp/ScopeFifo.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include <juce_core/juce_core.h>

class WaveformView : public juce::Component, private juce::Timer
{
public:
    explicit WaveformView (ScopeFifo& source) : fifo (source) { startTimerHz (30); }
    void paint (juce::Graphics&) override;

private:
    void timerCallback() override;

    ScopeFifo& fifo;
    static constexpr int displaySize = 512;
    std::array<float, ScopeFifo::capacity> scratch {};
    std::array<float, displaySize> display {};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveformView)
};
