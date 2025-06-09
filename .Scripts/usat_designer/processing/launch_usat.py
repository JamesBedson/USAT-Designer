import xml.etree.ElementTree as ET
from pathlib import Path
from typing import Union
import gc
import sys, os
from numpy import array as npArray
import threading
from usat_designer.processing.constants import *
from universal_transcoder.auxiliars.my_coordinates import MyCoordinates
from usat_designer.processing.optimize_usat_designer import optimize_for_usat_designer
from universal_transcoder.auxiliars.get_decoder_matrices import get_ambisonics_decoder_matrix
import matplotlib
import xml.dom.minidom as minidom


from universal_transcoder.auxiliars.get_input_channels import (
    get_input_channels_ambisonics,
    get_input_channels_vbap,
)

from universal_transcoder.auxiliars.get_cloud_points import (
    get_all_sphere_points,
    get_equi_circumference_points,
    get_equi_t_design_points,
    mix_clouds_of_points
)

from universal_transcoder.calculations.energy_intensity import (
    energy_calculation,
    radial_I_calculation,
    transverse_I_calculation,
    angular_error,
    width_angle
)

from universal_transcoder.calculations.pressure_velocity import (
    pressure_calculation
)

from usat_designer.processing.plots_usat_designer import *

#################################################################################

def create_speaker_layout(speaker_layout_xml: ET.Element) -> MyCoordinates:
    assert(speaker_layout_xml is not None)
    speakers = []
    for speaker in speaker_layout_xml:
        azimuth     = speaker.get(DSN_SPK_AZIMUTH)
        elevation   = speaker.get(DSN_SPK_ELEVATION)
        distance    = speaker.get(DSN_SPK_DISTANCE)
        isLFE       = speaker.get(DSN_SPK_LFE)

        assert(azimuth is not None)
        assert(elevation is not None)
        assert(distance is not None)
        assert(isLFE is not None)
        
        if bool(float(isLFE)) == False:
            speakers.append((float(azimuth), float(elevation), float(distance)))
    
    return MyCoordinates.mult_points(npArray(speakers))


def parse_coefficients(coefficients_xml: ET.Element) -> dict[str, float]:
    coefficients = {}
    
    energy_val                  = coefficients_xml.get(DSN_COEFF_ENERGY)
    assert(energy_val is not None)

    radial_intensity_val        = coefficients_xml.get(DSN_COEFF_RADIAL_INTENSITY)
    assert(radial_intensity_val is not None)

    transverse_intensity_val    = coefficients_xml.get(DSN_COEFF_TRANSVERSE_INTENSITY)
    assert(transverse_intensity_val is not None)

    pressure_val                = coefficients_xml.get(DSN_COEFF_PRESSURE)
    assert(pressure_val is not None)

    radial_velocity_val         = coefficients_xml.get(DSN_COEFF_RADIAL_VELOCITY)
    assert(radial_velocity_val is not None)
    
    transverse_velocity_val     = coefficients_xml.get(DSN_COEFF_TRANSVERSE_VELOCITY)
    assert(transverse_velocity_val is not None)
    
    in_phase_quadratic_val      = coefficients_xml.get(DSN_COEFF_IN_PHASE_QUADRATIC)
    assert(in_phase_quadratic_val is not None)

    symmetry_quadratic_val      = coefficients_xml.get(DSN_COEFF_SYMMETRY_QUADRATIC)
    assert(symmetry_quadratic_val is not None)

    in_phase_linear_val         = coefficients_xml.get(DSN_COEFF_IN_PHASE_LINEAR)
    assert(in_phase_linear_val is not None)

    symmetry_linear_val         = coefficients_xml.get(DSN_COEFF_SYMMETRY_LINEAR)
    assert(symmetry_linear_val is not None)

    total_gains_linear_val      = coefficients_xml.get(DSN_COEFF_TOTAL_GAINS_LINEAR)
    assert(total_gains_linear_val is not None)

    total_gains_quadratic_val   = coefficients_xml.get(DSN_COEFF_TOTAL_GAINS_QUADRATIC)
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

    cloud_optimization  = get_equi_t_design_points(path_to_t_design, False)
    G                   = get_input_channels_ambisonics(cloud_optimization, order)
    
    return cloud_optimization, G


def get_ambisonics_output(order: int) -> MyCoordinates:

    basepath = Path(__file__).resolve().parents[2]
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

    list_of_weights = [1, 1]
    ambisonics_output, _ = mix_clouds_of_points(
        list_of_cloud_points=list_of_cloud_points,
        list_of_weights=list_of_weights,
        discard_lower_hemisphere=True
    ) 
    
    return ambisonics_output


def get_speaker_enc_matrix(speaker_layout: MyCoordinates,
                           path_to_t_design: Union[os.PathLike, str]) -> tuple:
    
    t_design_points             = get_equi_t_design_points(path_to_t_design, False)
    circumference_points        = get_equi_circumference_points(15, False)
    cloud_points_list           = [t_design_points, circumference_points, speaker_layout]     
    
    cloud_optimization, weights = mix_clouds_of_points(
        cloud_points_list,
        list_of_weights=None,
        discard_lower_hemisphere=True
    )

    G = get_input_channels_vbap(cloud_optimization, speaker_layout)
    
    return cloud_optimization, G, weights


def create_encoding_matrix(format: str, parameter_dict: dict, layout_data: Union[int, MyCoordinates]) -> dict:

    basepath = Path(__file__).resolve().parents[2]

    path_to_t_design = (
        basepath /
        "universal_transcoder" /
        "encoders" /
        "t-design" /
        "des.3.56.9.txt"
    )

    cloud_plots = get_all_sphere_points(1, plot_show=False).discard_lower_hemisphere()

    if format == DSN_XML_AMBISONICS:
        assert(isinstance(layout_data, int))
        directional_weights         = 1
        cloud_optimization, G       = get_ambisonics_enc_matrix(layout_data, path_to_t_design)
        input_matrix_plots          = get_input_channels_ambisonics(cloud_plots, layout_data)
    
    elif format == DSN_XML_SPEAKER_LAYOUT:
        assert(isinstance(layout_data, MyCoordinates))
        cloud_optimization, G, directional_weights      = get_speaker_enc_matrix(layout_data, path_to_t_design)
        input_matrix_plots                              = get_input_channels_vbap(cloud_plots, layout_data)

    else:
        raise ValueError("Invalid Format.", format)

    parameter_dict[OPT_PD_DIRECTIONAL_WEIGHTS]       = directional_weights
    parameter_dict[OPT_PD_CLOUD_OPTIMIZATION]        = cloud_optimization
    parameter_dict[OPT_PD_CLOUD_PLOTS]               = cloud_plots
    parameter_dict[OPT_PD_INPUT_MATRIX_PLOTS]        = input_matrix_plots
    parameter_dict[OPT_PD_INPUT_MATRIX_OPTIMIZATION] = G

    return parameter_dict


def parse_encoding_settings(usat_parameter_settings_xml: ET.Element) -> dict:
    
    # Parse encoding settings
    encoding_settings_xml = usat_parameter_settings_xml.find(DSN_XML_SETTINGS)
    assert(encoding_settings_xml is not None)

    parameter_dict = {}

    input_type  = encoding_settings_xml.get(DSN_XML_INPUT_TYPE)
    output_type = encoding_settings_xml.get(DSN_XML_OUTPUT_TYPE)

    assert(input_type is not None)
    assert(output_type is not None)

    #############################################
    # INPUT

    if input_type == DSN_XML_AMBISONICS:
        input_ambisonics_xml = usat_parameter_settings_xml.find(DSN_XML_INPUT_AMBISONICS)
        assert(input_ambisonics_xml is not None)
        
        order = input_ambisonics_xml.get(DSN_XML_AMBISONICS_ORDER_IN)
        assert(order is not None)

        parameter_dict = create_encoding_matrix(format=input_type, 
                                                parameter_dict=parameter_dict, 
                                                layout_data=int(order))

    elif input_type == DSN_XML_SPEAKER_LAYOUT:
        input_speaker_layout_xml = usat_parameter_settings_xml.find(DSN_XML_INPUT_SPEAKER_LAYOUT)
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
    if output_type == DSN_XML_AMBISONICS:
        output_ambisonics_xml = usat_parameter_settings_xml.find(DSN_XML_OUTPUT_AMBISONICS)
        assert(output_ambisonics_xml is not None)

        order = output_ambisonics_xml.get(DSN_XML_AMBISONICS_ORDER_OUT)        
        assert(order is not None)

        parameter_dict[OPT_PD_OUTPUT_LAYOUT] = get_ambisonics_output(int(order))
        parameter_dict[OPT_PD_DSPK]          = get_ambisonics_decoder_matrix(
            int(order), 
            parameter_dict["output_layout"], 
            "pseudo"
            )

    elif output_type == DSN_XML_SPEAKER_LAYOUT:
        output_speaker_layout_xml = usat_parameter_settings_xml.find(DSN_XML_OUTPUT_SPEAKER_LAYOUT)
        
        assert(output_speaker_layout_xml is not None)
        parameter_dict[OPT_PD_OUTPUT_LAYOUT] = create_speaker_layout(output_speaker_layout_xml)

    else:
        raise AssertionError("Not valid format")
    #############################################

    # COEFFICIENTS
    coefficients_xml = usat_parameter_settings_xml.find(DSN_XML_COEFFICIENTS)
    assert(coefficients_xml is not None)
    
    parameter_dict[OPT_PD_COEFFICIENTS]  = parse_coefficients(coefficients_xml)
    parameter_dict[OPT_PD_SHOW_RESULTS]  = False
    parameter_dict[OPT_PD_SAVE_RESULTS]  = False

    return parameter_dict

def threaded_plot_worker(optimisation_data, output_container):
    plots = generate_base64_plots(optimisation_data, return_base_64=True)
    output_container.update(plots)

def generate_base64_plots(optimisation_data: dict,
                          return_base_64 = True) -> dict:
    
    S               = optimisation_data[DSN_OUT_SPEAKER_MATRIX]
    cloud           = optimisation_data[DSN_OUT_CLOUD]
    output_layout   = optimisation_data[DSN_OUT_OUTPUT_LAYOUT]
    
    energy          = energy_calculation(S)
    pressure        = pressure_calculation(S)
    radial_i        = radial_I_calculation(cloud, S, output_layout)
    transverse_i    = transverse_I_calculation(cloud, S, output_layout)
    ae              = angular_error(radial_i, transverse_i)
    source_width    = width_angle(radial_i)

    colors = [
    DSN_PLT_GRADIENT_COOL,
    DSN_PLT_GRANDIENT_NEUTRAL,
    DSN_PLT_GRADIENT_WARM
    ]

    cmap = LinearSegmentedColormap.from_list("custom_coolwarm", colors)
    
    # Energy
    energy_base64           = plot_scalar_map(values=energy,
                                              cloud_points=cloud,
                                              title="Energy",
                                              colorbar_label="Energy",
                                              clim_range=(0, 2),
                                              cmap=cmap,
                                              return_base64=return_base_64)
    
    pressure_base64         = plot_scalar_map(values=pressure,
                                              cloud_points=cloud,
                                              title="Pressure",
                                              colorbar_label="Pressure",
                                              clim_range=(0, 2),
                                              cmap=cmap,
                                              return_base64=return_base_64)
    
    # Radial Itensity
    radial_i_base64         = plot_scalar_map(values=radial_i,
                                              cloud_points=cloud,
                                              title="Radial Intensity",
                                              colorbar_label="Radial Intensity",
                                              clim_range=(0,1),
                                              cmap=cmap,
                                              return_base64=return_base_64)
    # Transverse Intensity
    transverse_i_base64     = plot_scalar_map(values=transverse_i,
                                              cloud_points=cloud,
                                              title="Transverse Intensity",
                                              colorbar_label="Transverse Intensity",
                                              clim_range=(0,1),
                                              cmap=cmap,
                                              return_base64=return_base_64)
    # Angular Error
    angular_error_base64    = plot_scalar_map(values=ae,
                                              cloud_points=cloud,
                                              title="Angular Error",
                                              colorbar_label="Angular Error (Degrees)",
                                              clim_range=(0, 45),
                                              cmap=cmap,
                                              return_base64=return_base_64)
    # Apparent Source Width
    source_width_base64     = plot_scalar_map(values=source_width,
                                              cloud_points=cloud,
                                              title="Source Width",
                                              colorbar_label="Source Width (Degrees)",
                                              clim_range=(0, 45),
                                              cmap=cmap,
                                              return_base64=return_base_64)

    plot_data = {
        DSN_PLT_ENERGY: energy_base64,
        DSN_PLT_PRESSURE: pressure_base64,
        DSN_PLT_RADIAL_INTENSITY: radial_i_base64,
        DSN_PLT_TRANSVERSE_INTENSITY: transverse_i_base64,
        DSN_PLT_ANGULAR_ERROR: angular_error_base64,
        DSN_PLT_SOURCE_WIDTH: source_width_base64
    }

    return plot_data


def start_decoding(xml_string: str,
                   progress_callback=None,
                   status_callback=None) -> tuple:
    
    matplotlib.use("Agg")

    total_steps = 4
    
    if progress_callback:
        progress_callback(1.0 / total_steps)
    
    if status_callback:
        status_callback("Processing USAT parameters")

    usat_state_parameters_xml   = ET.fromstring(xml_string)
    optimization_dict           = parse_encoding_settings(usat_state_parameters_xml)

    optimization_dict[OPT_PD_SHOW_RESULTS]       = False
    optimization_dict[OPT_PD_SAVE_RESULTS]       = False
    optimization_dict[OPT_PD_RESULTS_FILE_NAME]  = None
    
    if progress_callback:
        progress_callback(2.0 / total_steps)

    if status_callback:
        status_callback("Optimizing")
    optimisation_data   = optimize_for_usat_designer(optimization_dict)
    T_optimised         = optimisation_data[DSN_OUT_TRANSCODING_MATRIX].T.tolist()

    if progress_callback:
        progress_callback(3.0 / total_steps)

    if status_callback:
        status_callback("Generating Plots")

    plot_data_container = {}
    plot_thread = threading.Thread(target=threaded_plot_worker, args=(optimisation_data, plot_data_container))
    plot_thread.start()
    plot_thread.join()
    
    if progress_callback:
        progress_callback(4.0 / total_steps)

    if status_callback:
        status_callback("Finishing")

    energy_base_64                  = plot_data_container[DSN_PLT_ENERGY]
    pressure_base_64                = plot_data_container[DSN_PLT_PRESSURE]
    radial_intensity_base_64        = plot_data_container[DSN_PLT_RADIAL_INTENSITY]
    transverse_intensity_base_64    = plot_data_container[DSN_PLT_TRANSVERSE_INTENSITY]
    angular_error_base_64           = plot_data_container[DSN_PLT_ANGULAR_ERROR]
    source_width_base_64            = plot_data_container[DSN_PLT_SOURCE_WIDTH]
    
    return (
        T_optimised,
        energy_base_64,
        pressure_base_64,
        radial_intensity_base_64,
        transverse_intensity_base_64,
        angular_error_base_64,
        source_width_base_64
    )


def decode_for_random_parameter_generation(xml_string: str) -> dict:
    usat_state_parameters_xml   = ET.fromstring(xml_string)
    optimization_dict           = parse_encoding_settings(usat_state_parameters_xml)
    
    optimization_dict["show_results"]       = False
    optimization_dict["save_results"]       = False
    optimization_dict["results_file_name"]  = None
    
    output_data = optimize_for_usat_designer(optimization_dict) 
    print(output_data.keys())
    return output_data
    

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
