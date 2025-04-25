import numpy as np
from enum import Enum

class ParameterType(Enum):
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

def get_parameter_range(parameter: ParameterType):
    if parameter == ParameterType.ENERGY:
        return (0, 1)
    
    elif parameter == ParameterType.RADIAL_INTENSITY:
        return (0, 1)
    
    elif parameter == ParameterType.TRANSVERSE_INTENSITY:
        return (0, 1)
    
    elif parameter == ParameterType.PRESSURE:
        return (0, 1)
    
    elif parameter == ParameterType.RADIAL_VELOCITY:
        return (0, 1)
    
    elif parameter == ParameterType.TRANSVERSE_VELOCITY:
        return (0, 1)
    
    elif parameter == ParameterType.IN_PHASE_QUADRATIC:
        return (0, 1)
    
    elif parameter == ParameterType.SYMMETRY_QUADRATIC:
        return (0, 1)
    
    elif parameter == ParameterType.IN_PHASE_LINEAR:
        return (0, 1)
    
    elif parameter == ParameterType.SYMMETRY_LINEAR:
        return (0, 1)
    
    elif parameter == ParameterType.TOTAL_GAINS_LINEAR:
        return (0, 1)
    
    elif parameter == ParameterType.TOTAL_GAINS_QUADRATIC:
        return (0, 1)

    else:
        raise ValueError(f"Unknown parameter type: {parameter}")

def get_random_ambisonics_order():
    return np.random.uniform(1, 7)

def get_random_speaker_layout():
    

def main():

    N = 2000

    # RANDOM INPUT FORMAT


    # RANDOM OUTPUT FORMAT
     
    return

if __name__ == main:
    main()