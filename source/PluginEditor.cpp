#include "PluginEditor.h"
#include "juce_events/juce_events.h"
#include "juce_graphics/juce_graphics.h"

PluginEditor::PluginEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    juce::ignoreUnused (processorRef);

    addAndMakeVisible (inspectButton);

    // this chunk of code instantiates and opens the melatonin inspector
    inspectButton.onClick = [&] {
        if (!inspector)
        {
            inspector = std::make_unique<melatonin::Inspector> (*this);
            inspector->onClose = [this]() { inspector.reset(); };
        }

        inspector->setVisible (true);
    };

    // Define parameters of slider object
    midiVolume.setSliderStyle (juce::Slider::LinearBarVertical);
    midiVolume.setRange (0.0, 127.0, 1.0);
    midiVolume.setTextBoxStyle (juce::Slider::NoTextBox, false, 90, 0);
    midiVolume.setPopupDisplayEnabled (true, false, this);
    midiVolume.setTextValueSuffix (" Volume");
    midiVolume.setValue (1.0);
    // midiVolume.addListener(this);
    // this function adds the slider to the editor
    addAndMakeVisible (&midiVolume);

    // define label and button parameters
    checkTimeButton.setButtonText ("Check the time...");
    checkTimeButton.addListener (this);
    addAndMakeVisible (checkTimeButton);
    addAndMakeVisible (timeLabel);
    timeLabel.setColour (juce::Label::backgroundColourId, juce::Colours::black);
    timeLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    timeLabel.setJustificationType (juce::Justification::centred);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);

    displayData.resize (displaySize);
    std::fill (displayData.begin(), displayData.end(), 0.0f);
    displayBuffer.setSize (1, PluginProcessor::fifoSize);
    startTimer (33);
}

PluginEditor::~PluginEditor()
{
    checkTimeButton.removeListener (this);
    stopTimer();
}

void PluginEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    auto area = getLocalBounds();
    g.setColour (juce::Colours::white);
    g.setFont (16.0f);
    auto helloWorld = juce::String ("Hello jaime from ") + PRODUCT_NAME_WITHOUT_VERSION + " v" VERSION + " running in " + CMAKE_BUILD_TYPE;
    g.drawText (helloWorld, area.removeFromTop (150), juce::Justification::centred, false);

    // Draw waveform
    if (!displayData.empty())
    {
        juce::Path waveformPath;
        bool firstPoint = true;

        for (int i = 0; i < displayData.size(); ++i)
        {
            float x = waveformArea.getX() + (i * waveformArea.getWidth() / (float) displayData.size());
            float y = waveformArea.getCentreY() - (displayData[i] * waveformArea.getHeight() * 0.4f);

            // Clamp y to waveform area
            y = juce::jlimit ((float) waveformArea.getY(),
                (float) waveformArea.getBottom(),
                y);

            if (firstPoint)
            {
                waveformPath.startNewSubPath (x, y);
                firstPoint = false;
            }
            else
            {
                waveformPath.lineTo (x, y);
            }
        }

        // Draw the waveform
        g.setColour (juce::Colours::lightblue);
        g.strokePath (waveformPath, juce::PathStrokeType (1.5f));
    }
}

void PluginEditor::resized()
{
    // layout the positions of your child components here
    auto area = getLocalBounds().reduced (10); // 10px margin all around
    auto footer = area.removeFromBottom (30); // 30px footer area
    inspectButton.setBounds (footer.removeFromRight (120));
    area.removeFromBottom (10);
    waveformArea = area;

    // Set waveform area with some padding
    waveformArea = getLocalBounds().reduced (20);
    waveformArea.removeFromTop (40); // Space for title
}

void PluginEditor::buttonClicked (juce::Button* button)
{
    if (button == &checkTimeButton)
    {
        auto currentTime = juce::Time::getCurrentTime();
        auto includeDate = true;
        auto includeTime = true;
        auto currentTimeString = currentTime.toString (includeDate, includeTime);
        timeLabel.setText (currentTimeString, juce::dontSendNotification);
    }
}

void PluginEditor::timerCallback()
{
    const int ready = processorRef.getNumSamplesReady();
    if (ready == 0)
    {
        return;
    }

    processorRef.readDisplayData (displayBuffer, ready);

    const int n = juce::jmin (ready, displaySize);
    const float* newest = displayBuffer.getReadPointer (0) + (ready - n);

    std::move (displayData.begin() + n, displayData.end(), displayData.begin());
    std::copy (newest, newest + n, displayData.end() - n);

    repaint (waveformArea);
}
