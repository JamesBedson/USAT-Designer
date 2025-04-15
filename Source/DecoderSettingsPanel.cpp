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
    decode.addListener(this);
    decode.setButtonText("decode");
}

DecoderSettingsPanel::~DecoderSettingsPanel(){
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

void DecoderSettingsPanel::buttonClicked(juce::Button *button) {
    if (button == &decode) {
        
        const auto valueTree    = audioProcessor.stateManager.createGlobalValueTree();
        auto file               = audioProcessor.stateManager.presetsDirectory.getChildFile("GlobalValueTree.xml");
        std::unique_ptr<juce::XmlElement> xml(valueTree.createXml());
        
        if (xml != nullptr) {
            xml->writeTo(file);
        }
        
        BlockingPopup* popup = new BlockingPopup(&audioProcessor);
        const float
        popupWidth  = getWidth() * 0.3f,
        popupHeight = getHeight() * 0.3f;
        
        popup->setSize(popupWidth, popupHeight);
        
        juce::DialogWindow::LaunchOptions options;
        options.content.setOwned(popup);
        options.dialogTitle                     = "Decoding...";
        options.escapeKeyTriggersCloseButton    = false;
        options.useNativeTitleBar               = false;
        options.resizable                       = false;
        options.useBottomRightCornerResizer     = false;
        options.launchAsync();
        
        audioProcessor.decode();
    }
}
