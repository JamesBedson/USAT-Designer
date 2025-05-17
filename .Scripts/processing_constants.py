# ENCODING
USAT_STATE_PARAMETERS   = "USAT_State_Parameters"
ENCODING_SETTINGS       = "Encoding_Settings"
INPUT_TYPE              = "InputType"
OUTPUT_TYPE             = "OutputType"

# SPEAKER LAYOUT
SPEAKER_LAYOUT          = "SpeakerLayout"
INPUT_SPEAKER_LAYOUT    = "Input_Loudspeaker_Layout"
OUTPUT_SPEAKER_LAYOUT   = "Output_Loudspeaker_Layout"

# AMBISONICS
AMBISONICS              = "Ambisonics"
INPUT_AMBISONICS        = "Input_Ambisonics"
OUTPUT_AMBISONICS       = "Output_Ambisonics"
AMBISONICS_ORDER_IN     = "AmbisonicsOrderIn"
AMBISONICS_ORDER_OUT    = "AmbisonicsOrderOut"

# COEFFICIENTS
COEFFICIENTS            = "Coefficients"

ENERGY                  = "energy"
RADIAL_INTENSITY        = "radialIntensity"
TRANSVERSE_INTENSITY    = "transverseIntensity"
PRESSURE                = "pressure"
RADIAL_VELOCITY         = "radialVelocity"
TRANSVERSE_VELOCITY     = "transverseVelocity"
IN_PHASE_QUADRATIC      = "inPhaseQuadratic"
SYMMETRY_QUADRATIC      = "symmetryQuadratic"
IN_PHASE_LINEAR         = "inPhaseLinear"
SYMMETRY_LINEAR         = "symmetryLinear"
TOTAL_GAINS_LINEAR      = "totalGainsLinear"
TOTAL_GAINS_QUADRATIC   = "totalGainsQuadratic"

parameter_to_coefficient_key = {
    "energy": "energy",
    "radialIntensity": "radial_intensity",
    "transverseIntensity": "transverse_intensity",
    "pressure": "pressure",
    "radialVelocity": "radial_velocity",
    "transverseVelocity": "transverse_velocity",
    "inPhaseQuadratic": "in_phase_quad",
    "symmetryQuadratic": "symmetry_quad",
    "totalGainsQuadratic": "total_gains_quad",
    "inPhaseLinear": "in_phase_lin",
    "symmetryLinear": "symmetry_lin",
    "totalGainsLinear": "total_gains_lin"
}

# Parameter Sampling
DISTRIBUTION_RANGE  = "Range"
DISTRIBUTION        = "Distribution"
INPUT_FORMAT        = "InputFormat"
OUTPUT_FORMAT       = "OutputFormat"
FORMAT_CHOICES      = "FormatChoices"

UNIFORM    = "uniform"
NORMAL     = "normal"
CHOICES    = "choice"
BETA       = "beta"