# plot_worker.py
import base64
import numpy as np
from io import BytesIO
from matplotlib import pyplot as plt
from matplotlib.colors import LinearSegmentedColormap
from usat_designer.processing.plots_usat_designer import *
from usat_designer.processing.constants import *
from universal_transcoder.calculations.energy_intensity import (
    energy_calculation,
    radial_I_calculation,
    transverse_I_calculation,
    angular_error,
    width_angle
)

def generate_base64_plots_mp(optimisation_data: dict) -> dict:
    S               = np.array(optimisation_data[DSN_OUT_SPEAKER_MATRIX])
    cloud           = optimisation_data[DSN_OUT_CLOUD]
    output_layout   = optimisation_data[DSN_OUT_OUTPUT_LAYOUT]
    
    energy          = energy_calculation(S)
    radial_i        = radial_I_calculation(cloud, S, output_layout)
    transverse_i    = transverse_I_calculation(cloud, S, output_layout)
    ae              = angular_error(radial_i, transverse_i)
    source_width    = width_angle(radial_i)

    colors = [DSN_PLT_GRADIENT_COOL, DSN_PLT_GRANDIENT_NEUTRAL, DSN_PLT_GRADIENT_WARM]
    cmap = LinearSegmentedColormap.from_list("custom_coolwarm", colors)

    return {
        DSN_PLT_ENERGY: plot_scalar_map(energy, cloud, "Energy", "Energy", (0, 2), cmap),
        DSN_PLT_RADIAL_INTENSITY: plot_scalar_map(radial_i, cloud, "Radial Intensity", "Radial Intensity", (0, 1), cmap),
        DSN_PLT_TRANSVERSE_INTENSITY: plot_scalar_map(transverse_i, cloud, "Transverse Intensity", "Transverse Intensity", (0, 1), cmap),
        DSN_PLT_ANGULAR_ERROR: plot_scalar_map(ae, cloud, "Angular Error", "Angular Error (Degrees)", (0, 45), cmap),
        DSN_PLT_SOURCE_WIDTH: plot_scalar_map(source_width, cloud, "Source Width", "Source Width (Degrees)", (0, 45), cmap),
    }
