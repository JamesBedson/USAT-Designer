#pragma once
#include <JuceHeader.h>

using Matrix = std::vector<std::vector<float>>;

class GainMatrix {

public:
    GainMatrix() = default;

    void setNumChannels(int numInputChannels, int numOutputChannels) {
        buffer.resize(numInputChannels);
        for (auto& row : buffer)
            row.resize(numOutputChannels, 0.0f);
    }

    void setNumInputChannels(int numInputs) {
        int currentOutputs = getNumOutputChannels();
        buffer.resize(numInputs);
        for (auto& row : buffer)
            row.resize(currentOutputs, 0.0f);
    }

    void setNumOutputChannels(int numOutputs) {
        for (auto& row : buffer)
            row.resize(numOutputs, 0.0f);
    }
    
    void setNumOutputChannelsForRow(int idx, int numOutputs) {
        if (idx >= 0 && idx < buffer.size()) {
            buffer[idx].resize(numOutputs, 0.0f);
        } else {
            DBG("setNumOutputChannelsAtIndex: Index out of bounds (" << idx << ")");
        }
    }
    
    void assign(int rowIdx, int columnIdx, double value) {
        if (rowIdx >= 0 && rowIdx < buffer.size()) {
            if (columnIdx >= 0 && columnIdx < buffer[rowIdx].size()) {
                buffer[rowIdx][columnIdx] = static_cast<float>(value);
            } else {
                DBG("assign: column index out of bounds (" << columnIdx << ")");
            }
        } else {
            DBG("assign: row index out of bounds (" << rowIdx << ")");
        }
    }
    
    const float get(int rowIdx, int columnIdx) const {
        if (rowIdx >= 0 && rowIdx < buffer.size()) {
            
            if (columnIdx >= 0 && columnIdx < buffer[rowIdx].size())
                return buffer[rowIdx][columnIdx];
            
            else {
                DBG("get: column index out of bounds: " << columnIdx);
                return -1.f;
            }
        }
        
        else {
            DBG("get: row index out of bounds: " << rowIdx);
            return -1.f;
        }
    }

    int getNumInputChannels() const {
        return static_cast<int>(buffer.size());
    }

    int getNumOutputChannels() const {
        return buffer.empty() ? 0 : static_cast<int>(buffer[0].size());
    }
    
    bool verifyMatrixDimensions(int numInputsFromHost, int numOutputsFromHost) const {
        
        int numInputChannels    = getNumInputChannels();
        int numOutputChannels   = getNumOutputChannels();
        
        return ((numInputsFromHost == numInputChannels) && (numOutputsFromHost == numOutputChannels));
    }

    void resize(int numInputChannels, int numOutputChannels) {
        setNumChannels(numInputChannels, numOutputChannels);
    }

    void clear() {
        for (auto& row : buffer) {
            std::fill(row.begin(), row.end(), 0.0f);
        }
    }

    Matrix& getMatrixReference() {
        return buffer;
    }

    void debugMatrix() {
        for (int i = 0; i < buffer.size(); i++) {
            juce::String row;
            for (int j = 0; j < buffer[i].size(); ++j) {
                row += juce::String(buffer[i][j], 6) + " ";
            }
            DBG("Row " << i << ": " << row);
        }
    }

private:
    Matrix buffer;
};
