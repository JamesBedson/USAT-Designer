/*
  ==============================================================================

    DecoderSettingsPanel.cpp
    Created: 11 Jan 2025 1:51:44pm
    Author:  James Bedson

  ==============================================================================
*/

#include <JuceHeader.h>
#include "DecodeSection.h"

//==============================================================================
DecoderSettingsPanel::DecoderSettingsPanel(USATAudioProcessor& p)
: audioProcessor(p),
sectionBackground(juce::ImageCache::getFromMemory(BinaryData::decode_section3x_png, BinaryData::decode_section3x_pngSize)),
stateManager(p.stateManager)
{
    addAndMakeVisible(decode);
    addAndMakeVisible(save);
    addAndMakeVisible(load);
    
    decode.setColour(juce::TextButton::textColourOnId, UI::ColourDefinitions::accentColour);
    decode.setColour(juce::TextButton::textColourOffId, UI::ColourDefinitions::accentColour);
    decode.setLookAndFeel(&decodeLNF);
    save.setLookAndFeel(&lookAndFeel);
    load.setLookAndFeel(&lookAndFeel);
    
    decode.addListener(this);
    save.addListener(this);
    load.addListener(this);
    
    decode.setButtonText("decode");
    save.setButtonText("save");
    load.setButtonText("load");
    
    decode.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    save.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    load.setMouseCursor(juce::MouseCursor::PointingHandCursor);

}

DecoderSettingsPanel::~DecoderSettingsPanel()
{
    decode.setLookAndFeel(nullptr);
    save.setLookAndFeel(nullptr);
    load.setLookAndFeel(nullptr);
}

void DecoderSettingsPanel::paint (juce::Graphics& g)
{
    g.drawImage(sectionBackground, getLocalBounds().toFloat(), juce::RectanglePlacement::stretchToFit);
}

void DecoderSettingsPanel::resized()
{
    auto reducedBounds = getLocalBounds().reduced(30.f, 20.f);
    
    const float
    panelWidth          = reducedBounds.getWidth(),
    panelHeight         = reducedBounds.getHeight(),
    decodeButtonWidth   = panelWidth * 0.2f,
    decodeButtonHeight  = panelHeight * 0.7f,
    xDecode             = reducedBounds.getX(),
    yDecode             = reducedBounds.getCentreY() - decodeButtonHeight  / 2.f;
    
    decode.setSize(decodeButtonWidth, decodeButtonHeight);
    decode.setTopLeftPosition(xDecode, yDecode);
    
    const float
    buttonPaddingX  = panelWidth * 0.03f,
    buttonWidth     = panelWidth * 0.15f,
    buttonHeight    = panelHeight * 0.5f;
    
    const float
    saveButtonX = decode.getRight() + buttonPaddingX,
    saveButtonY = reducedBounds.getCentreY() - buttonHeight / 2.f;
    
    save.setSize(buttonWidth, buttonHeight);
    save.setTopLeftPosition(saveButtonX, saveButtonY);
    
    const float
    loadButtonX = save.getRight() + buttonPaddingX,
    loadButtonY = saveButtonY;
    
    load.setSize(buttonWidth, buttonHeight);
    load.setTopLeftPosition(loadButtonX, loadButtonY);
}

void DecoderSettingsPanel::saveStateToXML()
{
    fileChooser = std::make_unique<juce::FileChooser>("Export decode settings as XML",
                                                      StateManager::transcodingsDirectory,
                                                      "*.xml",
                                                      true,
                                                      this);

    constexpr auto fileChooserFlags = juce::FileBrowserComponent::saveMode;

    fileChooser->launchAsync(
        fileChooserFlags,
        [this](const juce::FileChooser& chooser)
        {
            DBG("Exporting decode settings...");
            const juce::File chosenFile = chooser.getResult();

            if (chosenFile != juce::File{}) {
                juce::File fileToSave = chosenFile.hasFileExtension(".xml")
                                            ? chosenFile
                                            : chosenFile.withFileExtension(".xml");

                DBG("Chosen file: " + fileToSave.getFullPathName());
                stateManager.saveStateParametersToXML(fileToSave);
            }
            
            else {
                DBG("Export dialog canceled by user.");
            }
        });
}

void DecoderSettingsPanel::loadStateFromXML()
{
    fileChooser = std::make_unique<juce::FileChooser>("Choose a name",
                                                      StateManager::transcodingsDirectory,
                                                      "*.xml",
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
                
                stateManager.loadStateParametersFromXML(fileToLoad);
            }

            else {
                DBG("Save dialog canceled by user.");
            }
        });
}

void DecoderSettingsPanel::buttonClicked(juce::Button *button) {
    if (button == &decode) {
        
        const auto valueTree    = audioProcessor.stateManager.createParameterValueTree();
        auto file               = audioProcessor.stateManager.presetsDirectory.getChildFile("GlobalValueTree.xml");
        std::unique_ptr<juce::XmlElement> xml(valueTree.createXml());
        
        if (xml != nullptr) {
            xml->writeTo(file);
        }
        
        BlockingPopup* popup = new BlockingPopup(&audioProcessor);
        const float
        popupWidth  = getWidth() * 0.6f,
        popupHeight = getHeight() * 0.6f;
        
        popup->setSize(popupWidth, popupHeight);
        
        juce::DialogWindow::LaunchOptions options;
        options.content.setOwned(popup);
        options.dialogTitle                     = "USAT Designer";
        options.escapeKeyTriggersCloseButton    = false;
        options.useNativeTitleBar               = false;
        options.resizable                       = false;
        options.useBottomRightCornerResizer     = false;
        options.launchAsync();
        
        audioProcessor.decode();
    }
    
    else if (button == &save)
    {
        saveStateToXML();
        //stateManager.debugValueTree(stateManager.createGlobalStateTree());
    }
    
    else if (button == &load)
    {
        audioProcessor.matrixReadyAtomic.store(false);
        loadStateFromXML();
        //stateManager.debugValueTree(stateManager.createGlobalStateTree());
    }
}


