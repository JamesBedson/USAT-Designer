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
        
    for (int i = 0; i < sliderPanels.size(); i++)
    {
        auto* sliderPanel = sliderPanels[i];
        addAndMakeVisible(sliderPanel);
        
        auto* slider        = &(sliderPanel->slider);
        auto* textLabel     = &(sliderPanel->textLabel);
        auto* valueLabel    = &(sliderPanel->valueLabel);
        
        const auto propertyName = ProcessingConstants::Coeffs::coefficientTypes[i];
        auto& coefficientsTree  = s.coefficientsTree;
        
        slider->onValueChange = [&, slider, propertyName]() {
            float value     = slider->getValue();
            stateManager.coefficientsTree.setProperty(propertyName,
                                        value,
                                        nullptr
                                        );
        };
        
        slider->setLookAndFeel(&lookAndFeel);
        slider->setValue(coefficientsTree.getProperty(propertyName));
        slider->setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 20);
        slider->setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
        slider->setTextBoxIsEditable(true);
        
        const auto labelText = labelTexts[i];
        textLabel->setText(labelText, juce::dontSendNotification);
        textLabel->setFont( juce::Font(juce::FontOptions {"Futura", 11.f, juce::Font::bold}));
        
        float value = slider->getValue();
        juce::String valueText(value, 2);
        valueLabel->setText(valueText, juce::dontSendNotification);
        
        valueLabel->setFont(juce::Font(juce::FontOptions {"Futura", 11.f, juce::Font::bold}));
    }
}


ParameterPanel::~ParameterPanel()
{
    for (auto* sliderPanel : sliderPanels) {
        auto* slider = &(sliderPanel->slider);
        slider->setLookAndFeel(nullptr);
    }
}

void ParameterPanel::paint (juce::Graphics& g)
{
    juce::Rectangle<int> reducedBounds = getLocalBounds();
    g.drawImage(juce::Image(juce::ImageCache::getFromMemory(BinaryData::placeholder3x_png,
                                                            BinaryData::placeholder3x_pngSize)),
                reducedBounds.toFloat());
    //g.setColour(UI::ColourDefinitions::accentColour);
    //g.drawRoundedRectangle(reducedBounds.toFloat(), 15.f, UI::Geometry::lineThickness);
    
}

void ParameterPanel::resized()
{
    const float
    panelHeight     = getHeight(),
    panelWidth      = getWidth() * 0.6f,
    sliderHeight    = static_cast<float>(panelHeight) / 9.f;
    
}

void ParameterPanel::setSimpleParameterPage() {
    for (auto* sliderPanel : sliderPanels) {
        sliderPanel->setVisible(false);
    }
}

void ParameterPanel::setAdvancedParameterPage(const ParameterSelectorChoice &advancedChoice)
{
    int mid = static_cast<int>(sliderPanels.size()) / 2;
    
    auto slidersFirstHalf   = std::vector<SliderPanel*>(sliderPanels.begin(),
                                                        sliderPanels.begin() + mid
                                                        );
    
    auto slidersSecondHalf  = std::vector<SliderPanel*>(sliderPanels.begin() + mid,
                                                        sliderPanels.end()
                                                        );
    
    std::vector<SliderPanel*> sliderPanelsToUse;
    
    if (advancedChoice == ParameterSelectorChoice::Advanced_1) {
        DBG("Selected first page");
        sliderPanelsToUse = slidersFirstHalf;
        for (int i = 0; i < slidersFirstHalf.size(); i++) {
            auto* activeSliderPanel         = slidersFirstHalf[i];
            auto* deactivatedSliderPanel    = slidersSecondHalf[i];
            
            activeSliderPanel->setVisible(true);
            deactivatedSliderPanel->setVisible(false);
        }
    }
    
    else if (advancedChoice == ParameterSelectorChoice::Advanced_2) {
        DBG("Selected second page");
        sliderPanelsToUse = slidersSecondHalf;
        for (int i = 0; i < slidersFirstHalf.size(); i++) {
            auto* activeSliderPanel         = slidersSecondHalf[i];
            auto* deactivatedSliderPanel    = slidersFirstHalf[i];

            activeSliderPanel->setVisible(true);
            deactivatedSliderPanel->setVisible(false);
        }
    }
    
    else
        jassertfalse;
    
    juce::Rectangle<int> reducedBounds = getLocalBounds().reduced(10.f);

    const int
    numCols = 3;
    
    const float
    startX      = (float) reducedBounds.getX(),
    startY      = (float) reducedBounds.getY(),
    totalWidth  = (float) reducedBounds.getWidth(),
    panelWidth  = totalWidth * UI::ParameterPanelFactors::sliderWidthFactor,
    panelHeight = (float) reducedBounds.getHeight() * UI::ParameterPanelFactors::sliderHeightFactor;

    const float
    colCenters[numCols] = {
        startX,
        static_cast<float>(reducedBounds.getCentreX()) - panelWidth * 0.5f,
        reducedBounds.getRight() - panelWidth
    };
    
    // Now layout sliders in pairs vertically
    for (int i = 0; i + 1 < slidersFirstHalf.size(); i += 2)
    {
        int col = static_cast<int>(i / 2);
        if (col >= numCols) break;  // just in case

        const float x = colCenters[col];
    
        sliderPanelsToUse[i]->setBounds(x,
                                startY,
                                panelWidth,
                                panelHeight);
        
        sliderPanelsToUse[i + 1]->setBounds(x,
                                           reducedBounds.getBottom() - panelHeight,
                                           panelWidth,
                                           panelHeight);
    }
}

void SliderPanel::resized() {
    
    auto reducedBounds = getLocalBounds();
    
    const float
    panelWidth      = reducedBounds.getWidth(),
    panelHeight     = reducedBounds.getHeight(),
    sliderWidth     = panelWidth * 0.5f,
    sliderHeight    = panelHeight,
    labelWidth      = panelWidth * 0.5f,
    labelHeight     = panelHeight * 0.5f;
    
    slider.setBounds(reducedBounds.getCentreX(),
                     reducedBounds.getY(),
                     sliderWidth,
                     sliderHeight
                     );

    textLabel.setBounds(reducedBounds.getX(),
                        reducedBounds.getY(),
                        labelWidth,
                        labelHeight
                        );
    
    valueLabel.setBounds(textLabel.getX(),
                         textLabel.getBottom(),
                         labelWidth,
                         labelHeight);
    
}

void SliderPanel::paint(juce::Graphics &g) {

}

void SliderPanel::sliderValueChanged(juce::Slider *s) {
    if (s == &slider) {
        float value = s->getValue();
        juce::String valueText(value, 2);
        valueLabel.setText(valueText, juce::dontSendNotification);
        //valueLabel.repaint();
    }
}
