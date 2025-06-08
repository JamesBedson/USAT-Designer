/*
  ==============================================================================

    VisualisationSection.cpp
    Created: 11 Jan 2025 1:22:20pm
    Author:  James Bedson

  ==============================================================================
*/

#include <JuceHeader.h>
#include "VisualisationSection.h"

//==============================================================================
VisualisationSection::VisualisationSection(StateManager& s)
: stateManager(s),
visPanelTop(s),
visPanelBottom(s),
sectionBackground(juce::ImageCache::getFromMemory(BinaryData::vis_section3x_png, BinaryData::vis_section3x_pngSize)),
panelBackground(juce::ImageCache::getFromMemory(BinaryData::placeholder3x_png, BinaryData::placeholder3x_pngSize)),
currentPlotSet(ImageToDisplay::energy)
{
    stateManager.signalPlots.addListener(this);
    
    addAndMakeVisible(visPanelTop);
    addAndMakeVisible(visPanelBottom);
    addAndMakeVisible(errorButton);
    addAndMakeVisible(energyButton);
    addAndMakeVisible(intensityButton);
    
    energyButton.addListener(this);
    intensityButton.addListener(this);
    errorButton.addListener(this);
    
    energyButton.setLookAndFeel(&lookAndFeel);
    energyButton.setButtonText("1");
    
    intensityButton.setLookAndFeel(&lookAndFeel);
    intensityButton.setButtonText("2");
    
    errorButton.setLookAndFeel(&lookAndFeel);
    errorButton.setButtonText("3");
    
    if (isPlotDataAvailable()) {
        loadPlots(false);
        currentPlotSet = VisualisationSection::ImageToDisplay::energy;
    }
    else {
        currentPlotSet = VisualisationSection::ImageToDisplay::placeholder;
    }
    
    setButtonToggleStates(currentPlotSet);
    setPanelImages();
}

VisualisationSection::~VisualisationSection()
{
    energyButton.setLookAndFeel(nullptr);
    intensityButton.setLookAndFeel(nullptr);
    errorButton.setLookAndFeel(nullptr);
}

void VisualisationSection::paint (juce::Graphics& g)
{
    g.drawImage(sectionBackground, getLocalBounds().toFloat(), juce::RectanglePlacement::stretchToFit);
}

void VisualisationSection::resized()
{
    const float
    sectionWidth    = getWidth(),
    sectionHeight   = getHeight(),
    padding         = sectionHeight * UI::VisualisationSectionFactors::paddingFactor,
    visPanelWidth   = sectionWidth - 2.f * padding,
    visPanelHeight  = sectionHeight / 2.f - 2.f * padding,
    buttonWidth     = sectionWidth * 0.1f,
    buttonHeight    = padding;
    
    const float
    xTopVisPanel    = padding,
    yTopVisPanel    = padding;
    
    visPanelTop.setBounds(xTopVisPanel,
                          yTopVisPanel,
                          visPanelWidth,
                          visPanelHeight
                          );
    
    const float
    xBottomVisPanel = padding,
    yBottomVisPanel = visPanelTop.getBottom() + padding;
    
    visPanelBottom.setBounds(xBottomVisPanel,
                             yBottomVisPanel,
                             visPanelWidth,
                             visPanelHeight
                             );
    
    const float
    xCentreIntensityButton  = getLocalBounds().getCentreX(),
    yIntensityButton        = visPanelBottom.getBottom() + padding * 0.9f,
    xPaddingBetweenButtons  = sectionWidth * 0.1f;
    
    intensityButton.setSize(buttonWidth, buttonHeight);
    intensityButton.setCentrePosition(xCentreIntensityButton, yIntensityButton);
    
    energyButton.setSize(buttonWidth, buttonHeight);
    energyButton.setTopRightPosition(intensityButton.getX() - xPaddingBetweenButtons, intensityButton.getY());
    
    errorButton.setSize(buttonWidth, buttonHeight);
    errorButton.setTopLeftPosition(intensityButton.getRight() + xPaddingBetweenButtons, intensityButton.getY());
}

void VisualisationSection::valueChanged(juce::Value &value)
{
    if (value.refersToSameSourceAs(stateManager.signalPlots))
    {
        if (static_cast<bool>(stateManager.signalPlots.getValue()) ==  true)
        {
            DBG("Updating plots");
            loadPlots(true);
            stateManager.signalPlots = false;
        }
    }
}

juce::Image VisualisationSection::decodeBase64ToImage(const juce::String& base64String)
{
    if (base64String.isEmpty()) {
        DBG("Base64 string is empty!");
        return juce::Image();
    }

    juce::MemoryOutputStream decodedDataStream;
    bool success = juce::Base64::convertFromBase64(decodedDataStream, base64String);
    
    if (success)
    {
        juce::MemoryBlock imageData = decodedDataStream.getMemoryBlock();
        juce::MemoryInputStream imageStream(imageData, false);
        juce::PNGImageFormat pngFormat;
        
        juce::Image plot = pngFormat.decodeImage(imageStream);
        
        return plot;
    }
    
    else {
        DBG("Error converting from base64!!");
        return juce::Image();
    }
}

bool VisualisationSection::isPlotDataAvailable() {
    if (stateManager.plotsTree.getNumChildren() > 0) return true;
    else return false;
}

void VisualisationSection::setButtonToggleStates(const VisualisationSection::ImageToDisplay currentPlot)
{
    switch (currentPlot) {
        case VisualisationSection::ImageToDisplay::energy:
            energyButton.setToggleState(true, juce::dontSendNotification);
            intensityButton.setToggleState(false, juce::dontSendNotification);
            errorButton.setToggleState(false, juce::dontSendNotification);
            break;
            
        case VisualisationSection::ImageToDisplay::intensity:
            energyButton.setToggleState(false, juce::dontSendNotification);
            intensityButton.setToggleState(true, juce::dontSendNotification);
            errorButton.setToggleState(false, juce::dontSendNotification);
            break;
            
        case VisualisationSection::ImageToDisplay::errorWidth:
            energyButton.setToggleState(false, juce::dontSendNotification);
            intensityButton.setToggleState(false, juce::dontSendNotification);
            errorButton.setToggleState(true, juce::dontSendNotification);
        
        case VisualisationSection::ImageToDisplay::placeholder:
            energyButton.setToggleState(true, juce::dontSendNotification);
            intensityButton.setToggleState(true, juce::dontSendNotification);
            errorButton.setToggleState(true, juce::dontSendNotification);
            break;
    }
}

void VisualisationSection::buttonClicked(juce::Button* button)
{
    bool plotDataAvailable = isPlotDataAvailable();
    
    if (button == &energyButton) {
        if (plotDataAvailable) {
            currentPlotSet = ImageToDisplay::energy;
            setButtonToggleStates(currentPlotSet);
            setPanelImages();
            visPanelTop.repaint();
            visPanelBottom.repaint();
        }
    }
    
    else if (button == &intensityButton) {
        if (plotDataAvailable) {
            currentPlotSet = ImageToDisplay::intensity;
            setButtonToggleStates(currentPlotSet);
            setPanelImages();
            visPanelTop.repaint();
            visPanelBottom.repaint();
        }
    }
    
    else if (button == &errorButton) {
        if (plotDataAvailable) {
            currentPlotSet = ImageToDisplay::errorWidth;
            setButtonToggleStates(currentPlotSet);
            setPanelImages();
            visPanelTop.repaint();
            visPanelBottom.repaint();
        }
    }
    
    else {
        jassertfalse;
    }
}

void VisualisationSection::setPanelImages()
{
    switch (currentPlotSet) {
        case VisualisationSection::ImageToDisplay::energy:
            visPanelTop.displayImage(energyPlot);
            visPanelBottom.displayImage(pressurePlot);
            break;
            
        case VisualisationSection::ImageToDisplay::intensity:
            visPanelTop.displayImage(radialIntensityPlot);
            visPanelBottom.displayImage(transverseIntensityPlot);
            break;
            
        case VisualisationSection::ImageToDisplay::errorWidth:
            visPanelTop.displayImage(angularErrorPlot);
            visPanelBottom.displayImage(sourceWidthPlot);
            break;
            
        case VisualisationSection::ImageToDisplay::placeholder:
            visPanelTop.displayImage(panelBackground);
            visPanelBottom.displayImage(panelBackground);
    }
}


void VisualisationSection::loadPlots(bool repaintPanels) {
    
    auto plots = stateManager.plotsTree.createCopy();
    
    juce::String energyPlotBase64               = plots.getChildWithName(ProcessingConstants::TreeTags::energyPlotID).getProperty(ProcessingConstants::TreeTags::plotData);
    
    juce::String pressurePlotBase64             = plots.getChildWithName(ProcessingConstants::TreeTags::pressurePlotID).getProperty(ProcessingConstants::TreeTags::plotData);
    
    juce::String radialIntensityPlotBase64      = plots.getChildWithName(ProcessingConstants::TreeTags::radialIntensityPlotID).getProperty(ProcessingConstants::TreeTags::plotData);
    
    juce::String transverseIntensityPlotsBase64 = plots.getChildWithName(ProcessingConstants::TreeTags::transverseIntensityPlotID).getProperty(ProcessingConstants::TreeTags::plotData);
    
    juce::String angularErrorPlotBase64         = plots.getChildWithName(ProcessingConstants::TreeTags::angularErrorPlotID).getProperty(ProcessingConstants::TreeTags::plotData);
    
    juce::String sourceWidthPlotBase64          = plots.getChildWithName(ProcessingConstants::TreeTags::sourceWidthPlotID).getProperty(ProcessingConstants::TreeTags::plotData);
    
    energyPlot              = decodeBase64ToImage(energyPlotBase64);
    pressurePlot            = decodeBase64ToImage(pressurePlotBase64);
    radialIntensityPlot     = decodeBase64ToImage(radialIntensityPlotBase64);
    transverseIntensityPlot = decodeBase64ToImage(transverseIntensityPlotsBase64);
    angularErrorPlot        = decodeBase64ToImage(angularErrorPlotBase64);
    sourceWidthPlot         = decodeBase64ToImage(sourceWidthPlotBase64);
    
    currentPlotSet = VisualisationSection::ImageToDisplay::energy;
    setButtonToggleStates(currentPlotSet);
    
    setPanelImages();
    
    if (repaintPanels) {
        visPanelTop.repaint();
        visPanelBottom.repaint();
    }
}
