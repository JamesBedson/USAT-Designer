/*
  ==============================================================================

    ControlSection.cpp
    Created: 11 Jan 2025 1:22:02pm
    Author:  James Bedson

  ==B============================================================================
*/

#include <JuceHeader.h>
#include "ControlSection.h"

//==============================================================================
ControlSection::ControlSection(StateManager& s)
: stateManager(s),
parameterPanel(s),
sectionBackground(juce::ImageCache::getFromMemory(BinaryData::parameter_section3x_png, BinaryData::parameter_section3x_pngSize))
{
    addAndMakeVisible(parameterPanel);
    addAndMakeVisible(parameterTypeSwitch);
    addAndMakeVisible(parameterStyleSwitch);
    addAndMakeVisible(firstPageButton);
    addAndMakeVisible(secondPageButton);
    addAndMakeVisible(simple);
    addAndMakeVisible(advanced);
    
    firstPageButton.setLookAndFeel(&lookAndFeel);
    secondPageButton.setLookAndFeel(&lookAndFeel);
    
    firstPageButton.setButtonText("1");
    secondPageButton.setButtonText("2");
    
    parameterStyleSwitch.setClickingTogglesState(true);
    parameterStyleSwitch.setToggleState(true, juce::dontSendNotification);
    parameterStyleSwitch.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    
    firstPageButton.setEnabled(false);
    secondPageButton.setEnabled(true);
    
    
    // Preload images
    auto advancedImg = juce::ImageCache::getFromMemory(BinaryData::advanced3x_png,
                                                    BinaryData::advanced3x_pngSize);
    
    auto advancedHighlightedImg = juce::ImageCache::getFromMemory(BinaryData::advanced_highlighted3x_png,
                                                               BinaryData::advanced_highlighted3x_pngSize);
    
    auto simpleImg = juce::ImageCache::getFromMemory(BinaryData::simple3x_png,
                                                  BinaryData::simple3x_pngSize);
    
    auto simpleHighlightedImg = juce::ImageCache::getFromMemory(BinaryData::simple_highlighted3x_png,
                                                             BinaryData::simple_highlighted3x_pngSize);

    /*
     void ImageButton::setImages (const bool resizeButtonNowToFitThisImage,
                                  const bool rescaleImagesWhenButtonSizeChanges,
                                  const bool preserveImageProportions,
                                  const Image& normalImage_,
                                  const float imageOpacityWhenNormal,
                                  Colour overlayColourWhenNormal,
                                  const Image& overImage_,
                                  const float imageOpacityWhenOver,
                                  Colour overlayColourWhenOver,
                                  const Image& downImage_,
                                  const float imageOpacityWhenDown,
                                  Colour overlayColourWhenDown,
                                  const float hitTestAlphaThreshold)
     */
    parameterStyleSwitch.setImages(
        true,  // resize button to fit the image
        true,  // rescale images when button size changes
        true,  // preserve image proportions

        simpleImg, 1.f, juce::Colours::transparentBlack, // normal
        juce::Image(), 1.f, juce::Colours::transparentBlack, // hover (same as normal)
        advancedImg,   1.f, juce::Colours::transparentBlack, // down

        0.0f // hit test alpha threshold (0 = no transparency-based hit testing)
    );

    
    simple.setText("simple", juce::dontSendNotification);
    simple.setFont(juce::Font(juce::FontOptions {"Futura", 15.f, juce::Font::plain}));
    simple.setJustificationType(juce::Justification::centredRight);
    
    advanced.setText("advanced", juce::dontSendNotification);
    advanced.setFont(juce::Font(juce::FontOptions {"Futura", 15.f, juce::Font::plain}));
    advanced.setJustificationType(juce::Justification::centredLeft);
    
    firstPageButton.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    secondPageButton.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    
    firstPageButton.onClick = [&]() {
        parameterPanel.setAdvancedParameterPage(ParameterPanel::ParameterSelectorChoice::Advanced_1);
        secondPageButton.setEnabled(true);
        firstPageButton.setEnabled(false);
    };
    
    secondPageButton.onClick = [&]() {
        parameterPanel.setAdvancedParameterPage(ParameterPanel::ParameterSelectorChoice::Advanced_2);
        firstPageButton.setEnabled(true);
        secondPageButton.setEnabled(false);
    };
    
    parameterStyleSwitch.onClick = [&]() {
        auto advancedModeActive = parameterStyleSwitch.getToggleState();
        
        if (advancedModeActive) {
            firstPageButton.setEnabled(true);
            firstPageButton.setVisible(true);
            
            secondPageButton.setEnabled(true);
            secondPageButton.setVisible(true);
            
            parameterPanel.setAdvancedParameterPage(ParameterPanel::ParameterSelectorChoice::Advanced_1);
        }
        
        else {
            firstPageButton.setEnabled(false);
            firstPageButton.setVisible(false);
            
            secondPageButton.setEnabled(false);
            secondPageButton.setVisible(false);
            
            parameterPanel.setSimpleParameterPage();
        }
    };
    
}

ControlSection::~ControlSection()
{
    firstPageButton.setLookAndFeel(nullptr);
    secondPageButton.setLookAndFeel(nullptr);
}

void ControlSection::paint (juce::Graphics& g)
{
    g.drawImage(sectionBackground, getLocalBounds().toFloat());
    g.setColour(juce::Colours::white);
    g.drawLine(simple.getRight() + 10.f,
               simple.getBounds().getCentreY(),
               parameterStyleSwitch.getX() - 10.f,
               parameterStyleSwitch.getBounds().getCentreY(), 1.f);
    
    g.drawLine(advanced.getX() - 10.f,
               advanced.getBounds().getCentreY(),
               parameterStyleSwitch.getRight() + 10.f,
               parameterStyleSwitch.getBounds().getCentreY(), 1.f);
    /*
    juce::Rectangle<int> toggleButtonBounds {
        getLocalBounds().getX(),
        getLocalBounds().getY(),
        getWidth(),
        static_cast<int>(getHeight() * UI::ControlSectionFactors::topSwitchHeightFactor)
    };
    
    const int
    paramsX         = getLocalBounds().getX(),
    paramsY         = parameterTypeSwitch.getBottom(),
    paramsHeight    = getHeight() * UI::ControlSectionFactors::parameterPanelHeightFactor;
    
    const juce::Rectangle<int> parameterBounds {
        paramsX,
        paramsY,
        getWidth(),
        paramsHeight
    };
    
    const int
    x = getLocalBounds().getX(),
    arrowsBoundsHeight = getHeight() * UI::ControlSectionFactors::bottomSwitchHeightFactor;
    
    const juce::Rectangle<int> arrowPanelBounds {
        x,
        parameterBounds.getBottom(),
        getWidth(),
        arrowsBoundsHeight
    };
    
    g.drawRect(arrowPanelBounds);
    g.drawRect(toggleButtonBounds);
    g.drawRect(parameterBounds);
     */
}

void ControlSection::resized()
{
    const int
    panelWidth      = getWidth(),
    panelHeight     = getHeight(),
    panelCentreX    = getLocalBounds().getCentreX();
    
    const float
    toggleButtonWidth   = 50.f * 0.7f,
    toggleButtonHeight  = panelHeight * UI::ControlSectionFactors::topSwitchHeightFactor * 0.7f;
    
    juce::Rectangle<int> toggleButtonBounds {
        getLocalBounds().getX(),
        static_cast<int>(getLocalBounds().getY() + 14.f),
        getWidth(),
        static_cast<int>(panelHeight * UI::ControlSectionFactors::topSwitchHeightFactor)
    };
    
    parameterStyleSwitch.setSize(toggleButtonWidth, toggleButtonHeight);
    parameterStyleSwitch.setCentrePosition(toggleButtonBounds.getCentreX(),
                                          toggleButtonBounds.getCentreY());
    
    const float
    textWidth = 100.f;
    simple.setSize(textWidth, toggleButtonHeight);
    simple.setTopRightPosition(parameterStyleSwitch.getX() - 30.f, parameterStyleSwitch.getY());
    
    advanced.setSize(textWidth, toggleButtonHeight);
    advanced.setTopLeftPosition(parameterStyleSwitch.getRight() + 30.f, parameterStyleSwitch.getY());

    // PARAMETER PANEL ========================================================================
    const int
    paramsX         = getLocalBounds().getX(),
    paramsY         = parameterStyleSwitch.getBottom(),
    paramsHeight    = panelHeight * UI::ControlSectionFactors::parameterPanelHeightFactor;
    
    const juce::Rectangle<int> parameterBounds {
        paramsX,
        paramsY,
        panelWidth,
        paramsHeight
    };
    
    const auto paddedBounds = parameterBounds.reduced(20.f, 10.f);
    parameterPanel.setBounds(paddedBounds);
    
    // ARROW PANEL ============================================================================
    
    const float
    leftArrowRightCoord = panelCentreX - 15.f,
    rightArrowLeftCoord = panelCentreX + 15.f,
    arrowButtonWidth    = 50.f;
    
    const int
    arrowsBoundsHeight = panelHeight * UI::ControlSectionFactors::bottomSwitchHeightFactor;
    
    const juce::Rectangle<int> arrowPanelBounds {
        getLocalBounds().getX(),
        static_cast<int>(parameterBounds.getBottom() - 2.5f),
        panelWidth,
        arrowsBoundsHeight
    };
    
    const float
    arrowButtonY        = arrowPanelBounds.getY(),
    arrowButtonHeight   = arrowPanelBounds.getHeight() * 0.7f;
    
    firstPageButton.setSize(arrowButtonWidth, arrowButtonHeight);
    firstPageButton.setTopRightPosition(leftArrowRightCoord, arrowButtonY);
    
    secondPageButton.setSize(arrowButtonWidth, arrowButtonHeight);
    secondPageButton.setTopLeftPosition(rightArrowLeftCoord, arrowButtonY);
    
    parameterStyleSwitch.setToggleState(true, juce::dontSendNotification);
    parameterPanel.setAdvancedParameterPage(ParameterPanel::ParameterSelectorChoice::Advanced_1);
    
}
