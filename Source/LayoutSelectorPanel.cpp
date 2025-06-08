/*
  ==============================================================================

    LayoutSelectorPanel.cpp
    Created: 12 Jan 2025 11:56:56am
    Author:  James Bedson

  ==============================================================================
*/

#include <JuceHeader.h>
#include "LayoutSelectorPanel.h"

//==============================================================================
LayoutSelectorPanel::LayoutSelectorPanel(StateManager& s,
                                         const UI::FormatType formatType)
: formatType(formatType),
stateManager(s),
background(juce::ImageCache::getFromMemory(BinaryData::formatSelectBackground3x_png, BinaryData::formatSelectBackground3x_pngSize))
{
    addAndMakeVisible(editLayout);
    addAndMakeVisible(exportLayout);
    addAndMakeVisible(loadLayout);
    
    addAndMakeVisible(layoutName);
    addAndMakeVisible(channelsLabel);
    addAndMakeVisible(formatLabel);
    
    layoutName.setFont(juce::Font(juce::FontOptions{"Futura", 15.f, juce::Font::italic}));
    layoutName.setMouseCursor(juce::MouseCursor::IBeamCursor);
    layoutName.setColour(juce::Label::backgroundColourId, UI::ColourDefinitions::darkhighlightColour.withLightness(UI::ColourDefinitions::lightnessFactor));
    layoutName.setColour(juce::Label::textColourId, UI::ColourDefinitions::outlineColour.darker());
    
    formatLabel.setFont(UI::Fonts::getMainFontWithSize(19.f));
    formatLabel.setJustificationType(juce::Justification::horizontallyCentred);
    formatLabel.setColour(juce::Label::textColourId, UI::ColourDefinitions::accentColour);
  
    setFormatText();
    
    editLayout.setButtonText("edit");
    exportLayout.setButtonText("export");
    loadLayout.setButtonText("import");
    
    editLayout.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    exportLayout.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    loadLayout.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    
    editLayout.setLookAndFeel(&lookAndFeel);
    exportLayout.setLookAndFeel(&lookAndFeel);
    loadLayout.setLookAndFeel(&lookAndFeel);
    
    layoutName.setEditable(true);
    layoutName.setJustificationType(juce::Justification::centred);
    
    if (formatType == UI::FormatType::input)
        layoutName.setText(ProcessingConstants::SpeakerProperties::inputSpeakerLayoutName,
                           juce::NotificationType::dontSendNotification);
    
    else if (formatType == UI::FormatType::output)
        layoutName.setText(ProcessingConstants::SpeakerProperties::outputSpeakerLayoutName,
                           juce::NotificationType::dontSendNotification);
    
    else
        jassertfalse;
    
    editLayout.addListener(this);
    exportLayout.addListener(this);
    loadLayout.addListener(this);
}

LayoutSelectorPanel::~LayoutSelectorPanel()
{
    editLayout.setLookAndFeel(nullptr);
    exportLayout.setLookAndFeel(nullptr);
    loadLayout.setLookAndFeel(nullptr);
    delete layoutWindow;
}

void LayoutSelectorPanel::paint (juce::Graphics& g)
{
    g.drawImage(background, getLocalBounds().toFloat());
}

void LayoutSelectorPanel::resized()
{
    auto reducedBounds = getLocalBounds().reduced(10.f, 10.f);
    
    const float
    panelWidth      = reducedBounds.getWidth(),
    panelHeight     = reducedBounds.getHeight(),
    padding         = (panelWidth + panelHeight) * UI::SpeakerLayoutPanelFactors::paddingFactor,
    buttonWidth     = panelWidth / 3.f - 3 * padding,
    componentHeight = panelHeight * UI::AmbisonicsSelectorPanelFactors::comboBoxHeightFactor;
    
    const float
    editX   = reducedBounds.getX(),
    editY   = reducedBounds.getBottom() - componentHeight;
    
    editLayout.setBounds(editX,
                         editY,
                         buttonWidth,
                         componentHeight
                         );
    
    const float
    saveX = reducedBounds.getCentreX() - buttonWidth / 2.f,
    saveY = editLayout.getY();
    
    exportLayout.setBounds(saveX,
                         saveY,
                         buttonWidth,
                         componentHeight
                         );
    
    const float
    loadX = reducedBounds.getRight() - buttonWidth,
    loadY = editLayout.getY();
    
    loadLayout.setBounds(loadX,
                         loadY,
                         buttonWidth,
                         componentHeight
                         );
    
    const float
    xLabels     = reducedBounds.getX(),
    yLabels     = reducedBounds.getY(),
    labelWidth  = reducedBounds.getWidth(),
    yOffset     = 15.f;
    
    formatLabel.setSize(labelWidth, componentHeight);
    formatLabel.setTopLeftPosition(xLabels, yLabels);
    
    const float
    centreX = reducedBounds.getCentreX(),
    centreY = reducedBounds.getCentreY();
    layoutName.setSize(labelWidth, componentHeight);
    layoutName.setCentrePosition(centreX, centreY);
    
}

void LayoutSelectorPanel::buttonClicked(juce::Button* button)
{
    if (button == &editLayout) {
        
        if (layoutWindow) {
            layoutWindow->broughtToFront();
        }
        else {
            SpeakerLayoutWindow* layoutWindow;
            
            if (formatType == UI::FormatType::input)
                layoutWindow = new SpeakerLayoutWindow("Input Speaker Layout",
                                                       stateManager,
                                                       formatType
                                                       );
            
            else if (formatType == UI::FormatType::output)
                layoutWindow = new SpeakerLayoutWindow("Output Speaker Layout",
                                                       stateManager,
                                                       formatType
                                                       );
            else
                jassertfalse;
        
            layoutWindow->centreWithSize(600, 500);
            layoutWindow->setVisible(true);
        }
    }
    
    else if (button == &exportLayout) {
        showExportDialog();
    }
    
    else if (button == &loadLayout) {
        showImportDialog();
        
        // TODO: Check if load was successful.
        if (layoutWindow != nullptr)
            layoutWindow.getComponent()->repaint();
    }
    
    else
        jassertfalse;
}

void LayoutSelectorPanel::showExportDialog()
{
    fileChooser = std::make_unique<juce::FileChooser>(
        "Choose a name",
        StateManager::speakerLayoutDirectory,
        "*.xml",
        true,
        this);

    constexpr auto fileChooserFlags = juce::FileBrowserComponent::saveMode;
    
    fileChooser->launchAsync(
        fileChooserFlags,
        [this] (const juce::FileChooser& chooser)
        {
            DBG("Exporting File...");
            const juce::File chosenFile = chooser.getResult();

            if (chosenFile != juce::File{}) {
                
                juce::File fileToSave = chosenFile.hasFileExtension(".xml")
                                            ? chosenFile
                                            : chosenFile.withFileExtension(".xml");

                DBG("Chosen file: " + fileToSave.getFullPathName());
                
                formatType == UI::FormatType::input ?
                stateManager.outputSpeakerManager
                    .saveCurrentLayoutToXML(fileToSave)
                :
                stateManager.outputSpeakerManager
                    .saveCurrentLayoutToXML(fileToSave);
            }
            
            else {
                DBG("Save dialog canceled by user.");
            }
        });
}

void LayoutSelectorPanel::showImportDialog() 
{
    fileChooser = std::make_unique<juce::FileChooser>(
        "Choose a name",
        StateManager::speakerLayoutDirectory,
        "*.xml", // <-- Corrected filter
        true,
        this
    );
    

    constexpr auto fileChooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

    fileChooser->launchAsync(
        fileChooserFlags,
        [this](const juce::FileChooser& chooser)
        {
            DBG("Importing File...");
            const juce::File chosenFile = chooser.getResult();

            if (chosenFile != juce::File{}) {

                juce::File fileToLoad = chosenFile.hasFileExtension(".xml")
                    ? chosenFile
                    : chosenFile.withFileExtension(".xml");

                DBG("Chosen file: " + fileToLoad.getFullPathName());

                formatType == UI::FormatType::input ?
                    stateManager.inputSpeakerManager
                    .loadValueTreeFromXML(fileToLoad)
                    :
                    stateManager.outputSpeakerManager
                    .loadValueTreeFromXML(fileToLoad);
            }

            else {
                DBG("Save dialog canceled by user.");
            }
        });
}
