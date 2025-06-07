#!/bin/bash
set -e

PYENV_ROOT="$(pwd)/python"

echo "Setting up pyenv environment in $PYENV_ROOT"

if ! command -v pyenv &> /dev/null
then
    echo "pyenv not found. Installing via Homebrew..."
    if ! command -v brew &> /dev/null
    then
        echo "Homebrew not found. Please install Homebrew first: https://brew.sh"
        exit 1
    fi
    brew update
    brew install pyenv
fi

export PYENV_ROOT="$PYENV_ROOT"
export PATH="$PYENV_ROOT/bin:$PATH"

if command -v pyenv &> /dev/null; then
    eval "$(pyenv init --path)"
    eval "$(pyenv init -)"
else
    echo "pyenv still not found after install. Exiting."
    exit 1
fi

if pyenv versions --bare | grep -q "^3.11.1$"; then
    echo "Python 3.11.1 already installed locally."
else
    echo "Installing Python 3.11.1 locally via pyenv..."
    pyenv install 3.11.1
fi

echo "Setting local python version to 3.11.1"
pyenv local 3.11.1

echo "Python setup done."
echo "Python location: $(pyenv which python)"
echo "Python version: $(python --version)"

export PYENV_ROOT="$PYENV_ROOT"
export PATH="$PYENV_ROOT/bin:$PATH"
eval "$(pyenv init --path)"
eval "$(pyenv init -)"

python -m ensurepip --upgrade
python -m pip install --upgrade pip setuptools wheel

REQUIREMENTS_FILE="./.Scripts/requirements.txt"
TARGET_DIR="$(pwd)/python/site-packages"

mkdir -p "$TARGET_DIR"

if [ -f "$REQUIREMENTS_FILE" ]; then
    python -m pip install --upgrade --target="$TARGET_DIR" -r "$REQUIREMENTS_FILE"
else
    echo "Requirements file not found at $REQUIREMENTS_FILE, skipping Python dependencies install."
fi