
import matplotlib.pyplot as plt
import numpy as np
from universal_transcoder.auxiliars.my_coordinates import MyCoordinates
from universal_transcoder.auxiliars.typing import ArrayLike
from universal_transcoder.calculations.energy_intensity import (
    angular_error,
    width_angle,
)
from universal_transcoder.plots_and_logs.common_plots_functions import save_plot
from mpl_toolkits.basemap import Basemap
from matplotlib import rcParams
from matplotlib.colors import LinearSegmentedColormap
import base64
from io import BytesIO

def setup_plot_style():
    # Font sizes
    rcParams['axes.titlesize'] = 18
    rcParams['axes.labelsize'] = 14
    rcParams['xtick.labelsize'] = 12
    rcParams['ytick.labelsize'] = 12
    
    # Font family
    rcParams['font.family'] = 'futura'
    
    # Grid and spines
    rcParams['axes.grid'] = True
    rcParams['grid.alpha'] = 0.3
    rcParams['axes.edgecolor'] = 'gray'
    rcParams['axes.linewidth'] = 1

    # Figure background and axes background
    rcParams['figure.facecolor'] = 'white'
    rcParams['axes.facecolor'] = '#f5f5f5'
    
    # Legend font size and style
    rcParams['legend.fontsize'] = 12
    rcParams['legend.frameon'] = False
    
    # Other customizations as needed

setup_plot_style()
DEFAULT_FIGSIZE = (10, 6)
custom_cmap = LinearSegmentedColormap.from_list(
    "custom_blue_white_red", ["blue", "white", "red"]
)


def create_standard_figure(figsize=None, dpi=100):
    if figsize is None:
        figsize = DEFAULT_FIGSIZE
    fig, ax = plt.subplots(figsize=figsize, dpi=dpi)
    return fig, ax


def create_base_map(ax, x, y):
    m = Basemap(projection="robin", lon_0=0, resolution="c", ax=ax)
    x_map, y_map = m(x, y)
    
    m.drawcoastlines(linewidth=0.5, 
                     color="None")
    
    m.drawparallels(np.arange(-90.0, 120.0, 45.0), 
                    labels=[True, True, False, False],
                    labelstyle="+/-")
    
    m.drawmeridians(np.arange(0.0, 360.0, 60.0), 
                    labels=[False, False, False, True],
                    labelstyle="+/-")
    
    return m, x_map, y_map


def plot_scalar_map(
    values,
    cloud_points,
    title,
    colorbar_label,
    clim_range,
    cmap=custom_cmap,
    dpi=300,
    return_base64=True
):
    """
    Generic plot function for scalar fields on spherical maps.

    Args:
        values (ArrayLike): Scalar data to visualize.
        cloud_points (MyCoordinates): Coordinate object providing sph_deg().
        title (str): Title of the plot.
        colorbar_label (str): Label for the colorbar.
        clim_range (tuple): (min, max) range for color limits.
        save_path (str, optional): Path to save the figure.
        cmap (Colormap, optional): Matplotlib colormap to use.
    """
    points = cloud_points.sph_deg()
    x, y = points[:, 0], points[:, 1]

    fig, ax = create_standard_figure()
    _, x_map, y_map = create_base_map(ax, x, y)

    sc = ax.scatter(
        x_map,
        y_map,
        s=60 - 0.6 * np.abs(y),
        c=values,
        cmap=cmap,
        alpha=0.8,
        edgecolors="none"
    )

    plt.colorbar(sc, label=colorbar_label, ax=ax)
    ax.set_title(title)
    sc.set_clim(*clim_range)

    if return_base64:
        buffer = BytesIO()
        fig.savefig(buffer, format='png', bbox_inches='tight', dpi=dpi)
        plt.close(fig)
        buffer.seek(0)
        return base64.b64encode(buffer.read()).decode('utf-8')
    else:
        plt.show()
        return None