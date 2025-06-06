
import os
import time
from typing import Dict, Any
import warnings
import jax
import jax.numpy as jnp
import numpy as np
from jax import grad
from scipy.optimize import minimize

from usat_designer.processing.constants import *
from universal_transcoder.auxiliars.typing import ArrayLike
from universal_transcoder.auxiliars.typing import NpArray
from universal_transcoder.calculations.cost_function import State
from universal_transcoder.calculations.set_up_system import set_up_general

warnings.filterwarnings("ignore")
os.environ["JAX_ENABLE_X64"] = "1"

def optimize_for_usat_designer(info: Dict[str, Any]) -> dict:
        
    output_layout = info[OPT_PD_OUTPUT_LAYOUT]
    
    current_state, T_flatten_initial = set_up_general(info)

    T_flatten_optimized = bfgs_optim(
        current_state,
        T_flatten_initial,
        info[OPT_PD_SHOW_RESULTS],
        info[OPT_PD_SAVE_RESULTS],
        info[OPT_PD_RESULTS_FILE_NAME],
    )

    T_optimized = np.array(T_flatten_optimized).reshape(
        current_state.transcoding_matrix_shape
    )
    
    D = T_optimized
    if OPT_PD_DSPK in info.keys():
        D = jnp.dot(info[OPT_PD_DSPK], T_optimized)


    if OPT_PD_CLOUD_PLOTS in info:
        G       = info[OPT_PD_INPUT_MATRIX_PLOTS]
        cloud   = info[OPT_PD_CLOUD_PLOTS]

    else:
        G       = info[OPT_PD_INPUT_MATRIX_OPTIMIZATION]
        cloud   = info[OPT_PD_CLOUD_OPTIMIZATION]    


    S = jnp.dot(G, D.T)
    
    output = {
        DSN_OUT_ENCODING_MATRIX: G,
        DSN_OUT_DECODING_MATRIX: D,
        DSN_OUT_TRANSCODING_MATRIX: T_optimized,
        DSN_OUT_SPEAKER_MATRIX: S,
        DSN_OUT_OUTPUT_LAYOUT: output_layout,
        DSN_OUT_CLOUD: cloud, 
    }
    
    return output 
    

def bfgs_optim(
    current_state: State,
    flatten_initial_dec: ArrayLike,
    show_results: bool,
    save_results: bool,
    results_file_name,
) -> NpArray:
    """
    Optimization function to generate an optimized flatten transcoding matrix using
    BFGS method. It can also print through terminal or save an optimisation log

    Args:
        current_state (class State): saving cloud, input_matrix(LxM), output_layout(P)
                and transcoding_matrix shape
        flatten_initial_dec (Array): not-optimized flatten transcoding matrix from
                input format to output_layout ((NxM)x1 size)
        show_results (bool): flag to show plots and results
        save_results (bool): flag to save plots and results
        results_file_name (String or None): in case save_results=True, then it is the
                String that gives name to the folder where results are saved

    Returns:
        dec_matrix_bfgs (Array): optimized flatten transcoding matrix ((NxM)x1 size)
    """

    # Initial time
    start_time = time.time()

    # Optimization
    cost_values = []
    cost_values.append(
        current_state.cost_function(flatten_initial_dec)
    )  # starting point

    # Function to save cost value in each iteration
    def callback_func(xk):
        cost_values.append(current_state.cost_function(xk))

    # Include jit, reduces execution time
    cost_function_jit = jax.jit(current_state.cost_function)

    # Actual optimisation
    optimization_result = minimize(
        cost_function_jit,  # current_state.cost_function, if jit unwanted
        flatten_initial_dec,
        jac=grad(cost_function_jit),  # current_state.cost_function , if jit unwanted
        method="BFGS",
        callback=callback_func,
    )

    # Final flatten optimised matrix
    dec_matrix_bfgs = optimization_result.x

    # End time
    execution_time = time.time() - start_time

    print("Optimization time ", execution_time)

    return dec_matrix_bfgs