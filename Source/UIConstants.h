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
maximumWidth {1200.0},
maximumHeight {800.0},
minimumWidth    = maximumWidth / 4.0,
minimumHeight   = maximumHeight / 4.0;

static constexpr float
margin          = maximumWidth * 0.01f;
}

namespace RasterComponentFactors {

static constexpr float

headerHeightFactor          = 0.1f,
bodyHeightFactor            = 0.7f,
decPanelHeightFactor        = 1.f - headerHeightFactor - bodyHeightFactor,
controlWidthFactor          = 0.45f,
visPanelWidthFactor         = 1.f - controlWidthFactor;

}

namespace ControlSectionFactors {

static constexpr float
formatWidthFactor   = 0.5f,
formatHeightFactor  = 0.3f,
paramsHeightFactor  = 1.f - formatHeightFactor;

}

namespace VisualisationSectionFactors {

static constexpr float
paddingFactor = 0.05f;

}

namespace FormatSelectionFactors {

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
comboBoxWidthFactor     = 0.8f,
ordersYPosFactor        = 0.35f;

}

}
