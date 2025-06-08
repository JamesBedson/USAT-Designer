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
: formatType(formatType),
background(juce::ImageCache::getFromMemory(BinaryData::formatSelectBackground3x_png, BinaryData::formatSelectBackground3x_pngSize))
{
    orders.setLookAndFeel(&lookAndFeel);
    orders.addItemList(ProcessingConstants::EncodingOptions::Ambisonics::orderChoices, 1);
    orders.addListener(this);
    
    if (formatType == UI::FormatType::input) {
        comboBoxAttachment = std::make_unique<APVTS::ComboBoxAttachment>
        (s.formatSettings,
         ProcessingConstants::EncodingOptions::Ambisonics::orderIn,
         orders);
    }
    
    else {
        comboBoxAttachment = std::make_unique<APVTS::ComboBoxAttachment>
        (s.formatSettings,
         ProcessingConstants::EncodingOptions::Ambisonics::orderOut,
         orders);
    }
    
    addAndMakeVisible(orders);
    addAndMakeVisible(orderLabel);
    addAndMakeVisible(channelsLabel);
    addAndMakeVisible(formatLabel);
    
    orders.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    
    orderLabel.setText("order", juce::dontSendNotification);
    orderLabel.setFont(UI::Fonts::getMainFontWithSize(14.f));
    
    channelsLabel.setFont(UI::Fonts::getMainFontWithSize(14.f));
    setChannelConfigText();
    
    formatLabel.setFont(UI::Fonts::getMainFontWithSize(19.f));
    formatLabel.setJustificationType(juce::Justification::horizontallyCentred);
    formatLabel.setColour(juce::Label::textColourId, UI::ColourDefinitions::accentColour);
    setFormatLabelText();
    //orders.setText("Ambisonics Order");
}

AmbisonicsSelectorPanel::~AmbisonicsSelectorPanel()
{
    orders.setLookAndFeel(nullptr);
    orders.removeListener(this);
}

void AmbisonicsSelectorPanel::paint (juce::Graphics& g)
{
    g.drawImage(background, getLocalBounds().toFloat());
}

void AmbisonicsSelectorPanel::resized()
{
    auto reducedBounds = getLocalBounds().reduced(10.f, 10.f);
    const float
    panelWidth      = reducedBounds.getWidth(),
    panelHeight     = reducedBounds.getHeight(),
    comboBoxWidth   = panelWidth * UI::AmbisonicsSelectorPanelFactors::comboBoxWidthFactor,
    comboBoxHeight  = panelHeight * UI::AmbisonicsSelectorPanelFactors::comboBoxHeightFactor,
    comboBoxRightX  = reducedBounds.getRight(),
    comboBoxRightY  = reducedBounds.getBottom() - comboBoxHeight;
    
    orders.setSize(comboBoxWidth, comboBoxHeight);
    orders.setTopRightPosition(comboBoxRightX, comboBoxRightY);
    
    const float
    ordersLabelWidth = panelWidth * UI::AmbisonicsSelectorPanelFactors::ordersWidthFactor;
    
    orderLabel.setSize(ordersLabelWidth, comboBoxHeight);
    orderLabel.setTopLeftPosition(reducedBounds.getX(), orders.getY());
    
    const float
    componentWidth  = panelWidth,
    formatX         = reducedBounds.getX(),
    formatY         = reducedBounds.getY();
    
    formatLabel.setSize(componentWidth, comboBoxHeight);
    formatLabel.setTopLeftPosition(formatX, formatY);
    
    const float
    channelsLabelOffsetY = 35.f;
    
    channelsLabel.setSize(componentWidth, comboBoxHeight);
    channelsLabel.setTopLeftPosition(formatX, formatY + channelsLabelOffsetY);
    
}

void AmbisonicsSelectorPanel::comboBoxChanged(juce::ComboBox *comboBoxThatHasChanged) {
    setChannelConfigText();
}
