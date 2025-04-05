/*
  ==============================================================================

    ParameterPanel.cpp
    Created: 11 Jan 2025 2:14:13pm
    Author:  James Bedson

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ParameterPanel.h"

//==============================================================================
ParameterPanel::ParameterPanel(StateManager& s)
: stateManager(s)
{
    
    auto coefficientTree = s.pluginParameterHandler.getCoefficientTree();
    
    energy.setValue(coefficientTree.getProperty(ProcessingConstants::Coeffs::energy));
    radialIntensity.setValue(coefficientTree.getProperty(ProcessingConstants::Coeffs::radialIntensity));
    pressure.setValue(coefficientTree.getProperty(ProcessingConstants::Coeffs::pressure));
    transverseVelocity.setValue(coefficientTree.getProperty(ProcessingConstants::Coeffs::transverseVelocity));
    inPhaseQuadratic.setValue(coefficientTree.getProperty(ProcessingConstants::Coeffs::inPhaseQuadratic));
    inPhaseLinear.setValue(coefficientTree.getProperty(ProcessingConstants::Coeffs::inPhaseLinear));
    symmetryLinear.setValue(coefficientTree.getProperty(ProcessingConstants::Coeffs::symmetryLinear));
    totalGainsLinear.setValue(coefficientTree.getProperty(ProcessingConstants::Coeffs::totalGainsLinear));
    totalGainsQuadratic.setValue(coefficientTree.getProperty(ProcessingConstants::Coeffs::totalGainsQuadratic));
    
    addAndMakeVisible(energy);
    addAndMakeVisible(radialIntensity);
    addAndMakeVisible(pressure);
    addAndMakeVisible(transverseVelocity);
    addAndMakeVisible(radialVelocity);
    addAndMakeVisible(inPhaseQuadratic);
    addAndMakeVisible(inPhaseLinear);
    addAndMakeVisible(symmetryLinear);
    addAndMakeVisible(totalGainsLinear);
    addAndMakeVisible(totalGainsQuadratic);
    
    energy.onValueChange = [&]() {
        auto coefficientTree = s.pluginParameterHandler.getCoefficientTree();
        coefficientTree.setProperty(ProcessingConstants::Coeffs::energy,
                                    energy.getValue(),
                                    nullptr
                                    );
    };

    radialIntensity.onValueChange = [&]() {
        auto coefficientTree = s.pluginParameterHandler.getCoefficientTree();
        coefficientTree.setProperty(ProcessingConstants::Coeffs::radialIntensity,
                                    radialIntensity.getValue(),
                                    nullptr
                                    );
    };

    pressure.onValueChange = [&]() {
        auto coefficientTree = s.pluginParameterHandler.getCoefficientTree();
        coefficientTree.setProperty(ProcessingConstants::Coeffs::pressure,
                                    pressure.getValue(),
                                    nullptr
                                    );
    };

    transverseVelocity.onValueChange = [&]() {
        auto coefficientTree = s.pluginParameterHandler.getCoefficientTree();
        coefficientTree.setProperty(ProcessingConstants::Coeffs::transverseVelocity,
                                    transverseVelocity.getValue(),
                                    nullptr
                                    );
    };

    radialVelocity.onValueChange = [&]() {
        auto coefficientTree = s.pluginParameterHandler.getCoefficientTree();
        coefficientTree.setProperty(ProcessingConstants::Coeffs::radialVelocity,
                                    radialVelocity.getValue(),
                                    nullptr
                                    );
    };

    inPhaseQuadratic.onValueChange = [&]() {
        auto coefficientTree = s.pluginParameterHandler.getCoefficientTree();
        coefficientTree.setProperty(ProcessingConstants::Coeffs::inPhaseQuadratic,
                                    inPhaseQuadratic.getValue(),
                                    nullptr
                                    );
    };

    inPhaseLinear.onValueChange = [&]() {
        auto coefficientTree = s.pluginParameterHandler.getCoefficientTree();
        coefficientTree.setProperty(ProcessingConstants::Coeffs::inPhaseLinear,
                                    inPhaseLinear.getValue(),
                                    nullptr
                                    );
    };

    symmetryLinear.onValueChange = [&]() {
        auto coefficientTree = s.pluginParameterHandler.getCoefficientTree();
        coefficientTree.setProperty(ProcessingConstants::Coeffs::symmetryLinear,
                                    symmetryLinear.getValue(),
                                    nullptr
                                    );
    };

    totalGainsLinear.onValueChange = [&]() {
        auto coefficientTree = s.pluginParameterHandler.getCoefficientTree();
        coefficientTree.setProperty(ProcessingConstants::Coeffs::totalGainsLinear,
                                    totalGainsLinear.getValue(),
                                    nullptr
                                    );
    };

    totalGainsQuadratic.onValueChange = [&]() {
        auto coefficientTree = s.pluginParameterHandler.getCoefficientTree();
        coefficientTree.setProperty(ProcessingConstants::Coeffs::totalGainsQuadratic,
                                    totalGainsQuadratic.getValue(),
                                    nullptr
                                    );
    };
    
    addAndMakeVisible(energyLabel);
    addAndMakeVisible(radialIntensityLabel);
    addAndMakeVisible(pressureLabel);
    addAndMakeVisible(transverseVelocityLabel);
    addAndMakeVisible(radialVelocityLabel);
    addAndMakeVisible(inPhaseQuadraticLabel);
    addAndMakeVisible(inPhaseLinearLabel);
    addAndMakeVisible(symmetryLinearLabel);
    addAndMakeVisible(totalGainsQuadraticLabel);
    addAndMakeVisible(totalGainsLinearLabel);
    addAndMakeVisible(totalGainsQuadraticLabel);
    
    energyLabel.setText(ProcessingConstants::Coeffs::energy, juce::dontSendNotification);
    radialIntensityLabel.setText(ProcessingConstants::Coeffs::radialIntensity, juce::dontSendNotification);
    pressureLabel.setText(ProcessingConstants::Coeffs::pressure, juce::dontSendNotification);
    transverseVelocityLabel.setText(ProcessingConstants::Coeffs::transverseVelocity, juce::dontSendNotification);
    radialVelocityLabel.setText(ProcessingConstants::Coeffs::radialVelocity, juce::dontSendNotification);
    inPhaseQuadraticLabel.setText(ProcessingConstants::Coeffs::inPhaseQuadratic, juce::dontSendNotification);
    inPhaseLinearLabel.setText(ProcessingConstants::Coeffs::inPhaseLinear, juce::dontSendNotification);
    symmetryLinearLabel.setText(ProcessingConstants::Coeffs::symmetryLinear, juce::dontSendNotification);
    totalGainsQuadraticLabel.setText(ProcessingConstants::Coeffs::totalGainsQuadratic, juce::dontSendNotification);
    totalGainsLinearLabel.setText(ProcessingConstants::Coeffs::totalGainsLinear, juce::dontSendNotification);
    
    energy.setTextBoxStyle(juce::Slider::TextBoxRight, false, 100, 20);
    radialIntensity.setTextBoxStyle(juce::Slider::TextBoxRight, false, 100, 20);
    pressure.setTextBoxStyle(juce::Slider::TextBoxRight, false, 100, 20);
    transverseVelocity.setTextBoxStyle(juce::Slider::TextBoxRight, false, 100, 20);
    radialVelocity.setTextBoxStyle(juce::Slider::TextBoxRight, false, 100, 20);
    inPhaseQuadratic.setTextBoxStyle(juce::Slider::TextBoxRight, false, 100, 20);
    inPhaseLinear.setTextBoxStyle(juce::Slider::TextBoxRight, false, 100, 20);
    symmetryLinear.setTextBoxStyle(juce::Slider::TextBoxRight, false, 100, 20);
    totalGainsQuadratic.setTextBoxStyle(juce::Slider::TextBoxRight, false, 100, 20);
    totalGainsLinear.setTextBoxStyle(juce::Slider::TextBoxRight, false, 100, 20);

    energyLabel.attachToComponent(&energy, true);
    radialIntensityLabel.attachToComponent(&radialIntensity, true);
    pressureLabel.attachToComponent(&pressure, true);
    transverseVelocityLabel.attachToComponent(&transverseVelocity, true);
    radialVelocityLabel.attachToComponent(&radialVelocity, true);
    inPhaseQuadraticLabel.attachToComponent(&inPhaseQuadratic, true);
    inPhaseLinearLabel.attachToComponent(&inPhaseLinear, true);
    symmetryLinearLabel.attachToComponent(&symmetryLinear, true);
    totalGainsQuadraticLabel.attachToComponent(&totalGainsQuadratic, true);
    totalGainsLinearLabel.attachToComponent(&totalGainsLinear, true);
    
    energy.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    radialIntensity.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    pressure.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    transverseVelocity.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    radialVelocity.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    inPhaseQuadratic.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    inPhaseLinear.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    symmetryLinear.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    totalGainsLinear.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    totalGainsQuadratic.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    
    energy.setTextBoxIsEditable(true);
    radialIntensity.setTextBoxIsEditable(true);
    pressure.setTextBoxIsEditable(true);
    transverseVelocity.setTextBoxIsEditable(true);
    inPhaseQuadratic.setTextBoxIsEditable(true);
    inPhaseLinear.setTextBoxIsEditable(true);
    symmetryLinear.setTextBoxIsEditable(true);
    totalGainsLinear.setTextBoxIsEditable(true);
    totalGainsQuadratic.setTextBoxIsEditable(true);
    
}

ParameterPanel::~ParameterPanel()
{
}

void ParameterPanel::paint (juce::Graphics& g)
{

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (14.0f));
}

void ParameterPanel::resized()
{
    const float
    panelHeight             = getHeight(),
    panelWidth              = getWidth() * 0.6f,
    sliderHeight  = static_cast<float>(panelHeight) / 9.f;
    
    energy.setBounds(140, 0, panelWidth, sliderHeight);
    radialIntensity.setBounds(140, energy.getBottom(), panelWidth, sliderHeight);
    pressure.setBounds(140, radialIntensity.getBottom(), panelWidth, sliderHeight);
    transverseVelocity.setBounds(140, pressure.getBottom(), panelWidth, sliderHeight);
    inPhaseQuadratic.setBounds(140, transverseVelocity.getBottom(), panelWidth, sliderHeight);
    inPhaseLinear.setBounds(140, inPhaseQuadratic.getBottom(), panelWidth, sliderHeight);
    symmetryLinear.setBounds(140, inPhaseLinear.getBottom(), panelWidth, sliderHeight);
    totalGainsLinear.setBounds(140, symmetryLinear.getBottom(), panelWidth, sliderHeight);
    totalGainsQuadratic.setBounds(140, totalGainsLinear.getBottom(), panelWidth, sliderHeight);
    
    
}
