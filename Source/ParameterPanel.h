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
#include "PluginProcessor.h"
//==============================================================================
/*
*/
using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;

class SliderPanel  :
public juce::Component,
public juce::Slider::Listener,
public juce::Label::Listener
{
    
public:
    
    SliderPanel() {
        addAndMakeVisible(slider);
        addAndMakeVisible(textLabel);
        addAndMakeVisible(valueLabel);
        
        slider.addListener(this);
        valueLabel.addListener(this);
        slider.setMouseCursor(juce::MouseCursor::UpDownResizeCursor);
        valueLabel.setMouseCursor(juce::MouseCursor::IBeamCursor);
    }
    ~SliderPanel() override {
        slider.removeListener(this);
        valueLabel.removeListener(this);
    }

    void paint (juce::Graphics& g) override;
    void resized() override;
    void sliderValueChanged(juce::Slider* slider) override;
    void labelTextChanged (juce::Label* labelThatHasChanged) override;
    
    juce::Slider slider;
    juce::Label textLabel;
    juce::Label valueLabel;
private:
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SliderPanel)
};

class ParameterPanel  : public juce::Component,
juce::Value::Listener
{
    
public:
    
    enum ParameterSelectorChoice {
        Simple,
        Advanced_1,
        Advanced_2
    };
    
    ParameterPanel(StateManager&, USATAudioProcessor&);
    ~ParameterPanel() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    void setSimpleParameterPage();
    void setAdvancedParameterPage(const ParameterSelectorChoice& advancedChoice);
    void refreshUIFromState();
    void valueChanged(juce::Value& value) override;
    
private:
    
    void setSimpleVisibility(bool isVisible);
    void setAdvancedVisibility(bool isVisible);
    
    StateManager& stateManager;
    // ADVANCED ===========================================
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
    
    // SIMPLE ===========================================
    juce::ComboBox focusParams;
    juce::TextButton loadFocusParams;
    juce::Label focusLabel;
    USATAudioProcessor& p;
    CustomLNF lookAndFeel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParameterPanel)
};

