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
background(juce::ImageCache::getFromMemory(BinaryData::background3x_png, BinaryData::background3x_pngSize)),
stateManager(s)
{
    if (formatType == UI::FormatType::input)
        speakerManager = &(stateManager.inputSpeakerManager);
    
    else if (formatType == UI::FormatType::output)
        speakerManager = &(stateManager.outputSpeakerManager);
    else
        jassertfalse;
    
    initialiseLFEState();
    initTable();
    
    
    addAndMakeVisible(addSpeaker);
    addSpeaker.addListener(this);
    addSpeaker.setButtonText("add speaker");
    addSpeaker.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    addSpeaker.setLookAndFeel(&lookAndFeel);
    
    table.setLookAndFeel(&lookAndFeel);
    table.setColour(juce::TableListBox::backgroundColourId, UI::ColourDefinitions::backgroundColour);
    table.setColour(juce::TableListBox::textColourId, UI::ColourDefinitions::outlineColour);
    table.setColour(juce::TableListBox::outlineColourId, UI::ColourDefinitions::outlineColour);
    
    auto& header = table.getHeader();
    header.setColour(juce::TableHeaderComponent::backgroundColourId, UI::ColourDefinitions::darkhighlightColour);
    header.setColour(juce::TableHeaderComponent::textColourId, UI::ColourDefinitions::outlineColour);
    header.setColour(juce::TableHeaderComponent::outlineColourId, juce::Colours::transparentBlack);
    header.setColour(juce::TableHeaderComponent::highlightColourId, UI::ColourDefinitions::accentColour);
    
    addAndMakeVisible(removeSpeaker);
    removeSpeaker.addListener(this);
    removeSpeaker.setButtonText("remove speaker");
    removeSpeaker.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    removeSpeaker.setLookAndFeel(&lookAndFeel);
    
}

SpeakerLayoutPanel::~SpeakerLayoutPanel()
{
    table.setLookAndFeel(nullptr);
    addSpeaker.setLookAndFeel(nullptr);
    removeSpeaker.setLookAndFeel(nullptr);
}

void SpeakerLayoutPanel::initTable()
{
    addAndMakeVisible(table);
    table.setModel(this);
    table.setColour(juce::ListBox::outlineColourId,
                    juce::Colours::white
                    );
    
    /*
     void TableHeaderComponent::addColumn (const String& columnName,
                                           int columnId,
                                           int width,
                                           int minimumWidth,
                                           int maximumWidth,
                                           int propertyFlags,
                                           int insertIndex)
     */
    table.getHeader().addColumn(ProcessingConstants::SpeakerProperties::tableChannelID,
                                1,
                                1
                                );
    
    table.getHeader().addColumn(ProcessingConstants::SpeakerProperties::tableAzimuth,
                                2,
                                1
                                );
    
    table.getHeader().addColumn(ProcessingConstants::SpeakerProperties::tableElevation,
                                3,
                                1
                                );
    
    table.getHeader().addColumn(ProcessingConstants::SpeakerProperties::tableDistance,
                                4,
                                1
                                );
    
    table.getHeader().addColumn(ProcessingConstants::SpeakerProperties::tableLFE,
                                5,
                                1);
    
    table.setMultipleSelectionEnabled(false);
    table.setHeaderHeight(30);
    table.updateContent();
}

void SpeakerLayoutPanel::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    const float
    xTable = bounds.getX();
    
    juce::Rectangle<float> buttonBounds;
    buttonBounds.setX(xTable);
    buttonBounds.setY(table.getBounds().getBottom());
    buttonBounds.setWidth(bounds.getWidth());
    buttonBounds.setHeight(bounds.getHeight() * 0.1f);
    
    g.setColour(UI::ColourDefinitions::darkhighlightColour.withLightness(UI::ColourDefinitions::lightnessFactor));
    g.fillRect(buttonBounds);
}

void SpeakerLayoutPanel::onLFEToggled(int row, bool isOn)
{
    if (isOn) {
        activeLFERow = row;
        
        for (int i = 0; i < getNumRows(); ++i) {
            bool lfeValue = (i == row);
            speakerManager->modifySpeakerProperty(i + 1, Speaker::Attributes::LFE, lfeValue ? 1.f : 0.f);
        }
    }
    
    else {
        activeLFERow = -1;
        
        for (int i = 0; i < getNumRows(); ++i) {
            speakerManager->modifySpeakerProperty(i + 1, Speaker::Attributes::LFE, 0.f);
        }
    }
    
    table.updateContent();
}


void SpeakerLayoutPanel::resized()
{
    auto bounds = getLocalBounds();
    const float
    xTable      = bounds.getX(),
    yTable      = bounds.getY(),
    tableHeight = bounds.getHeight() * 0.9f,
    tableWidth  = bounds.getWidth();
    
    table.setBounds(xTable, yTable, tableWidth, tableHeight);
    
    const float
    columnWidth = table.getWidth() / 5.f;
    
    for (int i = 1; i <= 5; i++)
        table.getHeader().setColumnWidth(i, columnWidth);
    
    juce::Rectangle<float> buttonBounds;
    buttonBounds.setX(xTable);
    buttonBounds.setY(table.getBounds().getBottom());
    buttonBounds.setWidth(bounds.getWidth());
    buttonBounds.setHeight(bounds.getHeight() * 0.1f);
    
    addSpeaker.setBounds(buttonBounds.getX(),
                         buttonBounds.getY(),
                         buttonBounds.getWidth() * 0.5f,
                         buttonBounds.getHeight());
    
    removeSpeaker.setBounds(addSpeaker.getRight(),
                            buttonBounds.getY(),
                            buttonBounds.getWidth() * 0.5f,
                            buttonBounds.getHeight());
    
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
    if (columnId == 1)
    {
        text << rowNumber + 1;
    }
    else
    {
        auto attribute = getSpeakerAttributeFromColumn(columnId);
        g.setFont(UI::Fonts::getMainFontWithSize(12.f));
        text = getText(columnId, rowNumber, attribute);
    }

    // Draw cell text
    g.setColour(UI::ColourDefinitions::outlineColour);
    g.setFont(UI::Fonts::getMainFontWithSize(12.f));
    g.drawText(text,
               2,
               0,
               width - 4,
               height,
               juce::Justification::left);
}

void SpeakerLayoutPanel::paintRowBackground(juce::Graphics& g,
                                            int rowNumber,
                                            int width,
                                            int height,
                                            bool rowIsSelected)
{
    // Fill background based on selection
    if (rowNumber == speakerIDSelected - 1) {
        g.fillAll(UI::ColourDefinitions::accentColour);
    } else {
        g.fillAll(UI::ColourDefinitions::darkhighlightColour.withLightness(UI::ColourDefinitions::lightnessFactor));
    }

    // Draw vertical lines between columns
    g.setColour(UI::ColourDefinitions::outlineColour.withAlpha(0.3f));
    /*
    int x = 0;
    for (int i = 1; i <= 3; ++i) // Draw 3 lines between 4 columns
    {
        int colWidth = table.getHeader().getColumnWidth(i);
        x += colWidth;
        g.drawLine((float)x, 0.0f, (float)x, (float)height);
    }
*/
    // Optional: Draw a horizontal line below each row
    g.drawLine(0.0f, (float)height - 1.0f, (float)width, (float)height - 1.0f);
}

void SpeakerLayoutPanel::buttonClicked(juce::Button* button)
{
    if (button == &addSpeaker) {
        float
        defaultAzimuth      = 0.f,
        defaultElevation    = 0.f,
        defaultDistance     = 1.f;
        
        bool defaultLFE = false;
        
        auto numSpeakers = getNumRows();
        auto newSpeaker = std::make_unique<Speaker>(defaultAzimuth,
                                                    defaultElevation,
                                                    defaultDistance,
                                                    defaultLFE
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
                                               const Speaker::Attributes coordinate)
{
    auto* currentSpeaker = speakerManager->getSpeaker(rowNumber + 1);
    if (coordinate == Speaker::Attributes::Azimuth) {
        auto azimuth = currentSpeaker->getAttribute(Speaker::Attributes::Azimuth);
        return juce::String {azimuth};
    }
    
    else if (coordinate == Speaker::Attributes::Elevation) {
        auto elevation = currentSpeaker->getAttribute(Speaker::Attributes::Elevation);
        return juce::String {elevation};
    }
    
    else if (coordinate == Speaker::Attributes::Distance) {
        auto distance = currentSpeaker->getAttribute(Speaker::Attributes::Distance);
        return juce::String {distance};
    }
    
    else {
        jassertfalse;
        return "";
    }
}

void SpeakerLayoutPanel::updateSpeakerState(int row, int columnID, float value)
{
    auto attributeType = getSpeakerAttributeFromColumn(columnID);
    speakerManager->modifySpeakerProperty(row + 1, attributeType, value);
}

const Speaker::Attributes SpeakerLayoutPanel::getSpeakerAttributeFromColumn(int columnID)
{
    return static_cast<Speaker::Attributes>(columnID - 2);
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
    
    else if (columnId == 5) // LFE
    {
        EditableToggleComponent* toggleButton = static_cast<EditableToggleComponent*>(existingComponentToUpdate);
        
        if (toggleButton == nullptr) {
            toggleButton = new EditableToggleComponent(*this);
        }
        
        toggleButton->setRow(rowNumber);
        auto currentSpeaker = speakerManager->getSpeaker(rowNumber + 1);

        toggleButton->setToggleState(currentSpeaker->getBoolAttribute(), juce::dontSendNotification);
        
        return toggleButton;
    }
    
    else {
        
        EditableTextComponent* textComponent = static_cast<EditableTextComponent*>(existingComponentToUpdate);
        
        if (textComponent == nullptr)
            textComponent = new EditableTextComponent(*this);
        
        textComponent->setRowAndColumn(rowNumber, columnId);
        
        auto coordinateType = getSpeakerAttributeFromColumn(columnId);
        auto currentSpeaker = speakerManager->getSpeaker(rowNumber + 1);
        auto initialText    = juce::String {currentSpeaker->getAttribute(coordinateType)};
        
        textComponent->setText(initialText, juce::dontSendNotification);
        return textComponent;
    }
}

void SpeakerLayoutPanel::initialiseLFEState() {
    
    activeLFERow = -1;

        for (int i = 0; i < getNumRows(); ++i)
        {
            if (speakerManager->getSpeaker(i + 1)->getBoolAttribute())
            {
                activeLFERow = i;
                break;
            }
        }
}
