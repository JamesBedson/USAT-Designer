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
decodeSection(p),
formatSection(p.stateManager),
background(juce::ImageCache::getFromMemory(BinaryData::background3x_png, BinaryData::background3x_pngSize))
{
    addAndMakeVisible(header);
    addAndMakeVisible(controlSection);
    addAndMakeVisible(visualisationSection);
    addAndMakeVisible(decodeSection);
    addAndMakeVisible(formatSection);
    
}

RasterComponent::~RasterComponent()
{
    
}

void RasterComponent::paint (juce::Graphics& g)
{
    /*
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
    */
    g.drawImage(background, getLocalBounds().toFloat());
    
}

void RasterComponent::resized()
{
    
    juce::Rectangle<int> bounds = getBounds();
    const auto paddedBounds = bounds.reduced(20);

    const int
    windowWidth     = paddedBounds.getWidth(),
    windowHeight    = paddedBounds.getHeight(),
    windowX         = paddedBounds.getX(),
    windowY         = paddedBounds.getY();
    
    header.setBounds(windowX,
                     windowY,
                     windowWidth,
                     windowHeight * UI::RasterComponentFactors::headerHeightFactor
                     );
    
    const int
    bodyHeight = windowHeight - header.getHeight();
    
    const int
    formatX = windowX,
    formatY = header.getBottom();
    formatSection.setBounds(formatX,
                            formatY,
                            windowWidth * UI::RasterComponentFactors::bodyWidthFactor,
                            bodyHeight * UI::RasterComponentFactors::formatHeightFactor
                            );
    
    const int
    controlX = windowX,
    controlY = formatSection.getBottom();
    controlSection.setBounds(controlX,
                             controlY,
                             windowWidth * UI::RasterComponentFactors::bodyWidthFactor,
                             bodyHeight * UI::RasterComponentFactors::controlHeightFactor
                             );
    
    const int
    visPanelX = controlSection.getRight(),
    visPanelY = header.getBottom();
    visualisationSection.setBounds(visPanelX,
                                   visPanelY,
                                   windowWidth * UI::RasterComponentFactors::bodyWidthFactor,
                                   bodyHeight * UI::RasterComponentFactors::visPanelHeightFactor
                                   );
    
    const int
    decPanelX = visPanelX,
    decPanelY = visualisationSection.getBottom();
    
    decodeSection.setBounds(decPanelX,
                            decPanelY,
                            windowWidth * UI::RasterComponentFactors::bodyWidthFactor,
                            bodyHeight * UI::RasterComponentFactors::decPanelHeightFactor);
    
}
