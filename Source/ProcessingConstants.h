/*
  ==============================================================================

    ProcessingConstants.h
    Created: 20 Nov 2024 8:50:24pm
    Author:  James Bedson

  ==============================================================================
*/

#pragma once
namespace ProcessingConstants {

    namespace Paths {
    const juce::String scriptsDirectory         = ".Scripts";
    const juce::String resourceDirectory        = "Resources";
    const juce::String universalTranscoderPyDir = "universal_transcoder";
    const juce::String usatDesignerPyDir        = "usat_designer";
    const juce::String notebooksDir             = "notebooks";
    const juce::String analysisDir              = "analysis";
    const juce::String focusDir                 = "usat_state_parameters";
    const juce::String focusLow                 = "focus_low";
    const juce::String focusMid                 = "focus_mid";
    const juce::String focusHigh                = "focus_high";
    const juce::String pythonDir                = "python";
    const juce::String sitePackagesDir          = "site-packages";
    const juce::String versionsDir              = "versions";
    const juce::String pythonVersion            = "3.11.1";
    const juce::String speakerLayoutDir         = "Speaker_Layouts";
    const juce::String transcodingsDir          = "USAT_Transcodings";
    }

    namespace SpeakerProperties {
    
    const juce::String ID                       = "ID";
    const juce::String azimuth                  = "Azimuth";
    const juce::String elevation                = "Elevation";
    const juce::String distance                 = "Distance";
    const juce::String isLFE                    = "IsLFE";
    const juce::String inputSpeakerLayoutName   = "Input Layout";
    const juce::String outputSpeakerLayoutName  = "Output Layout";
    
    // Table Column Names
    const juce::String tableChannelID   = "channel";
    const juce::String tableAzimuth     = "azimuth";
    const juce::String tableElevation   = "elevation";
    const juce::String tableDistance    = "distance";
    const juce::String tableLFE         = "LFE";
    }

    namespace TreeTags {
    
    const juce::String mainStateID              = "USAT_Designer";
    
    // USAT State Parameters
    const juce::String stateParametersID        = "USAT_State_Parameters";
    const juce::String settingsID               = "Settings";
    const juce::String inputAmbisonicsID        = "Input_Ambisonics";
    const juce::String outputAmbisonicsID       = "Output_Ambisonics";
    const juce::String inputSpeakerLayoutID     = "Input_Loudspeaker_Layout";
    const juce::String outputSpeakerLayoutID    = "Output_Loudspeaker_Layout";
    const juce::String coefficientsID           = "Coefficients";
    
    // Gain Matrix
    const juce::String gainMatrixID             = "Gain_Matrix";
    const juce::String channelCountsID          = "Channel_Counts";
    const juce::String LFEChannelIndices        = "LFE_Channel_Indices";
    const juce::String matrixCoefficientsID     = "Matrix_Coefficients";
    
    // Plots
    const juce::String allPlotsID                   = "Plots";
    const juce::String energyPlotID                 = "EnergyPlot";
    const juce::String pressurePlotID               = "PressurePlot";
    const juce::String radialIntensityPlotID        = "RadialIntensityPlot";
    const juce::String transverseIntensityPlotID    = "TransverseIntensityPlot";
    const juce::String angularErrorPlotID           = "AngularErrorPlot";
    const juce::String sourceWidthPlotID            = "SourceWidthPlot";
    
    const juce::String plotData = "PlotData";
    }

    namespace GainMatrixTree {
    
        namespace ChannelCount {
        
        const juce::String inputChannelCount    = "Input_Channel_Count";
        const juce::String outputChannelCount   = "Output_Channel_Count";
        
        }
    
        namespace MatrixCoefficient {
        
        const juce::String baseCoefficientID    = "T";
        inline const juce::String getCoefficientID(const int chIn, const int chOut)
        {
            juce::String baseName = "T";
            baseName << chIn;
            baseName << chOut;
            return baseName;
        }
        }
    
        namespace LFEIndices {
        
        const juce::String inputLFEChannelIndex     = "Input_LFE_Channel_Index";
        const juce::String outputLFEChannelIndex    = "Output_LFE_Channel_index";
        
        }
        
    
    }

    namespace ParameterNames {
        
    const juce::String bypass   = "bypass";
    const juce::String focus    = "focus";
    
        namespace Focus {
        const juce::String low  = "low";
        const juce::String mid  = "mid";
        const juce::String high = "high";
        }
    
    }

    namespace EncodingOptions {
    
    const juce::String inputType    = "InputType";
    const juce::String outputType   = "OutputType";
    
    const juce::String ambisonics       = "Ambisonics";
    const juce::String speakerLayout    = "SpeakerLayout";
    
    // EncodingOptions
    const juce::StringArray encodingChoices {
        ProcessingConstants::EncodingOptions::ambisonics,
        ProcessingConstants::EncodingOptions::speakerLayout
    };
    
        namespace Ambisonics {
        const juce::String orderIn  = "AmbisonicsOrderIn";
        const juce::String orderOut = "AmbisonicsOrderOut";
        const juce::StringArray orderChoices {
                "1", "2", "3", "4", "5"
            };
        }
    
        namespace Surround {
            const juce::String surroundLayout   = "SurroundLayout";
            const juce::StringArray layoutChoices {
                "5.1.4",
                "7.1.4",
                "9.1.6"
            };
        }
    }
    
    namespace Coeffs {
    
    // ================================================================
    // Names
    
    const juce::String  energy                  = "energy";
    const juce::String  radialIntensity         = "radialIntensity";
    const juce::String  transverseIntensity     = "transverseIntensity";
    const juce::String  pressure                = "pressure";
    const juce::String  radialVelocity          = "radialVelocity";
    const juce::String  transverseVelocity      = "transverseVelocity";
    const juce::String  inPhaseQuadratic        = "inPhaseQuadratic";
    const juce::String  symmetryQuadratic       = "symmetryQuadratic";
    const juce::String  inPhaseLinear           = "inPhaseLinear";
    const juce::String  symmetryLinear          = "symmetryLinear";
    const juce::String  totalGainsLinear        = "totalGainsLinear";
    const juce::String  totalGainsQuadratic     = "totalGainsQuadratic";
    
    const juce::StringArray coefficientTypes {
        
        energy,
        radialIntensity,
        transverseIntensity,
        pressure,
        radialVelocity,
        transverseVelocity,
        inPhaseQuadratic,
        symmetryQuadratic,
        inPhaseLinear,
        symmetryLinear,
        totalGainsLinear,
        totalGainsQuadratic
        
    };
    
    // ================================================================
    // Values
    
    
    // energy
    constexpr float startValEnergy                  {0.1f};
    constexpr float endValEnergy                    {10.f};
    constexpr float incrementEnergy                 {0.1f};
    constexpr float defaultEnergy                   {5.f};
    constexpr float skewEnergy                      {1.f};
    
    // radialIntensity
    constexpr float startValRadialIntensity         {0.1f};
    constexpr float endValRadialIntensity           {10.f};
    constexpr float incrementRadialIntensity        {0.1f};
    constexpr float defaultRadialIntensity          {5.f};
    constexpr float skewRadialIntensity             {1.f};
    
    // transverseIntensity
    constexpr float startValTransverseIntensity     {0.1f};
    constexpr float endValTransverseIntensity       {10.f};
    constexpr float defaultTransverseIntensity      {5.f};
    constexpr float incrementTransverseIntensity    {0.1f};
    constexpr float skewTransverseIntensity         {1.f};
    
    // pressure
    constexpr float startValPressure                {0.1f};
    constexpr float endValPressure                  {10.f};
    constexpr float defaultPressure                 {5.f};
    constexpr float incrementPressure               {0.1f};
    constexpr float skewPressure                    {1.f};
    
    // radialVelocity
    constexpr float startValRadialVelocity          {0.1f};
    constexpr float endValRadialVelocity            {10.f};
    constexpr float defaultRadialVelocity           {5.f};
    constexpr float incrementRadialVelocity         {0.1f};
    constexpr float skewRadialVelocity              {1.f};
    
    // transverseVelocity
    constexpr float startValTransverseVelocity      {0.1f};
    constexpr float endValTransverseVelocity        {10.f};
    constexpr float defaultTransverseVelocity       {5.f};
    constexpr float incrementTransverseVelocity     {0.1f};
    constexpr float skewTransverseVelocity          {1.f};

    // inPhaseQuadratic
    constexpr float startValInPhaseQuadratic        {0.1f};
    constexpr float endValInPhaseQuadratic          {10.f};
    constexpr float defaultInPhaseQuadratic         {5.f};
    constexpr float incrementInPhaseQuadratic       {0.1f};
    constexpr float skewInPhaseQuadratic            {1.f};

    // symmetryQuadratic
    constexpr float startValSymmetryQuadratic       {0.1f};
    constexpr float endValSymmetryQuadratic         {10.f};
    constexpr float defaultSymmetryQuadratic        {5.f};
    constexpr float incrementSymmetryQuadratic      {0.1f};
    constexpr float skewSymmetryQuadratic           {1.f};

    // inPhaseLinear
    constexpr float startValInPhaseLinear           {0.1f};
    constexpr float endValInPhaseLinear             {10.f};
    constexpr float defaultInPhaseLinear            {5.f};
    constexpr float incrementInPhaseLinear          {0.1f};
    constexpr float skewInPhaseLinear               {1.f};

    // symmetryLinear
    constexpr float startValSymmetryLinear          {0.1f};
    constexpr float endValSymmetryLinear            {10.f};
    constexpr float defaultSymmetryLinear           {5.f};
    constexpr float incrementSymmetryLinear         {0.1f};
    constexpr float skewSymmetryLinear              {1.f};

    // totalGainsLinear
    constexpr float startValTotalGainsLinear        {0.1f};
    constexpr float endValTotalGainsLinear          {10.f};
    constexpr float defaultTotalGainsLinear         {5.f};
    constexpr float incrementTotalGainsLinear       {0.1f};
    constexpr float skewTotalGainsLinear            {1.f};

    // totalGainsQuadratic
    constexpr float startValTotalGainsQuadratic     {0.1f};
    constexpr float endValTotalGainsQuadratic       {10.f};
    constexpr float defaultTotalGainsQuadratic      {5.f};
    constexpr float incrementTotalGainsQuadratic    {0.1f};
    constexpr float skewTotalGainsQuadratic         {1.f};
    
    
    const std::unordered_map<juce::String, float> defaultValues
    {
        {energy, defaultEnergy},
        {radialIntensity, defaultRadialIntensity},
        {transverseIntensity, defaultTransverseIntensity},
        {pressure, defaultPressure},
        {radialVelocity, defaultRadialVelocity},
        {transverseVelocity, defaultTransverseVelocity},
        {inPhaseQuadratic, defaultInPhaseQuadratic},
        {symmetryQuadratic, defaultInPhaseQuadratic},
        {inPhaseLinear, defaultInPhaseLinear},
        {symmetryLinear, defaultSymmetryLinear},
        {totalGainsLinear, defaultTotalGainsLinear},
        {totalGainsQuadratic, defaultTotalGainsQuadratic}
    };
    
    }

    class PythonParameterNameMap {

    public:
        
        const std::map<juce::String, juce::String>  encodingOptionMap = {
            { EncodingOptions::inputType,                   "input_type"},
            { EncodingOptions::outputType,                  "output_type"},
            { EncodingOptions::ambisonics,                  "ambisonics"},
            { EncodingOptions::speakerLayout,               "surround"},
            { EncodingOptions::Ambisonics::orderIn,         "ambisonics_order_in"},
            { EncodingOptions::Ambisonics::orderOut,        "ambisonics_order_out"},
            { EncodingOptions::Surround::surroundLayout,    "surroundLayout"}
        };
        
        const std::map<juce::String, juce::String>  coefficientMap = {
            { Coeffs::energy,                               "energy" },
            { Coeffs::radialIntensity,                      "radial_intensity" },
            { Coeffs::transverseIntensity,                  "transverse_intensity" },
            { Coeffs::pressure,                             "pressure" },
            { Coeffs::radialVelocity,                       "radial_velocity" },
            { Coeffs::transverseVelocity,                   "transverse_velocity" },
            { Coeffs::inPhaseQuadratic,                     "in_phase_quad" },
            { Coeffs::symmetryQuadratic,                    "symmetry_quad" },
            { Coeffs::inPhaseLinear,                        "in_phase_lin" },
            { Coeffs::symmetryLinear,                       "symmetry_lin" },
            { Coeffs::totalGainsLinear,                     "total_gains_lin" },
            { Coeffs::totalGainsQuadratic,                  "total_gains_quad" }
        };
    };
}
