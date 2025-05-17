import numpy as np
import speaker_layouts as sl
import xml.etree.ElementTree as ET
import yaml
from processing_constants import *
from typing import Union
import receive_parameters
import speaker_layouts as sl
import os

def parse_from_config(yaml_file):
    with open(yaml_file, "r") as file:
        config = yaml.safe_load(file)
    
    # COEFFICIENTS
    coeff_config = config.get(COEFFICIENTS, {})
    coeffs = {}

    for coeff_name, coeff_data in coeff_config.items():
        coeff_distribution = coeff_data.get(DISTRIBUTION)
        coeff_range        = coeff_data.get(DISTRIBUTION_RANGE)
        coeff_value        = get_y_i(coeff_distribution, coeff_range)
        coeffs[coeff_name] = coeff_value

    # FORMATS
    default_ambisonics_order = 1
    
    input_data  = get_random_x_lambda(config[INPUT_FORMAT])
    output_data = get_random_x_lambda(config[OUTPUT_FORMAT])

    encoding_settings = {
        INPUT_TYPE: input_data[0],
        OUTPUT_TYPE: output_data[0],
        AMBISONICS_ORDER_IN: input_data[1] if input_data[0] == AMBISONICS else default_ambisonics_order,
        AMBISONICS_ORDER_OUT: output_data[1] if output_data[0] == AMBISONICS else default_ambisonics_order,
    }
    
    input_speaker_layout    = []
    output_speaker_layout   = []

    if input_data[0] == SPEAKER_LAYOUT:
        input_speaker_layout = sl.SPEAKER_LAYOUTS.get(input_data[1])

    if output_data[0] == SPEAKER_LAYOUT:
        output_speaker_layout = sl.SPEAKER_LAYOUTS.get(output_data[1])

    return {
        ENCODING_SETTINGS: encoding_settings,
        INPUT_SPEAKER_LAYOUT: input_speaker_layout,
        OUTPUT_SPEAKER_LAYOUT: output_speaker_layout,
        COEFFICIENTS: coeffs
    }


def get_random_x_lambda(config_section):
    formats         = config_section.get("choices")
    selected_format = np.random.choice(formats)

    if selected_format == AMBISONICS:
        ambisonics_orders   = config_section.get(AMBISONICS).get(DISTRIBUTION_RANGE)
        selected_value      = np.random.choice(ambisonics_orders)

    elif selected_format == SPEAKER_LAYOUT:
        speaker_layouts_section = config_section.get(SPEAKER_LAYOUT)
        layout_names            = speaker_layouts_section.get(DISTRIBUTION_RANGE)
        selected_value          = np.random.choice(layout_names)

    else:
        raise ValueError(f"Unsupported format: {selected_format}")

    return selected_format, selected_value


def get_y_i(distribution: str, range) -> float:
    distribution = distribution.lower()
    
    if distribution == "none":
        assert(isinstance(range, float) or isinstance(range, int))
        return range
    
    elif distribution == "uniform":
        low, high = range
        return np.random.uniform(low, high)

    elif distribution == "normal":
        mean, std = range
        return np.random.normal(mean, std)

    elif distribution == "lognormal":
        mean, sigma = range
        return np.random.lognormal(mean, sigma)

    elif distribution == "beta":
        a, b = range
        return np.random.beta(a, b)

    elif distribution == "choice":
        return np.random.choice(range)

    else:
        raise ValueError(f"Unsupported distribution: {distribution}")

def speaker_layout_to_xml(parent: ET.Element, layout_list: list) -> ET.Element:
    
    for idx, speaker in enumerate(layout_list, start = 1):
        tag_name        = f"Speaker_{idx}"
        speaker_elem    = ET.SubElement(parent, tag_name)
        
        for k, v in speaker.items():
            speaker_elem.set(k, str(v))
    
    return parent


def build_xml_config(usat_state_parameters):
    state_params_xml            = ET.Element(USAT_STATE_PARAMETERS)
    encoding_settings_xml       = ET.SubElement(state_params_xml, ENCODING_SETTINGS)
    input_speaker_layout_xml    = ET.SubElement(state_params_xml, INPUT_SPEAKER_LAYOUT)
    output_speaker_layout_xml   = ET.SubElement(state_params_xml, OUTPUT_SPEAKER_LAYOUT)
    coefficients_xml            = ET.SubElement(state_params_xml, COEFFICIENTS)

    # ENCODING SETTINGS
    for key, val in usat_state_parameters.get(ENCODING_SETTINGS, {}).items():
        encoding_settings_xml.set(key, str(val))

    # SPEAKER LAYOUTS
    speaker_layout_to_xml(input_speaker_layout_xml, usat_state_parameters.get(INPUT_SPEAKER_LAYOUT, []))
    speaker_layout_to_xml(output_speaker_layout_xml, usat_state_parameters.get(OUTPUT_SPEAKER_LAYOUT, []))

    # COEFFICIENTS
    for key, val in usat_state_parameters.get(COEFFICIENTS, {}).items():
        coefficients_xml.set(key, str(val))

    return state_params_xml
    
def main():
    config_dir_name     = "config" 
    config_file_name    = "config.yaml"

    project_dir     = os.path.dirname(os.path.abspath(__file__))
    config_dir      = os.path.join(project_dir, config_dir_name)
    yaml_file       = os.path.join(config_dir, config_file_name)

    usat_state_parameters_dict  = parse_from_config(yaml_file)
    usat_state_parameters_xml   = build_xml_config(usat_state_parameters_dict)
    
    xml_bytes   = ET.tostring(usat_state_parameters_xml, encoding="utf-8", method="xml")
    xml_string  = xml_bytes.decode("utf-8") 
    print(xml_string)

if __name__ == main:
    main()