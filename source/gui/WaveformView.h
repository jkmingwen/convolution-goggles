// draw one scrolling waveform from one ScopeFifo
#pragma once

#include "juce_gui_basics/juce_gui_basics.h"
#include <juce_core/juce_core.h>

class WaveformView : public juce::Component, private juce::Timer
{
 public:
     explicit WaveformView (ScgopeFifo& source) : fifo (source) { startTimerHz (30); }
     void paint (juce::Graphics&) override;

 private:
     void timerCallback() override;

     ScopeFifo& fifo;
     std::array<float, ScopeFifo::capacity> scratch {};
     std::array<float, 512> display {};
};
