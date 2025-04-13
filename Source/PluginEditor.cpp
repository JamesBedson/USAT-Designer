/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

// WRAPPER
//==============================================================================
WrappedAudioProcessorEditor::WrappedAudioProcessorEditor (USATAudioProcessor& p)
    : juce::AudioProcessorEditor(p),
    rasterComponent(p),
    audioProcessor (p)
{
    addAndMakeVisible(rasterComponent);
    if (auto constrainer = getConstrainer())
    {
        constrainer->setFixedAspectRatio(UI::SizeLimits::maximumWidth / UI::SizeLimits::maximumHeight);
        
        constrainer->setSizeLimits(UI::SizeLimits::minimumWidth,
                                   UI::SizeLimits::minimumHeight,
                                   UI::SizeLimits::maximumWidth,
                                   UI::SizeLimits::maximumHeight
                                   );
    }
    setResizable(true, true);
    setSize (UI::SizeLimits::maximumWidth, UI::SizeLimits::maximumHeight);
    
}

WrappedAudioProcessorEditor::~WrappedAudioProcessorEditor()
{
    
}

void WrappedAudioProcessorEditor::resized()
{
    const auto scaleFactor = static_cast<float>(getWidth()) / UI::SizeLimits::maximumWidth;
    rasterComponent.setTransform(juce::AffineTransform::scale(scaleFactor));
    rasterComponent.setBounds(0, 0, UI::SizeLimits::maximumWidth, UI::SizeLimits::maximumHeight);
    
}

// RASTER COMPONENT
//==============================================================================

RasterComponent::RasterComponent(USATAudioProcessor& p)
: audioProcessor(p),
header(p.stateManager),
controlSection(p.stateManager),
visualisationSection(p.stateManager),
decoderPanel(p)
{
    addAndMakeVisible(header);
    addAndMakeVisible(controlSection);
    addAndMakeVisible(visualisationSection);
    addAndMakeVisible(decoderPanel);
}

RasterComponent::~RasterComponent()
{
    
}

void RasterComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
    
}

void RasterComponent::resized()
{
    const float
    windowWidth     = getWidth(),
    windowHeight    = getHeight();
    header.setBounds(0, 0, windowWidth, windowHeight * UI::RasterComponentFactors::headerHeightFactor);
    
    const float
    controlX = header.getX(),
    controlY = header.getBottom();
    
    controlSection.setBounds(controlX,
                             controlY,
                             windowWidth * UI::RasterComponentFactors::controlWidthFactor,
                             windowHeight * UI::RasterComponentFactors::bodyHeightFactor
                             );
    
    const float
    visPanelX = controlSection.getRight(),
    visPanelY = header.getBottom();
    visualisationSection.setBounds(visPanelX,
                                   visPanelY,
                                   windowWidth * UI::RasterComponentFactors::visPanelWidthFactor,
                                   windowHeight * UI::RasterComponentFactors::bodyHeightFactor
                                   );
    
    const float
    decPanelX = controlSection.getX(),
    decPanelY = controlSection.getBottom();
    decoderPanel.setBounds(decPanelX,
                           decPanelY,
                           windowWidth,
                           windowHeight * UI::RasterComponentFactors::decPanelHeightFactor
                           );
}
