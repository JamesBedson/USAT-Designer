/*
  ==============================================================================

    DecoderSettingsPanel.cpp
    Created: 11 Jan 2025 1:51:44pm
    Author:  James Bedson

  ==============================================================================
*/

#include <JuceHeader.h>
#include "DecoderSettingsPanel.h"

//==============================================================================
DecoderSettingsPanel::DecoderSettingsPanel(USATAudioProcessor& p)
: audioProcessor(p)
{
    addAndMakeVisible(decode);
    decode.setButtonText("decode");
    
    decode.onClick = [this]()
    {
        const auto valueTree    = audioProcessor.stateManager.createGlobalValueTree();
        auto file               = audioProcessor.stateManager.presetsDirectory.getChildFile("GlobalValueTree.xml");
        std::unique_ptr<juce::XmlElement> xml(valueTree.createXml());
        
        if (xml != nullptr) {
            xml->writeTo(file);
        }
        
        audioProcessor.decode();
    };
}

DecoderSettingsPanel::~DecoderSettingsPanel()
{
}

void DecoderSettingsPanel::paint (juce::Graphics& g)
{
    
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (14.0f));
}

void DecoderSettingsPanel::resized()
{
    const float
    buttonCentreX = getLocalBounds().getCentreX(),
    buttonCentreY = getLocalBounds().getCentreY(),
    buttonWidth   = getWidth() * 0.2f,
    buttonHeight  = getHeight() * 0.4f;
    
    decode.setBounds(0, 0, buttonWidth, buttonHeight);
    decode.setCentrePosition(buttonCentreX, buttonCentreY);
}

void DecoderSettingsPanel::valueChanged(juce::Value &value) {
    DBG("Received value: " << value.toString());
}
