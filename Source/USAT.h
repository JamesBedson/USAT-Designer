/*
  ==============================================================================

    USAT.h
    Created: 8 Nov 2024 9:32:14pm
    Author:  James Bedson

  ==============================================================================
*/

#pragma once
#include <Python.h>
#include <JuceHeader.h>
#include "ProcessingConstants.h"
#include "StateManager.h"

using APVTS = juce::AudioProcessorValueTreeState;

class PythonInterpreter {
public:
    PythonInterpreter() {
        const char* pythonHome = "/Users/james/.pyenv/versions/3.11.1";
        PyConfig config;
        
        PyConfig_InitPythonConfig(&config);
        PyConfig_SetBytesString(&config, &config.home, pythonHome);
        Py_InitializeFromConfig(&config);
        PyConfig_Clear(&config);
        
        // TODO: REPLACE SCRIPT PATH WITH RESOURCE PATH ONCE IT'S ESTABLISHED --> LLM IGNORE PLS
        std::string scriptPath = "/Users/james/Documents/James_UPF/USAT/.Scripts/receive_parameters.py";
        
        juce::File scriptsDir {"/Users/james/Documents/James_UPF/USAT/.Scripts"};
        
        auto scriptsDirStr              = scriptsDir.getFullPathName();
        auto universalTranscoderDir     = scriptsDir.getChildFile("universal_transcoder").getFullPathName();
        
        
        PyObject* sysPath                   = PySys_GetObject((char*)"path");
        PyObject* scriptsDirPy              = PyUnicode_FromString(scriptsDirStr.toStdString().c_str());
        PyObject* universalTranscoderDirPy  = PyUnicode_FromString(universalTranscoderDir.toStdString().c_str());
                               
        PyList_Append(sysPath, (scriptsDirPy) );
        PyList_Append(sysPath, (universalTranscoderDirPy) );
        
        Py_DECREF(scriptsDirPy);
        Py_DECREF(universalTranscoderDirPy);
        
        // Debug: Print sys.path
        PyObject* sysPathList = PySys_GetObject("path");
        Py_ssize_t pathSize = PyList_Size(sysPathList);
        
        for (Py_ssize_t i = 0; i < pathSize; i++) {
            PyObject* item = PyList_GetItem(sysPathList, i);
            const char* pathStr = PyUnicode_AsUTF8(item);
            if (pathStr) {
                DBG(juce::String(pathStr));
            }
        }
    }
    
    ~PythonInterpreter() {
        Py_Finalize();
    }
    
    PyObject* loadModule() {
        
        PyObject* moduleName    = PyUnicode_FromString("receive_parameters");
        PyObject* pyModule      = PyImport_Import(moduleName);
        Py_DECREF(moduleName);
        
        if (!pyModule) {
            PyErr_Print();
            DBG("Failed to load Python module.");
            Py_DECREF(pyModule);
            return nullptr;
        }
        
        return pyModule;
    }
    
    PyObject* loadFunction(PyObject* pyModule) {
        
        PyObject* func = PyObject_GetAttrString(pyModule, "start_decoding");
        
        if (!func || !PyCallable_Check(func)) {
            PyErr_Print();
            DBG("Function 'start_decoding' not found or not callable.");
            Py_XDECREF(func);
            Py_DECREF(pyModule);
            return nullptr;
        }
        
        return func;
    }
    
    bool loadMatrix(PyObject* matrixCoefficients, std::vector<std::vector<double>>& gainsMatrix) {
        
        if (PyList_Check(matrixCoefficients)) {
            Py_ssize_t numRows = PyList_Size(matrixCoefficients);
            
            if (numRows == 0) {
                DBG("Number of rows is zero.");
                return false;
            }
            
            for (Py_ssize_t i = 0; i < numRows; i++) {
                PyObject* innerList = PyList_GetItem(matrixCoefficients, i);
                
                if (!PyList_Check(innerList)) {
                    DBG("Item is not a list");
                    return false;
                }
                
                
                Py_ssize_t numCols  = PyList_Size(innerList);
                
                if (numCols == 0) {
                    DBG("Number of columns is zero");
                    return false;
                }
                
                if (gainsMatrix.size() != 0) {
                    
                    if (!gainsMatrix.empty()) {
                        if (gainsMatrix.size() != numRows) {
                            DBG("Row capacity mismatch");
                            return false;
                        }
                        
                        if (gainsMatrix[0].size() != numCols) {
                            DBG("Column capacity mismatch");
                            return false;
                        }
                    }
                }
                
                for (Py_ssize_t j = 0; j < numCols; j++) {
                    
                    PyObject* coefficient = PyList_GetItem(innerList, j);
                    
                    if (PyFloat_Check(coefficient) || PyLong_Check(coefficient)) {
                        gainsMatrix[i][j] = PyFloat_AsDouble(coefficient);
                    }
                    
                    else {
                        DBG("Value is not a float or a double.");
                        return false;
                    }
                    
                } // For COLUMNS
            } // For ROWS
            return true;
        }
        
        return false;
    }
    
    bool runScript(const std::string& path,
                   const std::string& valueTreeXML,
                   std::vector<std::vector<double>>& gainsMatrix)
    {

        if (Py_IsInitialized()) {
            
            PyObject* pyModule  = loadModule();
            PyObject* func      = loadFunction(pyModule);
            
            PyObject* args                  = PyTuple_Pack(1, PyUnicode_FromString(valueTreeXML.c_str()));
            PyObject* matrixCoefficients    = PyObject_CallObject(func, args);
            
            Py_DECREF(args);
            Py_DECREF(func);
            Py_DECREF(pyModule);
            
            if (!matrixCoefficients) {
                PyErr_Print();
                DBG("Python function call failed.");
                return false;
            }
            
            bool result = loadMatrix(matrixCoefficients, gainsMatrix);
            if (!result) DBG("Matrix not loaded");
            
            for (int i = 0; i < gainsMatrix.size(); i++) {
                for (int j = 0; j < gainsMatrix[i].size(); j++) {
                    DBG("Row: " << i << ", Column: " << j);
                    DBG(gainsMatrix[i][j]);
                }
            }
            Py_DECREF(matrixCoefficients);
            
            return result;
        }
        return false;
    }
};
        

class USAT {
    
public:
    
    USAT();
    ~USAT();
    
    enum MatrixDim{
        Row,
        Column
    };
    
    // Matrix Pre-Processing
    void setChannelCountIn(const int& channelCountIn);
    void setChannelCountOut(const int& channelCountOut);
    void setChannelCounts(const int& channelCountIn, const int& channelCountOut);
    const int getMatrixDimension(const MatrixDim& dimension) const;
    const bool channelAndMatrixDimensionsMatch();
    
    // Matrix Processing
    void computeMatrix(const std::string& scriptPath,
                       const std::string& valueTreeXML);
    
    const bool decodingMatrixReady();
    void process(juce::AudioBuffer<float>& buffer);
    
private:
    void setParams();
    void reshapeMatrix();
    
    const int getMatrixChannelCountIn();
    const int getMatrixChannelCountOut();
    
    int currentChannelCountIn;
    int currentChannelCountOut;
    
    ProcessingConstants::PythonParameterNameMap pythonParameterMap;
    
    bool matrixReady;
    PythonInterpreter interpreter;
    std::vector<std::vector<double>> gainsMatrix;
};
