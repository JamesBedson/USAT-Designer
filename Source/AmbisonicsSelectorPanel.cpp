/*
  ==============================================================================

    AmbisonicsSelectorPanel.cpp
    Created: 12 Jan 2025 11:57:08am
    Author:  James Bedson

  ==============================================================================
*/

#include <JuceHeader.h>
#include "AmbisonicsSelectorPanel.h"

//==============================================================================
AmbisonicsSelectorPanel::AmbisonicsSelectorPanel(StateManager& s,
                                                 const UI::FormatType formatType)
: formatType(formatType)
{
    orders.addItemList(ProcessingConstants::EncodingOptions::Ambisonics::orderChoices, 1);
    
    if (formatType == UI::FormatType::input) {
        comboBoxAttachment = std::make_unique<APVTS::ComboBoxAttachment>
        (s.apvts,
         ProcessingConstants::EncodingOptions::Ambisonics::orderIn,
         orders);
    }
    
    else {
        comboBoxAttachment = std::make_unique<APVTS::ComboBoxAttachment>
        (s.apvts,
         ProcessingConstants::EncodingOptions::Ambisonics::orderOut,
         orders);
    }
    
    addAndMakeVisible(orders);
    //orders.setText("Ambisonics Order");
}

AmbisonicsSelectorPanel::~AmbisonicsSelectorPanel()
{
}

void AmbisonicsSelectorPanel::paint (juce::Graphics& g)
{

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (14.0f));
}

void AmbisonicsSelectorPanel::resized()
{
    const float
    panelHeight     = getHeight(),
    panelWidth      = getWidth(),
    padding         = panelHeight * UI::AmbisonicsSelectorPanelFactors::paddingFactor,
    comboBoxWidth   = panelWidth * UI::AmbisonicsSelectorPanelFactors::comboBoxWidthFactor,
    comboBoxHeight  = panelHeight * UI::AmbisonicsSelectorPanelFactors::comboBoxHeightFactor,
    comboBoxCentreX = getLocalBounds().getCentreX(),
    comboBoxCentreY = padding + panelHeight * UI::AmbisonicsSelectorPanelFactors::ordersYPosFactor;
    
    orders.setBounds(0, 0, comboBoxWidth, comboBoxHeight);
    orders.setCentrePosition(comboBoxCentreX, comboBoxCentreY);
    
}
