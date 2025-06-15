#!/bin/bash
set -e
set -x

APP_NAME="USAT"

PLUGIN_BUILD_DIR_RELATIVE_PATH="/Builds/MacOSX/build/Release/USAT.vst3"
VST3_BUNDLE_RELATIVE_PATH="$PLUGIN_BUILD_DIR_RELATIVE_PATH/${APP_NAME}.vst3"

SCRIPTS_SRC_RELATIVE_PATH=".Scripts"
PYTHON_SRC_RELATIVE_PATH="python"

# Resolving absolute paths (just in case)
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PLUGIN_BUILD_DIR="$(cd "$SCRIPT_DIR/$PLUGIN_BUILD_DIR_RELATIVE_PATH" && pwd)"
VST3_BUNDLE="$PLUGIN_BUILD_DIR"

SCRIPTS_SRC="$SCRIPT_DIR/$SCRIPTS_SRC_RELATIVE_PATH"
PYTHON_SRC="$SCRIPT_DIR/$PYTHON_SRC_RELATIVE_PATH"

if [ ! -d "$VST3_BUNDLE" ]; then
    echo "Error: VST3 bundle not found at $VST3_BUNDLE"
    exit 1
fi

# Copying directories to resources folder
RESOURCES_DIR="$VST3_BUNDLE/Contents/Resources"
mkdir -p "$RESOURCES_DIR"

cp -R "$SCRIPTS_SRC" "$RESOURCES_DIR/"
cp -R "$PYTHON_SRC" "$RESOURCES_DIR/"

echo "Copied resources to $APP_NAME.vst3 bundle"

# Copying to VST Location on MacOS
USER_VST3_DIR="$HOME/Library/Audio/Plug-Ins/VST3"
mkdir -p "$USER_VST3_DIR"

cp -R "$VST3_BUNDLE" "$USER_VST3_DIR/"

echo "Copied $APP_NAME.vst3 bundle to $USER_VST3_DIR"
