/*
  ==============================================================================

    ParameterPanel.h
    Created: 11 Jan 2025 2:14:13pm
    Author:  James Bedson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "StateManager.h"
#include "UIConstants.h"
#include "CustomLNF.h"
//==============================================================================
/*
*/
using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;

class SliderPanel  :
public juce::Component,
public juce::Slider::Listener
{
    
public:
    
    SliderPanel() {
        addAndMakeVisible(slider);
        addAndMakeVisible(textLabel);
        addAndMakeVisible(valueLabel);
        slider.addListener(this);
    }
    ~SliderPanel() override {
        slider.removeListener(this);
    }

    void paint (juce::Graphics& g) override;
    void resized() override;
    void sliderValueChanged(juce::Slider* slider) override;
    
    juce::Slider slider;
    juce::Label textLabel;
    juce::Label valueLabel;
    
private:
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SliderPanel)
};

class ParameterPanel  : public juce::Component
{
    
public:
    
    enum ParameterSelectorChoice {
        Simple,
        Advanced_1,
        Advanced_2
    };
    
    ParameterPanel(StateManager&);
    ~ParameterPanel() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    void setSimpleParameterPage();
    void setAdvancedParameterPage(const ParameterSelectorChoice& advancedChoice);

private:
    StateManager& stateManager;
    
    SliderPanel
    energy,
    radialIntensity,
    transverseIntensity,
    pressure,
    radialVelocity,
    transverseVelocity,
    inPhaseQuadratic,
    symmetryQuadratic,
    inPhaseLinear,
    symmetryLinear,
    totalGainsLinear,
    totalGainsQuadratic;
        
    std::vector<SliderPanel*> sliderPanels {
        &energy,
        &radialIntensity,
        &transverseIntensity,
        &pressure,
        &radialVelocity,
        &transverseVelocity,
        &inPhaseQuadratic,
        &symmetryQuadratic,
        &inPhaseLinear,
        &symmetryLinear,
        &totalGainsLinear,
        &totalGainsQuadratic
    };
    
    std::vector<juce::String> labelTexts{
        "energy",
        "radial\nintensity",
        "transverse\nintensity",
        "pressure",
        "radial\nvelocity",
        "transverse\nvelocity",
        "quadratic\nphase",
        "quadratic\nsymmetry",
        "linear\nphase",
        "linear\nsymmetry",
        "linear\ngain",
        "quadratic\ngain"
    };
    
    CustomLNF lookAndFeel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParameterPanel)
};

