import xml.etree.ElementTree as ET
from pathlib import Path
import gc
import sys, os
from numpy import array as npArray
from processing_constants import *
from universal_transcoder.auxiliars.my_coordinates import MyCoordinates
from universal_transcoder.calculations.optimization import optimize, optimize_for_usat_designer
from typing import Union

from universal_transcoder.auxiliars.get_input_channels import (
    get_input_channels_ambisonics,
    get_input_channels_vbap
)

from universal_transcoder.auxiliars.get_cloud_points import (
    get_all_sphere_points,
    get_equi_circumference_points,
    get_equi_t_design_points,
    mix_clouds_of_points
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


def get_num_ambisonics_channels(order: int) -> int:
    return (order + 1) ** 2


def get_ambisonics_enc_matrix(order: int, 
                            path_to_t_design: Union[os.PathLike, str]) -> tuple:

    '''
    1. Obtains cloud of points with directions sampling the sphere 
    2. Creates the encoding matrix G for ambisonics

    Args: 
        order(int): ambisonics order
        path_to_t_design(Union[os.PathLike, str]): Path to file containing the sampling points within the unit sphere

    Returns:
        point_cloud_optimization: Cloud points
        G: Encoding matrix

    '''

    point_cloud_optimization    = get_equi_t_design_points(path_to_t_design, False)
    G                           = get_input_channels_ambisonics(point_cloud_optimization, order)
    
    return point_cloud_optimization, G


def get_ambisonics_output(order: int) -> MyCoordinates:

    basepath = Path(__file__).resolve().parent
    path_to_t_design = (
        basepath /
        "universal_transcoder" /
        "encoders" /
        "t-design" /
        "des.3.60.10.txt"
    )

    list_of_cloud_points = [
        
        get_equi_t_design_points(
            path_to_t_design, 
            False
            ), 
        
        get_equi_circumference_points(
            get_num_ambisonics_channels(order), 
            False
            )

        ]

    list_of_weights = [1,1]
    ambisonics_output, _ = mix_clouds_of_points(
        list_of_cloud_points=list_of_cloud_points,
        list_of_weights=list_of_weights,
        discard_lower_hemisphere=True
    ) 
    
    return ambisonics_output


def get_speaker_enc_matrix(speaker_layout: MyCoordinates,
                        path_to_t_design: Union[os.PathLike, str]) -> tuple:
    
    point_cloud_optimization    = get_equi_t_design_points(path_to_t_design, False)
    G                           = get_input_channels_vbap(point_cloud_optimization, speaker_layout)

    return point_cloud_optimization, G


def create_encoding_matrix(format: str, parameter_dict: dict, layout_data: Union[int, MyCoordinates]) -> dict:

    basepath = Path(__file__).resolve().parent
    path_to_t_design = (
        basepath /
        "universal_transcoder" /
        "encoders" /
        "t-design" /
        "des.3.56.9.txt"
    )

    cloud_plots = get_all_sphere_points(1, plot_show=False).discard_lower_hemisphere()

    if format == AMBISONICS:
        assert(isinstance(layout_data, int))
        point_cloud_optimization, G = get_ambisonics_enc_matrix(layout_data, path_to_t_design)
        input_matrix_plots          = get_input_channels_ambisonics(cloud_plots, layout_data)
    
    elif format == SPEAKER_LAYOUT:
        assert(isinstance(layout_data, MyCoordinates))
        point_cloud_optimization, G = get_speaker_enc_matrix(layout_data, path_to_t_design)
        input_matrix_plots          = get_input_channels_vbap(cloud_plots, layout_data)

    else:
        raise ValueError("Invalid Format.", format)

    parameter_dict["cloud_optimization"]        = point_cloud_optimization
    parameter_dict["cloud_plots"]               = cloud_plots
    parameter_dict["input_matrix_plots"]        = input_matrix_plots
    parameter_dict["input_matrix_optimization"] = G

    return parameter_dict


def parse_encoding_settings(usat_parameter_settings_xml: ET.Element) -> dict:
    
    # Parse encoding settings
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

        parameter_dict = create_encoding_matrix(format=input_type, 
                                                parameter_dict=parameter_dict, 
                                                layout_data=int(order))

    elif input_type == SPEAKER_LAYOUT:
        input_speaker_layout_xml = usat_parameter_settings_xml.find(INPUT_SPEAKER_LAYOUT)
        assert(input_speaker_layout_xml is not None)
        
        input_speaker_layout = create_speaker_layout(input_speaker_layout_xml)
        
        parameter_dict = create_encoding_matrix(format=input_type,
                                                parameter_dict=parameter_dict,
                                                layout_data=input_speaker_layout)
        
    else:
        raise AssertionError("Not valid format")
    #############################################

    #############################################
    # OUTPUT
    if output_type == AMBISONICS:
        output_ambisonics_xml = usat_parameter_settings_xml.find(OUTPUT_AMBISONICS)
        assert(output_ambisonics_xml is not None)

        order = output_ambisonics_xml.get(AMBISONICS_ORDER_OUT)
        assert(order is not None)
        parameter_dict["output_layout"] = get_ambisonics_output(int(order))
    
    elif output_type == SPEAKER_LAYOUT:
        output_speaker_layout_xml = usat_parameter_settings_xml.find(OUTPUT_SPEAKER_LAYOUT)
        assert(output_speaker_layout_xml is not None)
        parameter_dict["output_layout"] = create_speaker_layout(output_speaker_layout_xml)

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


def start_decoding(xml_string: str) -> list:

    usat_state_parameters_xml   = ET.fromstring(xml_string)
    optimization_dict           = parse_encoding_settings(usat_state_parameters_xml)
    
    optimization_dict["show_results"]       = False
    optimization_dict["save_results"]       = False
    optimization_dict["results_file_name"]  = None
    
    T_optimised = optimize(optimization_dict)
    return T_optimised.T.tolist()

def decode_for_random_parameter_generation(xml_string: str) -> dict:
    usat_state_parameters_xml   = ET.fromstring(xml_string)
    optimization_dict           = parse_encoding_settings(usat_state_parameters_xml)
    
    optimization_dict["show_results"]       = False
    optimization_dict["save_results"]       = False
    optimization_dict["results_file_name"]  = None
    
    all_matrices = optimize_for_usat_designer(optimization_dict) 
    return all_matrices
    

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


'''
def start_decoding(xml_string: str, 
                   progress_callback=None, 
                   status_callback=None):
    import time
    matrix = [[0, 0], [0, 0]]

    for i in range(1, 11):
        time.sleep(0.5)
        if status_callback:
            status_callback(f"Processing {i+1}/10")

        if progress_callback:
            progress_callback(i / 10)

        else:
            print("Error callback!")

    return matrix
'''
