from universal_transcoder.auxiliars.my_coordinates import MyCoordinates
from receive_parameters import parse_coefficients, create_speaker_layout
from processing_constants import *

import numpy as np
import os
import json
import xml.etree.ElementTree as ET

def speaker_layout_to_xml(parent: ET.Element, layout_list: list) -> ET.Element:
    
    for idx, speaker in enumerate(layout_list, start = 1):
        tag_name        = f"Speaker_{idx}"
        speaker_elem    = ET.SubElement(parent, tag_name)
        
        for k, v in speaker.items():
            speaker_elem.set(k, str(v))
    
    return parent

def serialize_coordinates(obj):
    if isinstance(obj, MyCoordinates):
        return {
            "_type": "MyCoordinates",
            "spherical_deg": obj.sph_deg().tolist()
        }
    elif isinstance(obj, list):
        return [serialize_coordinates(o) for o in obj]
    elif isinstance(obj, dict):
        return {k: serialize_coordinates(v) for k, v in obj.items()}
    else:
        return obj
    
def restore_coordinates(serialized):
    if isinstance(serialized, dict) and serialized.get("_type") == "MyCoordinates":
        sph_deg = np.array(serialized["spherical_deg"])
        return MyCoordinates.mult_points(sph_deg)
    elif isinstance(serialized, list):
        return [restore_coordinates(v) for v in serialized]
    elif isinstance(serialized, dict):
        return {k: restore_coordinates(v) for k, v in serialized.items()}
    else:
        return serialized
    
def save_output_data(xml_string: str,
                    output_dict: dict,
                    seed: float,
                    base_dir: str):

    os.makedirs(base_dir, exist_ok=True)

    output_dir = os.path.join(base_dir, f"usat_{seed}")
    os.makedirs(output_dir, exist_ok=True)

    xml_path = os.path.join(output_dir, f"y_parameters_{seed}.xml")

    with open(xml_path, "w", encoding="utf-8") as f:
        f.write(xml_string)

    if "error_message" in output_dict:
        error_path = os.path.join(output_dir, "error.txt")
        
        with open(error_path, "w", encoding="utf-8") as f:
            f.write(output_dict["error_message"])
            f.write(output_dict["traceback"])

    else:
        matrix_path = os.path.join(output_dir, f"matrix_data_{seed}.npz")
        
        np.savez(matrix_path, 
                    G = output_dict["G"],
                    T = output_dict["T_optimized"],
                    D = output_dict["D"],
                    S = output_dict["S"])
        
        metadata = {
            "seed": seed,
            "output_layout": serialize_coordinates(output_dict["output_layout"]),
            "cloud_plots": serialize_coordinates(output_dict["cloud_plots"]),
            "cloud_optimization": serialize_coordinates(output_dict["cloud_optimization"]),
        }

        metadata_path = os.path.join(output_dir, f"metadata_{seed}.json")
        with open(metadata_path, "w") as f:
            json.dump(metadata, f, indent=2)
    return

import xml.etree.ElementTree as ET

def create_speaker_dict(speaker_layout):
    speakers = []
    if speaker_layout is not None:
        for speaker in speaker_layout:
            speakers.append({
                "id": int(speaker.attrib.get("ID", 0)),
                "azimuth": float(speaker.attrib.get("Azimuth", 0.0)),
                "elevation": float(speaker.attrib.get("Elevation", 0.0)),
                "distance": float(speaker.attrib.get("Distance", 1.0)),
            })


def usat_xml_to_dict(xml_string: str):
    
    root = ET.fromstring(xml_string)
    data = {}

    encoding_settings       = root.find(ENCODING_SETTINGS)
    data[ENCODING_SETTINGS] = encoding_settings.attrib if encoding_settings is not None else {}

    input_ambisonics        = root.find(INPUT_AMBISONICS)
    data[INPUT_AMBISONICS]  = input_ambisonics.attrib if input_ambisonics is not None else {}

    output_ambisonics       = root.find(OUTPUT_AMBISONICS)
    data[OUTPUT_AMBISONICS] = output_ambisonics.attrib if output_ambisonics is not None else {}

    input_layout_desc       = root.find(INPUT_LAYOUT_DESC)
    data[INPUT_LAYOUT_DESC] = input_layout_desc.attrib if input_layout_desc is not None else {}

    output_layout_desc          = root.find(OUTPUT_LAYOUT_DESC)
    data[OUTPUT_LAYOUT_DESC]    = output_layout_desc.attrib if output_layout_desc is not None else {}

    data[INPUT_SPEAKER_LAYOUT]  = []
    data[OUTPUT_SPEAKER_LAYOUT] = []
    
    if data[ENCODING_SETTINGS][INPUT_TYPE] == SPEAKER_LAYOUT:
        input_speaker_layout = root.find(INPUT_SPEAKER_LAYOUT)
        assert(input_speaker_layout is not None)
        data[INPUT_SPEAKER_LAYOUT] = create_speaker_layout(input_speaker_layout)

    if data[ENCODING_SETTINGS][OUTPUT_TYPE] == SPEAKER_LAYOUT:
        output_speaker_layout = root.find(OUTPUT_SPEAKER_LAYOUT)
        assert(output_speaker_layout is not None)
        data[OUTPUT_SPEAKER_LAYOUT] = create_speaker_layout(output_speaker_layout) 
    
    coefficients_xml = root.find(COEFFICIENTS)
    assert(coefficients_xml is not None)
    data[COEFFICIENTS] = parse_coefficients(coefficients_xml)

    return data
