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
stateManager(s)
{
    addAndMakeVisible(editLayout);
    addAndMakeVisible(exportLayout);
    addAndMakeVisible(loadLayout);
    addAndMakeVisible(layoutName);
    
    editLayout.setButtonText("edit");
    exportLayout.setButtonText("export");
    loadLayout.setButtonText("import");
    
    layoutName.setEditable(true);
    layoutName.setJustificationType(juce::Justification::centred);
    
    if (formatType == UI::FormatType::input)
        layoutName.setText(ProcessingConstants::SpeakerProperties::inputTreeType,
                           juce::NotificationType::dontSendNotification);
    
    else if (formatType == UI::FormatType::output)
        layoutName.setText(ProcessingConstants::SpeakerProperties::outputTreeType,
                           juce::NotificationType::dontSendNotification);
    
    else
        jassertfalse;
    
    editLayout.addListener(this);
    exportLayout.addListener(this);
    loadLayout.addListener(this);
}

LayoutSelectorPanel::~LayoutSelectorPanel()
{
    delete layoutWindow;
}

void LayoutSelectorPanel::paint (juce::Graphics& g)
{

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (14.0f));
}

void LayoutSelectorPanel::resized()
{
    const float
    panelWidth      = getWidth(),
    panelHeight     = getHeight(),
    padding         = (panelWidth + panelHeight) * UI::SpeakerLayoutPanelFactors::paddingFactor,
    buttonWidth     = panelWidth / 3.f - 3 * padding,
    buttonHeight    = panelHeight / 3.f - 2 * padding;
    
    const float
    labelCentreX    = getLocalBounds().getCentreX(),
    labelY          = padding + panelHeight * UI::SpeakerLayoutPanelFactors::labelYPosFactor,
    labelWidth      = panelWidth * UI::SpeakerLayoutPanelFactors::labelWidthFactor,
    labelHeight     = panelHeight * UI::SpeakerLayoutPanelFactors::labelHeightFactor;
    
    layoutName.setBounds(0, 0, labelWidth, labelHeight);
    layoutName.setCentrePosition(labelCentreX, labelY);
    
    const float
    editX   = padding,
    editY   = getLocalBounds().getBottom() - buttonHeight;
    
    editLayout.setBounds(editX,
                         editY,
                         buttonWidth,
                         buttonHeight
                         );
    
    const float
    saveX = getLocalBounds().getCentreX() - buttonWidth / 2.f,
    saveY = editLayout.getY();
    
    exportLayout.setBounds(saveX,
                         saveY,
                         buttonWidth,
                         buttonHeight
                         );
    
    const float
    loadX = getLocalBounds().getRight() - buttonWidth - padding,
    loadY = editLayout.getY();
    
    loadLayout.setBounds(loadX,
                         loadY,
                         buttonWidth,
                         buttonHeight
                         );
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
        
            layoutWindow->centreWithSize(600, 400);
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
                stateManager.transcodingConfigHandler.speakerManagerInput
                    .saveCurrentLayoutToXML(fileToSave)
                :
                stateManager.transcodingConfigHandler.speakerManagerOutput
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
                    stateManager.transcodingConfigHandler.speakerManagerInput
                    .loadValueTreeFromXML(fileToLoad)
                    :
                    stateManager.transcodingConfigHandler.speakerManagerOutput
                    .loadValueTreeFromXML(fileToLoad);
            }

            else {
                DBG("Save dialog canceled by user.");
            }
        });
}
