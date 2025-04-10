/*
  ==============================================================================

    PythonInterpreter.h
    Created: 10 Apr 2025 3:46:34pm
    Author:  James Bedson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

using Matrix    = std::vector<std::vector<double>>;

class ScopedGILGuard {
public:
    ScopedGILGuard() {
        gstate = PyGILState_Ensure();
    }

    ~ScopedGILGuard() {
        PyGILState_Release(gstate);
    }

    ScopedGILGuard(const ScopedGILGuard&) = delete;
    ScopedGILGuard& operator=(const ScopedGILGuard&) = delete;

private:
    PyGILState_STATE gstate;
};

class PythonInterpreter {
public:
    
    PythonInterpreter() {
        const char* pythonHome = "/Users/james/.pyenv/versions/3.11.1";
        PyConfig config;
        config.write_bytecode = 0;
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
        
        numpyModule             = loadModule("numpy");
        myCoordinatesModule     = loadModule("universal_transcoder.auxiliars.my_coordinates");
        optimisationModule      = loadModule("universal_transcoder.calculations.optimization");
        receiveParametersModule = loadModule("receive_parameters");
        
        Py_DECREF(scriptsDirPy);
        Py_DECREF(universalTranscoderDirPy);
        PyEval_SaveThread();
        
        DBG("Python initialised...");
        
    }
    
    ~PythonInterpreter() {
        // Check if Python is finalizing — don’t touch Python if it's already dying
        if (!_Py_IsFinalizing()) {
            ScopedGILGuard gil;  // Only safe if Python is not finalizing

            if (numpyModule)
                Py_DECREF(numpyModule);
            if (myCoordinatesModule)
                Py_DECREF(myCoordinatesModule);
            if (optimisationModule)
                Py_DECREF(optimisationModule);
            if (receiveParametersModule)
                Py_DECREF(receiveParametersModule);
        }
    }
    
    PyObject* loadModule(std::string name) {
        
        PyObject* moduleName    = PyUnicode_FromString(name.c_str());
        PyObject* pyModule      = PyImport_Import(moduleName);
        
        Py_DECREF(moduleName);
        
        if (!pyModule) {
            PyErr_Print();
            DBG("Failed to load Python module: " << name);
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
            gainsMatrix.resize(numRows);
            
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
                gainsMatrix[i].resize(numCols);
                
                if (numCols == 0) {
                    DBG("Number of columns is zero");
                    return false;
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
    
    bool runScript(const std::string& valueTreeXML,
                   std::vector<std::vector<double>>& gainsMatrix)
    {
        ScopedGILGuard gil;
        
        if (Py_IsInitialized()) {
            
            DBG("Entered run script");
            
            PyObject* func      = loadFunction(receiveParametersModule);
            PyObject* args      = PyTuple_Pack(1, PyUnicode_FromString(valueTreeXML.c_str()));
            
            DBG("Calling function...");
            PyObject* matrixCoefficients = PyObject_CallObject(func, args);
            
            Py_DECREF(args);
            Py_DECREF(func);
            
            if (!matrixCoefficients) {
                PyErr_Print();
                DBG("Python function call failed.");
                return false;
            }
            
            DBG("Loading Matrix...");
            bool result = loadMatrix(matrixCoefficients, gainsMatrix);
            if (!result) DBG("Matrix not loaded");
            
            Py_DECREF(matrixCoefficients);

            return result;
        }
        return false;
    }
    
private:
    
    PyObject* numpyModule;
    PyObject* myCoordinatesModule;
    PyObject* optimisationModule;
    PyObject* receiveParametersModule;
    
};


class PythonThread : public juce::Thread {
  
public:
    
    using OnDoneCallback = std::function<void()>;
    
    PythonThread(PythonInterpreter& pyReference,
                 Matrix& gainsMatrix)
    
    : juce::Thread ("Python Thread"),
    pyRef(pyReference),
    gainsMatrixRef(gainsMatrix)
    {
        
    }
    
    ~PythonThread() {
        stopThread(1000);
    }
    
    void setOnDoneCallback(OnDoneCallback callback)
    {
        onDone = std::move(callback);
    }
    
    void run() override
    {
        DBG("Starting thread...");
        if (valueTreeXML.empty()) {
            DBG("Value Tree does not contain info.");
            return;
        }
        
        bool success = pyRef.runScript(valueTreeXML, gainsMatrixRef);
        DBG("Ran script...");
        if (success && onDone) {
            juce::MessageManager::callAsync([this]() {
                onDone();
            });
        }
        else {
            DBG("Python Execution failed.");
        }
        
        signalThreadShouldExit();
    }
    
    void setNewValueTree(const std::string& valueTreeXML) {
        this->valueTreeXML = valueTreeXML;
    }
    
private:
    PythonInterpreter& pyRef;
    Matrix& gainsMatrixRef;
    std::string valueTreeXML;
    
    OnDoneCallback onDone;
};
