/*
  ==============================================================================

    VisualisationSection.h
    Created: 11 Jan 2025 1:22:20pm
    Author:  James Bedson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "StateManager.h"
#include "VisualisationPanel.h"
#include "UIConstants.h"
#include "CustomLNF.h"
//==============================================================================
/*
*/
class VisualisationSection  : public juce::Component,
juce::Value::Listener,
juce::Button::Listener
{
public:
    
    enum ImageToDisplay {
        energy,
        intensity,
        errorWidth,
        placeholder
    };
    
    VisualisationSection(StateManager&);
    ~VisualisationSection() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void valueChanged(juce::Value& value) override;
    juce::Image decodeBase64ToImage(const juce::String& base64String);
    void loadPlots(bool repaint);
    void buttonClicked (juce::Button*) override;
    
    void setButtonToggleStates(const VisualisationSection::ImageToDisplay currentPlot);
    
    void setPanelImages();
    
    bool isPlotDataAvailable();
    
private:
    
    StateManager& stateManager;
    
    VisualisationPanel
    visPanelTop,
    visPanelBottom;
    
    juce::Image sectionBackground;
    juce::Image panelBackground;
    
    ImageToDisplay currentPlotSet;
    
    bool containsPlots;
    
    juce::TextButton energyButton;
    juce::TextButton intensityButton;
    juce::TextButton errorButton;
    
    juce::Image energyPlot;
    juce::Image pressurePlot;
    juce::Image radialIntensityPlot;
    juce::Image transverseIntensityPlot;
    juce::Image angularErrorPlot;
    juce::Image sourceWidthPlot;
    
    CustomLNF lookAndFeel;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VisualisationSection)
};
