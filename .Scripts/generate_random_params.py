import numpy as np
import speaker_layouts as sl
import xml.etree.ElementTree as ET
import xml.dom.minidom as minidom
import yaml
from processing_constants import *
import receive_parameters, parameter_utils as pu
import speaker_layouts as sl
import os
import time
import traceback
import argparse

def parse_from_config(yaml_file):

    with open(yaml_file, "r") as file:
        config = yaml.safe_load(file)
    
    # COEFFICIENTS
    coeff_config = config.get(COEFFICIENTS, {})
    coeffs = {}

    for coeff_name, coeff_data in coeff_config.items():
        coeff_distribution = coeff_data.get(DISTRIBUTION)
        coeff_range        = coeff_data.get(DISTRIBUTION_RANGE)
        coeff_value        = round(get_y_i(coeff_distribution, coeff_range))
        coeffs[coeff_name] = coeff_value

    # FORMATS
    default_ambisonics_order = 1
    
    input_data  = get_random_x_lambda(config[INPUT_FORMAT])
    output_data = get_random_x_lambda(config[OUTPUT_FORMAT])

    encoding_settings = {
        INPUT_TYPE: input_data[0],
        OUTPUT_TYPE: output_data[0]
    }

    input_ambisonics    = {AMBISONICS_ORDER_IN: default_ambisonics_order}
    output_ambisonics   = {AMBISONICS_ORDER_OUT: default_ambisonics_order}

    input_speaker_layout    = []
    output_speaker_layout   = []
    input_layout_desc       = ""
    output_layout_desc      = ""

    if input_data[0] == AMBISONICS:
        input_ambisonics[AMBISONICS_ORDER_IN] = input_data[1]

    elif input_data[0] == SPEAKER_LAYOUT:
        input_layout_desc = input_data[1]
        input_speaker_layout = sl.SPEAKER_LAYOUTS.get(input_layout_desc)
        if input_speaker_layout is None:
            print(f"Warning: input layout '{input_layout_desc}' not found!")
    else:
        raise ValueError("Input format not supported.")

    if output_data[0] == AMBISONICS:
        output_ambisonics[AMBISONICS_ORDER_OUT] = output_data[1]

    elif output_data[0] == SPEAKER_LAYOUT:
        output_layout_desc = output_data[1]
        output_speaker_layout = sl.SPEAKER_LAYOUTS.get(output_layout_desc)
        if output_speaker_layout is None:
            print(f"Warning: output layout '{output_layout_desc}' not found!")

    else:
        raise ValueError("Output format not supported.")
    
    return {
        ENCODING_SETTINGS: encoding_settings,
        INPUT_AMBISONICS: input_ambisonics,
        OUTPUT_AMBISONICS: output_ambisonics,
        INPUT_SPEAKER_LAYOUT: input_speaker_layout,
        OUTPUT_SPEAKER_LAYOUT: output_speaker_layout,
        INPUT_LAYOUT_DESC: input_layout_desc,
        OUTPUT_LAYOUT_DESC: output_layout_desc,
        COEFFICIENTS: coeffs
    }

def get_random_x_lambda(config_section):
    formats         = config_section.get(FORMAT_CHOICES)
    selected_format = np.random.choice(formats)

    if selected_format == AMBISONICS:
        ambisonics_orders   = config_section.get(AMBISONICS).get(DISTRIBUTION_RANGE)
        selected_value      = np.random.choice(ambisonics_orders)

    elif selected_format == SPEAKER_LAYOUT:
        layout_names        = config_section.get(SPEAKER_LAYOUT).get(DISTRIBUTION_RANGE)
        selected_value      = np.random.choice(layout_names)

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

        sample = -1
        while sample < 0:
            sample = int(round(np.random.normal(mean, std)))
        return sample

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


def build_xml_config(usat_state_parameters):
    state_params_xml            = ET.Element(USAT_STATE_PARAMETERS)
    encoding_settings_xml       = ET.SubElement(state_params_xml, ENCODING_SETTINGS)
    input_ambisonics_xml        = ET.SubElement(state_params_xml, INPUT_AMBISONICS)
    output_ambisonics_xml       = ET.SubElement(state_params_xml, OUTPUT_AMBISONICS)
    input_speaker_layout_xml    = ET.SubElement(state_params_xml, INPUT_SPEAKER_LAYOUT)
    output_speaker_layout_xml   = ET.SubElement(state_params_xml, OUTPUT_SPEAKER_LAYOUT)
    input_layout_desc_xml       = ET.SubElement(state_params_xml, INPUT_LAYOUT_DESC)
    output_layout_desc_xml      = ET.SubElement(state_params_xml, OUTPUT_LAYOUT_DESC)
    coefficients_xml            = ET.SubElement(state_params_xml, COEFFICIENTS)

    # ENCODING SETTINGS
    for key, val in usat_state_parameters.get(ENCODING_SETTINGS, {}).items():
        encoding_settings_xml.set(key, str(val))

    for key, val in usat_state_parameters.get(INPUT_AMBISONICS, {}).items():
        input_ambisonics_xml.set(key, str(val))

    for key, val in usat_state_parameters.get(OUTPUT_AMBISONICS, {}).items():
        output_ambisonics_xml.set(key, str(val))

    input_layout_desc_xml.set(INPUT_LAYOUT_DESC, usat_state_parameters[INPUT_LAYOUT_DESC])
    output_layout_desc_xml.set(OUTPUT_LAYOUT_DESC, usat_state_parameters[OUTPUT_LAYOUT_DESC])

    # SPEAKER LAYOUTS
    pu.speaker_layout_to_xml(input_speaker_layout_xml, usat_state_parameters.get(INPUT_SPEAKER_LAYOUT, []))
    pu.speaker_layout_to_xml(output_speaker_layout_xml, usat_state_parameters.get(OUTPUT_SPEAKER_LAYOUT, []))

    # COEFFICIENTS
    for key, val in usat_state_parameters.get(COEFFICIENTS, {}).items():
        coefficients_xml.set(key, str(val))

    return state_params_xml
    

def generate_decoding_data(args):
    import numpy as np
    import warnings 
    import os

    yaml_file, seed = args
    if seed is not None:
        np.random.seed(seed)

    pretty_xml = None 

    print(f"Running in PID {os.getpid()} with seed {seed}")
    
    try:
        usat_state_parameters_dict  = parse_from_config(yaml_file)
        usat_state_parameters_xml   = build_xml_config(usat_state_parameters_dict)
        
        xml_bytes   = ET.tostring(usat_state_parameters_xml, encoding="utf-8", method="xml")
        parsed_xml  = minidom.parseString(xml_bytes)
        pretty_xml  = str(parsed_xml.toprettyxml(indent="  "))
        
        warnings.filterwarnings("ignore")
        output_dict = receive_parameters.decode_for_random_parameter_generation(pretty_xml)
        return pretty_xml, output_dict
    
    except Exception as e:
        tb_str = traceback.format_exc()
        print(f"Error in PID {os.getpid()} with seed {seed}: {e}")
        print(f"Traceback:\n{tb_str}")
        output_dict = {
            "error_message": e,
            "traceback": tb_str 
        }
        return pretty_xml, output_dict
    

def main(num_decodings_targeted):
    config_dir_name     = "config" 
    config_file_name    = "test.yaml"
    base_dir            = "random_usat_decodings"
    
    project_dir     = os.path.dirname(os.path.abspath(__file__))
    config_dir      = os.path.join(project_dir, config_dir_name)
    yaml_file       = os.path.join(config_dir, config_file_name)

    seed                    = int((os.getpid() * time.time()) % (2**32))
    start_time              = time.time()
    
    for i in range(1, num_decodings_targeted + 1):
        seed                = int(os.getpid() * time.time() * i % (2 ** 32))
        xml, output_dict    = generate_decoding_data((yaml_file, seed))
        
        assert(isinstance(xml, str))
        pu.save_output_data(xml, output_dict, seed, base_dir)

    end_time    = time.time()
    elapsed     = end_time - start_time
    print(f"Elapsed time: {elapsed}")  

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generate random decoding data.")
    parser.add_argument(
        "-n", "--num",
        type=int,
        default=10,
        help="Number of decodings to generate (default: 10)"
    )
    args = parser.parse_args()
    main(args.num)
