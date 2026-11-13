#include "WaveformView.h"
#include <algorithm>

void WaveformView::timerCallback()
{
    const int pulled = fifo.pull (scratch);
    if (pulled == 0)
        return;

    const int n = std::min (pulled, displaySize);
    const float* newest = scratch.data() + (pulled - n); // take the last n samples pulled

    std::move (display.begin() + n, display.end(), display.begin());
    std::copy (newest, newest + n, display.end() - n);

    repaint();
}

void WaveformView::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);

    const auto bounds = getLocalBounds().toFloat();
    juce::Path path;

    for (int i = 0; i < display.size(); ++i)
    {
        const float x = bounds.getWidth() * (float) i / (float) (display.size() - 1);
        // Clamp y to waveform area
        const float y = juce::jlimit (bounds.getY(), bounds.getBottom(),
                                      bounds.getCentreY() - display[(size_t) i] * bounds.getHeight() * 0.4f);


        if (i == 0)
        {
            path.startNewSubPath(x, y);
        }
        else
        {
            path.lineTo(x, y);
        }
    }

    // Draw the waveform
    g.setColour (juce::Colours::lightblue);
    g.strokePath (path, juce::PathStrokeType (1.5f));
}
