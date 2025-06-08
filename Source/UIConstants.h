/*
  ==============================================================================

    UIConstants.h
    Created: 10 Jan 2025 6:07:36pm
    Author:  James Bedson

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>


namespace UI {

enum FormatType {
    input,
    output
};

namespace SizeLimits {

static constexpr double
maximumWidth {1000},
maximumHeight {600},
minimumWidth    = maximumWidth / 4.0,
minimumHeight   = maximumHeight / 4.0;

static constexpr float
margin          = maximumWidth * 0.01f;
}

namespace RasterComponentFactors {

static constexpr float
headerHeightFactor      = 0.1f,
bodyHeightFactor        = 1.f - headerHeightFactor,
bodyWidthFactor         = 0.5f,

formatHeightFactor      = 0.4f,
controlHeightFactor     = (1 - formatHeightFactor),

visPanelHeightFactor    = 0.8f,
decPanelHeightFactor    = (1 - visPanelHeightFactor);
}

namespace ControlSectionFactors {

static constexpr float
topSwitchHeightFactor       = 0.1f,
bottomSwitchHeightFactor    = 0.1f,
parameterPanelHeightFactor  = 1.f - topSwitchHeightFactor - bottomSwitchHeightFactor * 1.5f;
}

namespace ParameterPanelFactors {

static constexpr float
sliderWidthFactor   = 0.32f,
sliderHeightFactor  = 0.45f,
spacingFactorY      = 0.05f;
}

namespace VisualisationSectionFactors {

static constexpr float
paddingFactor = 0.05f;

}

namespace FormatSectionFactors {

static constexpr float
selectorWidthFactor = 0.5f;

}

namespace FormatPanelFactors {

static constexpr float
paddingFactor           = 0.02f,
comboBoxHeightFactor    = 0.2f;
}

namespace SpeakerLayoutPanelFactors {

static constexpr float
paddingFactor       = 0.01f,
tableWidthFactor    = 0.6f,
tableHeightFactor   = 0.9f,
labelYPosFactor     = 0.35f,
labelWidthFactor    = 0.8f,
labelHeightFactor   = 0.2f;

static constexpr int
idWidth         = 50,
azimuthWidth    = 50,
elevationWidth  = 50,
distanceWidth   = 50;

}

namespace AmbisonicsSelectorPanelFactors {

static constexpr float
paddingFactor           = 0.01f,
comboBoxHeightFactor    = 0.2f,
comboBoxWidthFactor     = 0.6f,
ordersWidthFactor       = 1.f - comboBoxWidthFactor,
ordersYPosFactor        = 0.35f;

}

namespace ColourDefinitions {

const juce::Colour
outlineColour   = juce::Colours::white,
accentColour    = juce::Colour(0xFFFB8B24),
backgroundColour = juce::Colour(0xFF0A0D0F),
darkhighlightColour = juce::Colour(0xFF15191F);

static constexpr float
lightnessFactor = 0.15f;

}

namespace Geometry {

const float
cornerRoundingFactor = 0.47f,
lineThickness = 1.f;

}

namespace Fonts {

inline const juce::Font getMainFont() {
    return juce::Font( juce::FontOptions {"Futura", 12.f, juce::Font::plain} );
}

inline const juce::Font getMainFontWithSize(const float fontSize) {
    return juce::Font( juce::FontOptions {"Futura", fontSize, juce::Font::plain} );
}

}

}

