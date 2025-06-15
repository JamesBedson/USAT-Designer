
# USAT Designer

  

**USAT Designer** is a JUCE-based spatial audio plugin based on the Universal Spatial Audio Transcoder. It is designed to transcode between Ambisonics (up to 5th order) and arbitrary user-defined loudspeaker layouts.

  

---

  

## Features

  

-  **Bi-Directional Decoding**

Decode between any two of the following formats:

- Ambisonics (1st–5th order)
- Arbitrary user-defined speaker layouts

  ---

**Two Modes**

-  **Advanced Mode**: Full control over USAT's internal cost parameters

-  **Simplified Mode**: Perceptually motivated presets for fast results

 ---

**Visualisation**

Visualise and evaluate decodings from the following plots:

- Energy distribution

- Pressure distribution

- Radial intensity distribution

- Angular error distribution

- Source width distribution

  

-  **Preset and Session Management**

Save/load:

- Loudspeaker configurations

- Entire decoding sessions including all visualisations

---

## Setup Guide for macOS

This guide will walk you through setting up **USAT Designer** on macOS.

---

### Prerequisites

  

-  **Xcode** installed (with command line tools)

-  **Git**

-  **Homebrew**

---

### Installation

#### 1. Clone the USAT Designer repository
```bash
git  clone  https://github.com/JamesBedson/USAT-Designer.git
cd  USAT-Designer
```

#### 2. Clone JUCE v8.0.8
```bash
git  clone  --branch  8.0.8  --depth  1  https://github.com/juce-framework/JUCE.git
```

#### 3. Build the Projucer
You can find it under ```bash JUCE/extras/Projucer/Builds/MacOSX```. Open the Xcode file called ```bash Projucer.xcodeproj``` and compile the app. Once it's finished, the application will be in ```JUCE/extras/Projucer/Builds/MacOSX/build/Debug```. It is recommended to add it to your Applications folder on your Mac.

#### 4. Download Python
Run the script **download_python.sh** in the ```USAT-Designer```directory.  This will download the correct Python version for this project, based on your system architecture.
```bash
./download_python.sh
```

#### 5. Open the Project in Projucer
Open **USAT.projucer**. You might need to remove the reference to the ```Assets``` folder, as it may configured with absolute paths. 

In Projucer, click on ```File Explorer``` and navigate to the ```Assets``` folder.  Right click on it, choose "just remove references". 

After that,  copy the ```Assets``` folder in the ```USAT-Designer``` directory back into the project.

#### 6. Configure the Exporter in Projucer
Go to Exporters ---> **Xcode (macOS)**
Choose your system architecture: **arm64 / arm64e** for Apple Silicon, **x86_64** for intel
*NOTE: This setup has been tested on an M1 Mac. Use x86_64 at your own discretion.*

#### 7. Open the USAT Designer XCode Project
Click on the XCode icon at the top of the Projucer window to open the project.

#### 8. Set up the Build Scheme for VST3

Select **VST3** as the target. Click on the target again and choose:
**Edit Scheme** ---> **Build Configuration** ---> *Release*

#### **9. Build the Project
```Cmd + B``` or by click the **Run** button in XCode.

#### 10. Run the Post Build Script
From the ```USAT-Designer``` directory, execute the ```./copy_files_to_vst3.sh``` script. This copies the ```python``` folder generated earlier and the ```.Scripts``` directory to the VST3 bundle. 
```.Scripts```: Folder containing the necessary Python scripts to execute USAT in the backend.
```python```: The python runtime
```bash
./copy_files_to_vst3.sh
```
---
## Usage
To run USAT-Designer, we recommend using ```Reaper``` as a DAW host. However, this VST3 should run on any DAW that supports multichannel configurations and VST3.