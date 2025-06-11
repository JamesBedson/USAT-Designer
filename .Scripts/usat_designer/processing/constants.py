# ENCODING
DSN_XML_USAT_STATE_PARAMETERS   = "USAT_State_Parameters"
DSN_XML_SETTINGS                = "Settings"
DSN_XML_INPUT_TYPE              = "InputType"
DSN_XML_OUTPUT_TYPE             = "OutputType"

# SPEAKER LAYOUT
DSN_XML_SPEAKER_LAYOUT          = "SpeakerLayout"
DSN_XML_INPUT_SPEAKER_LAYOUT    = "Input_Loudspeaker_Layout"
DSN_XML_OUTPUT_SPEAKER_LAYOUT   = "Output_Loudspeaker_Layout"
DSN_SPK_CHANNEL_ID              = "ID"
DSN_SPK_AZIMUTH                 = "Azimuth"
DSN_SPK_ELEVATION               = "Elevation"
DSN_SPK_DISTANCE                = "Distance"
DSN_SPK_LFE                     = "IsLFE"

# AMBISONICS
DSN_XML_AMBISONICS              = "Ambisonics"
DSN_XML_INPUT_AMBISONICS        = "Input_Ambisonics"
DSN_XML_OUTPUT_AMBISONICS       = "Output_Ambisonics"
DSN_XML_AMBISONICS_ORDER_IN     = "AmbisonicsOrderIn"
DSN_XML_AMBISONICS_ORDER_OUT    = "AmbisonicsOrderOut"
DSN_XML_COEFFICIENTS            = "Coefficients"

# COEFFICIENTS
DSN_COEFF_ENERGY                  = "energy"
DSN_COEFF_RADIAL_INTENSITY        = "radialIntensity"
DSN_COEFF_TRANSVERSE_INTENSITY    = "transverseIntensity"
DSN_COEFF_PRESSURE                = "pressure"
DSN_COEFF_RADIAL_VELOCITY         = "radialVelocity"
DSN_COEFF_TRANSVERSE_VELOCITY     = "transverseVelocity"
DSN_COEFF_IN_PHASE_QUADRATIC      = "inPhaseQuadratic"
DSN_COEFF_SYMMETRY_QUADRATIC      = "symmetryQuadratic"
DSN_COEFF_IN_PHASE_LINEAR         = "inPhaseLinear"
DSN_COEFF_SYMMETRY_LINEAR         = "symmetryLinear"
DSN_COEFF_TOTAL_GAINS_LINEAR      = "totalGainsLinear"
DSN_COEFF_TOTAL_GAINS_QUADRATIC   = "totalGainsQuadratic"

OPT_COEFF_ENERGY                  = "energy"
OPT_COEFF_RADIAL_INTENSITY        = "radial_intensity"
OPT_COEFF_TRANSVERSE_INTENSITY    = "transverse_intensity"
OPT_COEFF_PRESSURE                = "pressure"
OPT_COEFF_RADIAL_VELOCITY         = "radial_velocity"
OPT_COEFF_TRANSVERSE_VELOCITY     = "transverse_velocity"
OPT_COEFF_IN_PHASE_QUADRATIC      = "in_phase_quad"
OPT_COEFF_SYMMETRY_QUADRATIC      = "symmetry_quad"
OPT_COEFF_IN_PHASE_LINEAR         = "in_phase_lin"
OPT_COEFF_SYMMETRY_LINEAR         = "symmetry_lin"
OPT_COEFF_TOTAL_GAINS_LINEAR      = "total_gains_lin"
OPT_COEFF_TOTAL_GAINS_QUADRATIC   = "total_gains_quad"

# camel case to snake case translation
parameter_to_coefficient_key = {
    DSN_COEFF_ENERGY: OPT_COEFF_ENERGY,
    DSN_COEFF_RADIAL_INTENSITY: OPT_COEFF_RADIAL_INTENSITY,
    DSN_COEFF_TRANSVERSE_INTENSITY: OPT_COEFF_TRANSVERSE_INTENSITY,
    DSN_COEFF_PRESSURE: OPT_COEFF_PRESSURE,
    DSN_COEFF_RADIAL_VELOCITY: OPT_COEFF_RADIAL_VELOCITY,
    DSN_COEFF_TRANSVERSE_VELOCITY: OPT_COEFF_TRANSVERSE_VELOCITY,
    DSN_COEFF_IN_PHASE_QUADRATIC: OPT_COEFF_IN_PHASE_QUADRATIC,
    DSN_COEFF_SYMMETRY_QUADRATIC: OPT_COEFF_SYMMETRY_QUADRATIC,
    DSN_COEFF_TOTAL_GAINS_QUADRATIC: OPT_COEFF_TOTAL_GAINS_QUADRATIC,
    DSN_COEFF_IN_PHASE_LINEAR: OPT_COEFF_TOTAL_GAINS_LINEAR,
    DSN_COEFF_SYMMETRY_LINEAR: OPT_COEFF_SYMMETRY_LINEAR,
    DSN_COEFF_TOTAL_GAINS_LINEAR: OPT_COEFF_TOTAL_GAINS_LINEAR
}

# Optimisation Dictionary Keys
# PARAMETER DICTIONARY
OPT_PD_INPUT_MATRIX_PLOTS          = "input_matrix_plots"
OPT_PD_INPUT_MATRIX_OPTIMIZATION   = "input_matrix_optimization"
OPT_PD_CLOUD_PLOTS                 = "cloud_plots"
OPT_PD_CLOUD_OPTIMIZATION          = "cloud_optimization"
OPT_PD_OUTPUT_LAYOUT               = "output_layout"
OPT_PD_T_OPTIMIZED                 = "T_optimized"
OPT_PD_SHOW_RESULTS                = "show_results"
OPT_PD_SAVE_RESULTS                = "save_results"
OPT_PD_RESULTS_FILE_NAME           = "results_file_name"
OPT_PD_COEFFICIENTS                = "coefficients"
OPT_PD_DIRECTIONAL_WEIGHTS         = "directional_weights"
OPT_PD_DSPK                        = "Dspk"

# Parameter Sampling
DSN_SMPL_DISTRIBUTION_ARGS      = "Args"
DSN_SMPL_DISTRIBUTION           = "Distribution"
DSN_SMPL_INPUT_FORMAT           = "InputFormat"
DSN_SMPL_OUTPUT_FORMAT          = "OutputFormat"
DSN_SMPL_FORMAT_CHOICES         = "FormatChoices"
DSN_SMPL_INPUT_LAYOUT_DESC      = "InputLayoutDescription"
DSN_SMPL_OUTPUT_LAYOUT_DESC     = "OutputLayoutDescription"
DSN_SMPL_QUALITY_SCORE          = "quality_score"
DSN_SMPL_APPARENT_SOURCE_WIDTH  = "median_source_width"
DSN_SMPL_SEED                   = "seed"
DSN_SMPL_X_p                    = "X_p"
# Distributions
DSN_SMPL_UNIFORM     = "uniform"
DSN_SMPL_NORMAL      = "normal"
DSN_SMPL_CHOICE      = "choice"
DSN_SMPL_BETA        = "beta"

# Output Data and Plots 
DSN_OUT_SPEAKER_MATRIX      = "S"
DSN_OUT_DECODING_MATRIX     = "D"
DSN_OUT_ENCODING_MATRIX     = "G"
DSN_OUT_TRANSCODING_MATRIX  = "T_optimized"
DSN_OUT_OUTPUT_LAYOUT       = "output_layout"
DSN_OUT_CLOUD               = "cloud"

DSN_PLT_ENERGY                  = "e_base_64"
DSN_PLT_PRESSURE                = "pressure_base_64"
DSN_PLT_RADIAL_INTENSITY        = "ri_base_64"
DSN_PLT_TRANSVERSE_INTENSITY    = "ti_base_64"
DSN_PLT_ANGULAR_ERROR           = "ae_base_64"
DSN_PLT_SOURCE_WIDTH            = "sw_base_64"
DSN_PLT_ACCCENT_COLOUR          = "#ee9b00"
DSN_PLT_PRIMARY_BG_COLOUR       = "#15191F"
DSN_PLT_SECONDARY_BG_COLOUR     = "#15191F"
DSN_PLT_TEXT_COLOUR             = "#ffffff"
DSN_PLT_LINE_COLOUR             = "#ffffff"

DSN_PLT_GRADIENT_COOL           = "#0f4c5c"
DSN_PLT_GRANDIENT_NEUTRAL       = "#fb8b24"
DSN_PLT_GRADIENT_WARM           = "#5f0f40"

DSN_PLT_ENERGY_TITLE            = "Energy"
DSN_PLT_PRESSURE_TITLE          = "Pressure"
DSN_PLT_RADIAL_I_TITLE          = "Radial Intensity"
DSN_PLT_TRANSVERSE_I_TITLE      = "Transverse Intensity"
DSN_PLT_RADIAL_V_TITLE          = "Radial Velocity"
DSN_PLT_TRANSVERSE_V_TITLE      = "Transverse Velocity"
DSN_PLT_ANGULAR_ERROR_TITLE     = "Angular Error"
DSN_PLT_SOURCE_WIDTH_TITLE      = "Source Width"

# DIRECTORIES ANALYSIS
DSN_DIR_BASE                                = "analysis"
DSN_DIR_PLOTS                               = "plots"
DSN_DIR_USAT_STATE_PARAMETERS               = "usat_state_parameters"

# PLOTS ANALYSIS
DSN_PLT_PATH_FOCUS_VS_QUALITY_UNFILTERED    = "focus_vs_quality_unfiltered.png"
DSN_PLT_PATH_FOCUS_VS_QUALITY_THRESHOLD     = "focus_vs_quality_threshold.png"
DSN_PLT_PATH_FOCUS_QUALITY_DISTRIBUTION     = "focus_quality_distribution.png"
DSN_PLT_PATH_FOCUS_GRID                     = "focus_grid.png"

# USAT STATE PARAMETERS ANALYSIS
DSN_PARAMS_PATH_LOW                         = "focus_low.xml"
DSN_PARAMS_PATH_MID                         = "focus_mid.xml"
DSN_PARAMS_PATH_HIGH                        = "focus_high.xml" 