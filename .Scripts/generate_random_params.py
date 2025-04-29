import numpy as np
from enum import Enum
import speaker_layouts as sl
import xml.etree.ElementTree as ET
from processing_constants import *

class CoefficientType(Enum):
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

def get_parameter_range(parameter: CoefficientType):
    if parameter == CoefficientType.ENERGY:
        return (0, 1)
    
    elif parameter == CoefficientType.RADIAL_INTENSITY:
        return (0, 1)
    
    elif parameter == CoefficientType.TRANSVERSE_INTENSITY:
        return (0, 1)
    
    elif parameter == CoefficientType.PRESSURE:
        return (0, 1)
    
    elif parameter == CoefficientType.RADIAL_VELOCITY:
        return (0, 1)
    
    elif parameter == CoefficientType.TRANSVERSE_VELOCITY:
        return (0, 1)
    
    elif parameter == CoefficientType.IN_PHASE_QUADRATIC:
        return (0, 1)
    
    elif parameter == CoefficientType.SYMMETRY_QUADRATIC:
        return (0, 1)
    
    elif parameter == CoefficientType.IN_PHASE_LINEAR:
        return (0, 1)
    
    elif parameter == CoefficientType.SYMMETRY_LINEAR:
        return (0, 1)
    
    elif parameter == CoefficientType.TOTAL_GAINS_LINEAR:
        return (0, 1)
    
    elif parameter == CoefficientType.TOTAL_GAINS_QUADRATIC:
        return (0, 1)

    else:
        raise ValueError(f"Unknown parameter type: {parameter}")
    
def get_random_coefficient(coefficient: CoefficientType):
    parameter_range = get_parameter_range(coefficient)
    return np.random.uniform(parameter_range)

def get_random_ambisonics_order():
    return np.random.uniform(1, 7)

def get_random_speaker_layout():
    keys            = list(sl.SPEAKER_LAYOUTS.keys())
    random_key      = np.random.choice(keys)
    random_layout   = sl.SPEAKER_LAYOUTS[random_key]
    return random_layout

def generate_speaker_layout_xml(tree: ET.SubElement, speaker_layout: list):
    return tree

def build_xml_config():
    state_params        = ET.Element(USAT_STATE_PARAMETERS)
    encoding_settings   = ET.SubElement(state_params, ENCODING_SETTINGS)
    layout_types        = [AMBISONICS, SPEAKER_LAYOUT]
    
    # Subtrees and Default Parameters 
    input_ambisonics        = ET.SubElement(state_params, INPUT_AMBISONICS)
    output_ambisonics       = ET.SubElement(state_params, OUTPUT_AMBISONICS)
    input_speaker_layout    = ET.SubElement(state_params, INPUT_SPEAKER_LAYOUT)
    output_speaker_layout   = ET.SubElement(state_params, OUTPUT_SPEAKER_LAYOUT)
    coefficients            = ET.SubElement(state_params, COEFFICIENTS)

    ambisonics_order_default = 1
    input_ambisonics.set(AMBISONICS_ORDER_IN, ambisonics_order_default)
    output_ambisonics.set(AMBISONICS_ORDER_OUT, ambisonics_order_default)

    input_idx   = np.random.choice(layout_types)
    output_idx  = np.random.choice(layout_types)

    input_choice    = layout_types[input_idx]
    output_choice   = layout_types[output_idx]
    
    encoding_settings.set(INPUT_TYPE, input_choice)
    encoding_settings.set(OUTPUT_TYPE, output_choice)
    
    # Random Input Choice
    if input_choice == AMBISONICS:
        ambisonics_order_in = get_random_ambisonics_order()
        input_ambisonics.set(AMBISONICS_ORDER_IN, ambisonics_order_in)
        
    elif input_choice == SPEAKER_LAYOUT:
        speaker_layout = get_random_speaker_layout()
        input_speaker_layout = generate_speaker_layout_xml()

    else: 
        raise ValueError("Input Format Generation Failed.")
    
    # Random Output Choice
    if output_choice == AMBISONICS:
        ambisonics_order_out = get_random_ambisonics_order()
        output_ambisonics.set(AMBISONICS_ORDER_OUT, ambisonics_order_out)

    elif output_choice == SPEAKER_LAYOUT:
        speaker_layout          = get_random_speaker_layout()
        input_speaker_layout    = generate_speaker_layout_xml()

    else:
        raise ValueError("Output Format Generation Failed.")
    

    # Random Coefficient Choice



def main():

    N = 2000

    # RANDOM INPUT FORMAT


    # RANDOM OUTPUT FORMAT
     
    return

if __name__ == main:
    main()