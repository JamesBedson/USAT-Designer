/*
  ==============================================================================

    SpeakerLayoutPanel.cpp
    Created: 12 Jan 2025 2:21:51pm
    Author:  James Bedson

  ==============================================================================
*/

#include <JuceHeader.h>
#include "SpeakerLayoutPanel.h"

//==============================================================================
SpeakerLayoutPanel::SpeakerLayoutPanel(StateManager& s,
                                       UI::FormatType formatType)
: formatType(formatType),
stateManager(s)
{
    if (formatType == UI::FormatType::input)
        speakerManager = &(stateManager.transcodingConfigHandler.speakerManagerInput);
    
    else if (formatType == UI::FormatType::output)
        speakerManager = &(stateManager.transcodingConfigHandler.speakerManagerOutput);
    else
        jassertfalse;
    
    initTable();
    
    addAndMakeVisible(addSpeaker);
    addSpeaker.addListener(this);
    addSpeaker.setButtonText("Add Speaker");
    
    addAndMakeVisible(removeSpeaker);
    removeSpeaker.addListener(this);
    removeSpeaker.setButtonText("Remove Speaker");
}

SpeakerLayoutPanel::~SpeakerLayoutPanel()
{
}

void SpeakerLayoutPanel::initTable()
{
    addAndMakeVisible(table);
    table.setModel(this);
    table.setColour(juce::ListBox::outlineColourId,
                    juce::Colours::white
                    );
    
    table.getHeader().addColumn(ProcessingConstants::SpeakerProperties::ID,
                                1,
                                1
                                );
    
    table.getHeader().addColumn(ProcessingConstants::SpeakerProperties::azimuth,
                                2,
                                1
                                );
    
    table.getHeader().addColumn(ProcessingConstants::SpeakerProperties::elevation,
                                3,
                                1
                                );
    
    table.getHeader().addColumn(ProcessingConstants::SpeakerProperties::distance,
                                4,
                                1
                                );
    
    table.setMultipleSelectionEnabled(false);
    table.setHeaderHeight(30);
    table.updateContent();
}

void SpeakerLayoutPanel::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (14.0f));
    g.drawText ("SpeakerLayoutPanel", getLocalBounds(),
                juce::Justification::centred, true);   // draw some placeholder text
}

void SpeakerLayoutPanel::resized()
{
    auto bounds = getLocalBounds();
    bounds.removeFromRight(getWidth() * (1.f - UI::SpeakerLayoutPanelFactors::tableWidthFactor));
    bounds.removeFromBottom(getHeight() * (1.f - UI::SpeakerLayoutPanelFactors::tableHeightFactor));
    table.setBounds(bounds);
    
    const float
    columnWidth = table.getWidth() / 4.f;
    
    for (int i = 1; i <= 4; i++)
        table.getHeader().setColumnWidth(i, columnWidth);
    
    const float
    addSpeakerX     = table.getX(),
    addSpeakerY     = table.getBottom(),
    buttonWidth     = table.getWidth() / 2.f,
    buttonHeight    = getHeight() - table.getHeight();
    
    addSpeaker.setBounds(addSpeakerX,
                     addSpeakerY,
                     buttonWidth,
                     buttonHeight
                     );
    
    const float
    removeSpeakerX = addSpeaker.getRight(),
    removeSpeakerY = addSpeaker.getY();
    
    removeSpeaker.setBounds(removeSpeakerX,
                            removeSpeakerY,
                            buttonWidth,
                            buttonHeight
                            );
    
    speakerIDSelected == 0 ? removeSpeaker.setEnabled(false) : void();
}

int SpeakerLayoutPanel::getNumRows()
{
    return speakerManager->getSpeakerCount();
}

void SpeakerLayoutPanel::paintCell(juce::Graphics& g,
                                   int rowNumber,
                                   int columnId,
                                   int width,
                                   int height,
                                   bool rowIsSelected)
{
    juce::String text;
    if (columnId == 1) {
        text << rowNumber + 1;
    }
    
    else {
        auto coordinate = getSpeakerAttributeFromColumn(columnId);
        text            = getText(columnId, rowNumber, coordinate);
    }
    
    g.setColour(juce::Colours::white);
    g.drawText(text,
               2,
               0,
               width - 4,
               height,
               juce::Justification::centred);
    
    
}

void SpeakerLayoutPanel::paintRowBackground(juce::Graphics& g,
                                            int rowNumber,
                                            int width,
                                            int height,
                                            bool rowIsSelected)
{
    if (rowNumber == speakerIDSelected - 1) {
        DBG("Painting Selected colour for: " << rowNumber + 1);
        g.fillAll(juce::Colours::lightblue);
    }
        
    else {
        DBG("Updating Colour for non selected ID: " << rowNumber + 1);
        g.fillAll(juce::Colours::transparentBlack);
    }
}

void SpeakerLayoutPanel::buttonClicked(juce::Button* button)
{
    if (button == &addSpeaker) {
        int
        defaultAzimuth      = 0.f,
        defaultElevation    = 0.f,
        defaultDistance     = 1.f;
        
        auto numSpeakers = getNumRows();
        auto newSpeaker = std::make_unique<Speaker>(defaultAzimuth,
                                                    defaultElevation,
                                                    defaultDistance
                                                    );
        
        speakerManager->addSpeaker(std::move(newSpeaker),
                                   numSpeakers + 1
                                   );
        speakerIDSelected++;
        speakerIDSelected > 0 ? removeSpeaker.setEnabled(true) : void();
        table.updateContent();
    }
    
    else if (button == &removeSpeaker) {
        
        speakerManager->removeSpeaker(speakerIDSelected);
        speakerIDSelected = juce::jlimit(0, getNumRows(), speakerIDSelected);
        speakerIDSelected == 0 ? removeSpeaker.setEnabled(false) : void();
        table.updateContent();
    }
    
    else
        jassertfalse;
}

const juce::String SpeakerLayoutPanel::getText(const int columnNumber,
                                               const int rowNumber,
                                               const Speaker::SphericalCoordinates coordinate)
{
    auto* currentSpeaker = speakerManager->getSpeaker(rowNumber + 1);
    if (coordinate == Speaker::SphericalCoordinates::Azimuth) {
        auto azimuth = currentSpeaker->getCoordinate(Speaker::SphericalCoordinates::Azimuth);
        return juce::String {azimuth};
    }
    
    else if (coordinate == Speaker::SphericalCoordinates::Elevation) {
        auto elevation = currentSpeaker->getCoordinate(Speaker::SphericalCoordinates::Elevation);
        return juce::String {elevation};
    }
    
    else if (coordinate == Speaker::SphericalCoordinates::Distance) {
        auto distance = currentSpeaker->getCoordinate(Speaker::SphericalCoordinates::Distance);
        return juce::String {distance};
    }
    
    else {
        jassertfalse;
        return "";
    }
}

void SpeakerLayoutPanel::updateSpeakerState(int row, int columnID, float value)
{
    //auto speakerToUpdate    = speakerManager->getSpeaker(row + 1);
    auto coordinateType     = getSpeakerAttributeFromColumn(columnID);
    speakerManager->modifySpeakerProperty(row + 1, coordinateType, value);
    
}

const Speaker::SphericalCoordinates SpeakerLayoutPanel::getSpeakerAttributeFromColumn(int columnID)
{
    return static_cast<Speaker::SphericalCoordinates>(columnID - 2);
}

void SpeakerLayoutPanel::cellClicked(int rowNumber, int columnId, const juce::MouseEvent&)
{
    speakerIDSelected = rowNumber + 1;
    removeSpeaker.setEnabled(true);
    table.repaint();
}

void SpeakerLayoutPanel::backgroundClicked(const juce::MouseEvent&)
{
    speakerIDSelected = 0;
    removeSpeaker.setEnabled(false);
    table.repaint();
    //table.updateContent();
}

juce::Component* SpeakerLayoutPanel::refreshComponentForCell(int rowNumber,
                                                             int columnId,
                                                             bool isRowSelected,
                                                             juce::Component* existingComponentToUpdate)
{
    if (columnId == 1) // ID
    {
        jassert(existingComponentToUpdate == nullptr);
        return nullptr;
    }
    
    else {
        
        EditableTextComponent* textComponent = static_cast<EditableTextComponent*>(existingComponentToUpdate);
        
        if (textComponent == nullptr)
            textComponent = new EditableTextComponent(*this);
        
        textComponent->setRowAndColumn(rowNumber, columnId);
        
        auto coordinateType = getSpeakerAttributeFromColumn(columnId);
        auto currentSpeaker = speakerManager->getSpeaker(rowNumber + 1);
        auto initialText    = juce::String {currentSpeaker->getCoordinate(coordinateType)};
        
        textComponent->setText(initialText, juce::dontSendNotification);
        return textComponent;
    }
}
