/*
  ==============================================================================

    Header.cpp
    Created: 11 Jan 2025 1:21:19pm
    Author:  James Bedson

  ==============================================================================
*/

#include <JuceHeader.h>
#include "Header.h"

//==============================================================================
Header::Header(StateManager& s)
: stateManager(s)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

}

Header::~Header()
{
}

void Header::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (14.0f));
    g.drawText ("Header", getLocalBounds(),
                juce::Justification::centred, true);   // draw some placeholder text
}

void Header::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}
