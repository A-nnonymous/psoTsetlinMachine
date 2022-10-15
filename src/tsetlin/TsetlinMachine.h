/*
MIT License

Copyright (c) 2022 Pan Zhaowu

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include <vector>
#include <random>
#include <unordered_set>
#include <assert.h>
#include <span>
#include <iostream>


/*
    TODO: 
        1. Write a data parser to parse input and output.
        2. Write a public function to output pruned clauses in trained model neatly.
        3. Checkpointing training using clause data.
        4. Add swarm intellegence to parallel training.
        5. Rebuild and test TM class.
        6. Add confidence-wised argmax to 
*/

typedef struct Prediction
{
    std::vector<int>        digit;
    std::vector<float>      confidence;
}Prediction;

class TsetlinMachine {
public:
    struct Clause {
        std::vector<int>            _automataStates;    // Using 32bit integer represent states.
        std::unordered_set<int>     _inclusions;        // Store all literal that include in clause.

        int _state;                                     // Stand for its vote.
    };

    struct Output {
        std::span<Clause>           _clauses;   // All available clauses in vector.
        int                         _sum;       // Sum of all votes.
    };

private:
    const float                 _arg_s;
    const float                 _sInv;
    const float                 _sInvConj;
    const int                   _arg_T;
    const int                   _clausePerOutput;
    const int                   _clauseNum;
    const int                   _inputSize;
    const int                   _outputSize;
    
    std::mt19937                _rng;
    std::vector<int>            _inputStates;   // Bitwise input.
    std::vector<Clause>         _clauses;       // All agent constructed by tsetlin automata.
    std::vector<Output>         _outputs;       // Final output vector.
    Prediction                  _results;       // Vector of boolean

    void    create(int numInputs, int numOutputs, int clausesPerOutput);
    void    inclusionUpdate(int ouputIdx, int clauseIdx, int automataIdx);
    void    feedbackTypeI(int ouputIdx, int clauseIdx);
    void    feedbackTypeII(int ouputIdx, int clauseIdx);

public:
    const Prediction       &predict(const std::vector<int> &inputStates);
    void                    learn(const std::vector<int> &targetOutputStates);
    
    TsetlinMachine(int numInputs, int clausesPerOutput, int numOutputs,
                   float s, int T,std::mt19937 &rng):
        _clausePerOutput(clausesPerOutput),
        _clauseNum( numOutputs * clausesPerOutput),
        _inputSize(numInputs),
        _arg_s(s),
        _sInv(1.0f / s),
        _sInvConj(1.0f - _sInv),
        _arg_T(T),
        _rng(rng),
        _outputSize(numOutputs)
    {
        _inputStates.resize(numInputs, 0); // Resize and initialize to zeros.
        _clauses.resize(_clauseNum);
        _outputs.resize(numOutputs);
        _results.digit.resize(numOutputs, 0);
        _results.confidence.resize(numOutputs, 0);

        std::span<Clause>       allClauses(_clauses);
        for(int outputIdx = 0; outputIdx < numOutputs; outputIdx++)
        {
            int src = outputIdx * clausesPerOutput;
            for (int clauseIdx = 0; clauseIdx < clausesPerOutput; clauseIdx++)
            {
                _clauses[src + clauseIdx]
                        ._automataStates
                        .resize(numInputs * 2, 0);
            }
            _outputs[outputIdx]._clauses = allClauses.subspan(src, clausesPerOutput);
        }
    }
};

std::vector<int> hardMax(Prediction p);