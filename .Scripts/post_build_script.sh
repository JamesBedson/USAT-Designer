#!/bin/bash
set -e
set -x

APP_NAME="USAT"

# Project Root
CURRENT_DIR="$(pwd)"
PROJECT_ROOT="$(cd "$CURRENT_DIR/../.." && pwd)"

# Source Directory
SCRIPTS_SRC="$PROJECT_ROOT/.Scripts"
PYTHON_SRC="$PROJECT_ROOT/python"

# Destination App Bundle
APP_BUNDLE="$BUILT_PRODUCTS_DIR/$APP_NAME.app"

if [ ! -d "$APP_BUNDLE" ]; then
    echo "Error: App bundle not found at $APP_BUNDLE"
    exit 1
fi

# Resources Directory
RESOURCES_DIR="$APP_BUNDLE/Contents/Resources"
mkdir -p "$RESOURCES_DIR"

cp -R "$SCRIPTS_SRC" "$RESOURCES_DIR/"
cp -R "$PYTHON_SRC" "$RESOURCES_DIR/"

echo "Copied resources to $APP_NAME.app"
