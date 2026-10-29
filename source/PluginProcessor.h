#pragma once

#include "juce_core/juce_core.h"
#include <juce_audio_processors/juce_audio_processors.h>

#if (MSVC)
#include "ipps.h"
#endif

class PluginProcessor : public juce::AudioProcessor
{
public:
    PluginProcessor();
    ~PluginProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    float inputSampleVal;
    bool hasDataToRead() const { return audioFifo.getNumReady() > 0; }
    int getNumSamplesReady() const { return audioFifo.getNumReady(); }
    void writeSamplesToFifo(const float* data, int numSamples);
    void readDisplayData (juce::AudioBuffer<float>& destBuffer, int numSamples);
  static constexpr int audioBufferSize = 1024;
  static constexpr int scopeSize = 512;


  private:
      // FIFO for thread-safe communication
    static constexpr int fifoSize = 2048;  // Must be power of 2
    juce::AbstractFifo audioFifo { fifoSize };
    juce::AudioBuffer<float> fifoBuffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginProcessor)
};
