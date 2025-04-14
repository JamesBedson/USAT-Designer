import xml.etree.ElementTree as ET
from pathlib import Path
import gc
import sys, os
from numpy import array as npArray
from processing_constants import *
from universal_transcoder.auxiliars.my_coordinates import MyCoordinates
from universal_transcoder.calculations.optimization import optimize
from typing import Union

from universal_transcoder.auxiliars.get_input_channels import (
    get_input_channels_ambisonics,
    get_input_channels_vbap
)

from universal_transcoder.auxiliars.get_cloud_points import (
    get_all_sphere_points, 
    get_equi_t_design_points
)
#################################################################################

def create_speaker_layout(speaker_layout_xml: ET.Element) -> MyCoordinates:
    assert(speaker_layout_xml is not None)
    speakers = []
    for speaker in speaker_layout_xml:
        azimuth     = speaker.get("Azimuth")
        elevation   = speaker.get("Elevation")
        distance    = speaker.get("Distance")
        
        assert(azimuth is not None)
        assert(elevation is not None)
        assert(distance is not None)

        speakers.append((float(azimuth), float(elevation), float(distance)))

    return MyCoordinates.mult_points(npArray(speakers))


def parse_coefficients(coefficients_xml: ET.Element) -> dict[str, float]:
    coefficients = {}
    
    energy_val                  = coefficients_xml.get(ENERGY)
    assert(energy_val is not None)

    radial_intensity_val        = coefficients_xml.get(RADIAL_INTENSITY)
    assert(radial_intensity_val is not None)

    transverse_intensity_val    = coefficients_xml.get(TRANSVERSE_INTENSITY)
    assert(transverse_intensity_val is not None)

    pressure_val                = coefficients_xml.get(PRESSURE)
    assert(pressure_val is not None)

    radial_velocity_val         = coefficients_xml.get(RADIAL_VELOCITY)
    assert(radial_velocity_val is not None)
    
    transverse_velocity_val     = coefficients_xml.get(TRANSVERSE_VELOCITY)
    assert(transverse_velocity_val is not None)
    
    in_phase_quadratic_val      = coefficients_xml.get(IN_PHASE_QUADRATIC)
    assert(in_phase_quadratic_val is not None)

    symmetry_quadratic_val      = coefficients_xml.get(SYMMETRY_QUADRATIC)
    assert(symmetry_quadratic_val is not None)

    in_phase_linear_val         = coefficients_xml.get(IN_PHASE_LINEAR)
    assert(in_phase_linear_val is not None)

    symmetry_linear_val         = coefficients_xml.get(SYMMETRY_LINEAR)
    assert(symmetry_linear_val is not None)

    total_gains_linear_val      = coefficients_xml.get(TOTAL_GAINS_LINEAR)
    assert(total_gains_linear_val is not None)

    total_gains_quadratic_val   = coefficients_xml.get(TOTAL_GAINS_QUADRATIC)
    assert(total_gains_quadratic_val is not None)

    coefficients["energy"]                  = float(energy_val)
    coefficients["radial_intensity"]        = float(radial_intensity_val)
    coefficients["transverse_intensity"]    = float(transverse_intensity_val)
    coefficients["pressure"]                = float(pressure_val)
    coefficients["radial_velocity"]         = float(radial_velocity_val)
    coefficients["transverse_velocity"]     = float(transverse_velocity_val)
    coefficients["in_phase_quad"]           = float(in_phase_quadratic_val)
    coefficients["symmetry_quad"]           = float(symmetry_quadratic_val)
    coefficients["in_phase_lin"]            = float(in_phase_linear_val)
    coefficients["symmetry_lin"]            = float(symmetry_linear_val)
    coefficients["total_gains_lin"]         = float(total_gains_linear_val)
    coefficients["total_gains_quad"]        = float(total_gains_quadratic_val)

    return coefficients


def get_ambisonics_matrix(order: int, 
                          path_to_t_design: Union[os.PathLike, str]) -> tuple:

    cloud_optimization  = get_equi_t_design_points(path_to_t_design, False)
    matrix              = get_input_channels_ambisonics(cloud_optimization, order)
    name                = f"{order}OA"
    
    return cloud_optimization, matrix


def create_ambisonics_enc(parameter_dict: dict, order: int):
    basepath = Path(__file__).resolve().parent
    path_to_t_design = (
        basepath /
        "universal_transcoder" /
        "encoders" /
        "t-design" /
        "des.3.56.9.txt"
    )

    cloud_optimization, matrix  = get_ambisonics_matrix(order, path_to_t_design)
    cloud_plots                 = get_all_sphere_points(1, False)

    parameter_dict["cloud_optimization"]        = cloud_optimization
    parameter_dict["cloud_plots"]               = cloud_plots
    parameter_dict["input_matrix_plots"]        = get_input_channels_ambisonics(cloud_plots, order)
    parameter_dict["input_matrix_optimization"] = matrix

    return parameter_dict


def parse_encoding_settings(usat_parameter_settings_xml: ET.Element) -> dict:
    
    encoding_settings_xml = usat_parameter_settings_xml.find("Encoding_Settings")
    assert(encoding_settings_xml is not None)

    parameter_dict = {}

    input_type  = encoding_settings_xml.get(INPUT_TYPE)
    output_type = encoding_settings_xml.get(OUTPUT_TYPE)

    assert(input_type is not None)
    assert(output_type is not None)

    #############################################
    # INPUT

    if input_type == AMBISONICS:
        input_ambisonics_xml = usat_parameter_settings_xml.find(INPUT_AMBISONICS)
        assert(input_ambisonics_xml is not None)
        
        order = input_ambisonics_xml.get(AMBISONICS_ORDER_IN)
        assert(order is not None)

        parameter_dict = create_ambisonics_enc(parameter_dict, int(order))


    elif input_type == SPEAKER_LAYOUT:
        input_speaker_layout_xml = usat_parameter_settings_xml.find(INPUT_SPEAKER_LAYOUT)
        assert(input_speaker_layout_xml is not None)
        
        input_speaker_layout = create_speaker_layout(input_speaker_layout_xml)
        parameter_dict["input_matrix_optimization"] = input_speaker_layout

    else:
        raise AssertionError("Not valid format")
    #############################################

    #############################################
    # OUTPUT
    print(output_type)
    if output_type == AMBISONICS:
        output_ambisonics_xml = usat_parameter_settings_xml.find(OUTPUT_AMBISONICS)
        assert(output_ambisonics_xml is not None)

        order = output_ambisonics_xml.get(AMBISONICS_ORDER_OUT)
        assert(order is not None)
        parameter_dict["output_layout"] = create_ambisonics_enc(parameter_dict, int(order))
    
    elif output_type == SPEAKER_LAYOUT:
        output_speaker_layout_xml = usat_parameter_settings_xml.find(OUTPUT_SPEAKER_LAYOUT)
        assert(output_speaker_layout_xml is not None)

        output_speaker_layout = create_speaker_layout(output_speaker_layout_xml)
        parameter_dict["output_layout"] = output_speaker_layout

    else:
        raise AssertionError("Not valid format")
    #############################################

    # COEFFICIENTS
    coefficients_xml = usat_parameter_settings_xml.find(COEFFICIENTS)
    assert(coefficients_xml is not None)
    
    parameter_dict["coefficients"] = parse_coefficients(coefficients_xml)
    
    # MISC
    parameter_dict["show_results"]          = False
    parameter_dict["save_results"]          = False
    parameter_dict["directional_weights"]   = 1

    return parameter_dict

'''def start_decoding(xml_string: str):

    usat_state_parameters_xml   = ET.fromstring(xml_string)
    optimization_dict           = parse_encoding_settings(usat_state_parameters_xml)
    
    optimization_dict["show_results"]       = False
    optimization_dict["save_results"]       = False
    optimization_dict["results_file_name"]  = None
    
    gain_matrix = optimize(optimization_dict).T

    print("Number of Input Channels:", gain_matrix.shape[0])
    print("Number of Output Channels:", gain_matrix.shape[1])
    
    return gain_matrix.tolist()
'''
def start_decoding(xml_string: str, progress_callback=None):
    import time
    print("Callback function code")
    matrix = [[0, 0], [0, 0]]

    for i in range(10):
        time.sleep(0.5)
        if progress_callback:
            progress_callback(i / 10)
        else:
            print("Error callback!")
            
    return matrix
#################################################################################
def main():

    if len(sys.argv) != 2:
        print("Usage: python script.py <your_argument>")

    else:
        gain_matrix = start_decoding(sys.argv[1])

    return gain_matrix
#################################################################################

if __name__ == "__main__":
    main()
    sys.modules.clear()
    gc.collect()
