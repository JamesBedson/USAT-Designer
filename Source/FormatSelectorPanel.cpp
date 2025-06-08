/*
  ==============================================================================

    FormatSelectorPanel.cpp
    Created: 11 Jan 2025 2:13:58pm
    Author:  James Bedson

  ==============================================================================
*/

#include <JuceHeader.h>
#include "FormatSelectorPanel.h"

//==============================================================================
FormatSelectorPanel::FormatSelectorPanel(StateManager& s,
                                         const UI::FormatType formatType)
: stateManager(s),
layoutSelectorPanel(s, formatType),
ambisonicsSelectorPanel(s, formatType)
{
    addAndMakeVisible(comboBox);
    addChildComponent(layoutSelectorPanel);
    addChildComponent(ambisonicsSelectorPanel);
    
    comboBox.setLookAndFeel(&lookAndFeel);
    comboBox.addListener(this);
    comboBox.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    comboBox.addItem("speaker layout", 1);
    comboBox.addItem("ambisonics", 2);
    
    if (formatType == UI::FormatType::input) {
        comboBoxAttachment = std::make_unique<APVTS::ComboBoxAttachment>
        (s.formatSettings,
         ProcessingConstants::EncodingOptions::inputType,
         comboBox);
    }
    
    else {
        comboBoxAttachment = std::make_unique<APVTS::ComboBoxAttachment>
        (s.formatSettings,
         ProcessingConstants::EncodingOptions::outputType,
         comboBox);
    }
}

FormatSelectorPanel::~FormatSelectorPanel()
{
    comboBox.setLookAndFeel(nullptr);
}

void FormatSelectorPanel::paint (juce::Graphics& g)
{
    g.setColour(UI::ColourDefinitions::outlineColour);
    /*
    if (layoutSelectorPanel.isVisible())
        g.drawRoundedRectangle(layoutSelectorPanel.getBounds().toFloat(), 15.f, UI::Geometry::lineThickness);
    
    else
        g.drawRoundedRectangle(ambisonicsSelectorPanel.getBounds().toFloat(), 15.f, UI::Geometry::lineThickness);
     */
    
}

void FormatSelectorPanel::resized()
{
    const auto boundsReduced = getLocalBounds().reduced(20);
    const float
    panelWidth      = boundsReduced.getWidth(),
    panelHeight     = boundsReduced.getHeight(),
    windowX         = boundsReduced.getX(),
    windowY         = boundsReduced.getY(),
    comboBoxWidth   = panelWidth,
    comboBoxHeight  = panelHeight * UI::FormatPanelFactors::comboBoxHeightFactor;
    
    comboBox.setBounds(windowX,
                       windowY,
                       comboBoxWidth,
                       comboBoxHeight
                       );
    
    const float
    padding             = 20,
    selectorPanelX      = windowX,
    selectorPanelY      = comboBox.getBottom() + padding,
    selectorPanelWidth  = comboBoxWidth,
    selectorPanelHeight = panelHeight - comboBoxHeight - padding;
    
    layoutSelectorPanel.setBounds(selectorPanelX,
                                  selectorPanelY,
                                  selectorPanelWidth,
                                  selectorPanelHeight
                                  );
    
    ambisonicsSelectorPanel.setBounds(selectorPanelX,
                                      selectorPanelY,
                                      selectorPanelWidth,
                                      selectorPanelHeight
                                      );
}

void FormatSelectorPanel::comboBoxChanged(juce::ComboBox *comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == &comboBox) {
        
        const int selectedID = comboBox.getSelectedId();
        
        switch (selectedID) {
            case 1:
                ambisonicsSelectorPanel.setVisible(false);
                layoutSelectorPanel.setVisible(true);
                paintLayoutSelector = true;
                break;
                
            case 2:
                layoutSelectorPanel.setVisible(false);
                ambisonicsSelectorPanel.setVisible(true);
                paintLayoutSelector = true;
                break;
        }
    }
}
