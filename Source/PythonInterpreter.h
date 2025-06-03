/*
  ==============================================================================

    PythonInterpreter.h
    Created: 10 Apr 2025 3:46:34pm
    Author:  James Bedson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "GainMatrix.h"

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
        
        if (!_Py_IsFinalizing()) {
            ScopedGILGuard gil;

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
    
    bool loadMatrix(PyObject* matrixCoefficients, GainMatrix& gainsMatrix) {
        
        if (PyList_Check(matrixCoefficients)) {
            Py_ssize_t numInputChannels = PyList_Size(matrixCoefficients);
            
            if (numInputChannels == 0) {
                DBG("Number of rows is zero.");
                return false;
            }
            
            gainsMatrix.setNumInputChannels(static_cast<int>(numInputChannels));
    
            for (Py_ssize_t i = 0; i < numInputChannels; i++) {
                PyObject* innerList = PyList_GetItem(matrixCoefficients, i);
                
                if (!PyList_Check(innerList)) {
                    DBG("Item is not a list");
                    return false;
                }
                
                Py_ssize_t numOutputs  = PyList_Size(innerList);
                gainsMatrix.setNumOutputChannelsForRow(static_cast<int>(i), static_cast<int>(numOutputs));
                
                if (numOutputs == 0) {
                    DBG("Number of columns is zero");
                    return false;
                }
                
                for (Py_ssize_t j = 0; j < numOutputs; j++) {
                    
                    PyObject* coefficient = PyList_GetItem(innerList, j);
                    
                    if (PyFloat_Check(coefficient) || PyLong_Check(coefficient)) {
                        gainsMatrix.assign(static_cast<int>(i), static_cast<int>(j), PyFloat_AsDouble(coefficient));
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
    
    PyObject* makeProgressCallback(std::function<void(float)> cppCallback) {
        
        struct CallbackData {
            std::function<void(float)> callback;
        };
        
        auto* data = new CallbackData {cppCallback};
        
        PyObject* capsule = PyCapsule_New(data,
                                          nullptr,
                                          [](PyObject* capsule)
        {
            delete static_cast<CallbackData*>(PyCapsule_GetPointer(capsule, nullptr));
        });
        
        static PyMethodDef def = {
            "progress_callback",
            [](PyObject* self, PyObject* args) -> PyObject* {
                
                auto* data = static_cast<CallbackData*>(PyCapsule_GetPointer(self, nullptr));
                if (!data) return nullptr;
                
                double value;
                if (!PyArg_ParseTuple(args, "d", &value))
                    return nullptr;
                
                data->callback(static_cast<float>(value));
                Py_RETURN_NONE;
            },
            METH_VARARGS,
            nullptr
        };
        
        return PyCFunction_New(&def, capsule);
    }
    
    PyObject* makeStatusCallback(std::function<void(std::string)> cppCallback) {
        
        struct CallbackData {
            std::function<void(std::string)> callback;
        };
        
        auto* data = new CallbackData {cppCallback};
        
        PyObject* capsule = PyCapsule_New(data,
                                          nullptr,
                                          [](PyObject* capsule)
        {
            delete static_cast<CallbackData*>(PyCapsule_GetPointer(capsule, nullptr));
        });
        
        static PyMethodDef def = {
            "status_callback",
            [](PyObject* self, PyObject* args) -> PyObject* {
                auto* data = static_cast<CallbackData*>(PyCapsule_GetPointer(self, nullptr));
                if (!data) return nullptr;

                const char* message;
                if (!PyArg_ParseTuple(args, "s", &message))
                    return nullptr;
                
                data->callback(message);
                Py_RETURN_NONE;
            },
            METH_VARARGS,
            nullptr
        };

        return PyCFunction_New(&def, capsule);
    }
    
    bool runScript(const std::string& valueTreeXML,
                   GainMatrix& gainsMatrix,
                   std::function<void(float)> progressCallback = nullptr,
                   std::function<void(std::string)> statusCallback = nullptr)
    {
        ScopedGILGuard gil;
        
        if (Py_IsInitialized()) {
            
            PyObject* func  = loadFunction(receiveParametersModule);
            
            PyObject* pyProgressCallback = progressCallback
                ? makeProgressCallback(progressCallback)
                : Py_None;
            
            PyObject* pyStatusCallback = statusCallback
                ? makeStatusCallback(statusCallback)
                : Py_None;
                        
            Py_INCREF(pyProgressCallback);
            Py_INCREF(pyStatusCallback);
            
            PyObject* args                  = PyTuple_Pack(3,
                                                           PyUnicode_FromString(valueTreeXML.c_str()),
                                                           pyProgressCallback,
                                                           pyStatusCallback);
            
            PyObject* matrixCoefficients    = PyObject_CallObject(func, args);
            
            Py_DECREF(args);
            Py_DECREF(func);
            Py_DECREF(pyProgressCallback);
            Py_DECREF(pyStatusCallback);
            
            if (!matrixCoefficients) {
                PyErr_Print();
                DBG("Python function call failed.");
                return false;
            }
            
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
    
    using OnDoneCallback        = std::function<void()>;
    using OnProgressCallback    = std::function<void(float)>;
    using OnStatusCallback      = std::function<void(std::string)>;
    
    PythonThread(PythonInterpreter& pyReference,
                 GainMatrix& gainsMatrix)
    
    : juce::Thread ("Python Thread"),
    pyRef(pyReference),
    gainsMatrixRef(gainsMatrix)
    {
        
    }
    
    ~PythonThread() {
        stopThread(1000);
    }
    
    void setOnDoneCallback(OnDoneCallback callback) {
        onDone      = std::move(callback);
    }
    
    void setOnProgressCallback(OnProgressCallback callback) {
        onProgress  = std::move(callback);
    }
    
    void setOnStatusCallback(OnStatusCallback callback) {
        onStatus    = std::move(callback);
    }
    
    void run() override
    {
        
        if (valueTreeXML.empty()) {
            DBG("Value Tree does not contain info.");
            return;
        }
        
        bool success = pyRef.runScript(valueTreeXML,
                                       gainsMatrixRef,
                                       onProgress,
                                       onStatus);
        
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
    
    juce::Value progress;
    
private:
    PythonInterpreter& pyRef;
    GainMatrix& gainsMatrixRef;
    std::string valueTreeXML;
    
    OnDoneCallback      onDone;
    OnProgressCallback  onProgress;
    OnStatusCallback    onStatus;
};
