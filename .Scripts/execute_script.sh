#!/bin/bash

# Path to the XML file
XML_FILE="test_params.xml"

# Read the entire XML file into a string
XML_STRING=$(<"$XML_FILE")

# Call the Python script and pass the XML string as an argument
# Using `--xml` as a named argument for clarity
python3 receive_parameters.py "$XML_STRING"