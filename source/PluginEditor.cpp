#include "PluginEditor.h"
#include "juce_events/juce_events.h"
#include "juce_graphics/juce_graphics.h"

PluginEditor::PluginEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p), inputView (p.getInputScope())
{
    juce::ignoreUnused (processorRef);
    addAndMakeVisible (inputView);
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

    setSize (600, 300);

}

void PluginEditor::paint (juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void PluginEditor::resized()
{
    // layout the positions of your child components here
    auto area = getLocalBounds().reduced (10); // 10px margin all around
    auto footer = area.removeFromBottom (30); // 30px footer area
    inspectButton.setBounds (footer.removeFromRight (120));
    area.removeFromBottom (10);
    inputView.setBounds(area);
}

