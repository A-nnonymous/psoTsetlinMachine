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
        1. Write a public function to output pruned clauses in trained model neatly.
        2. Checkpointing training using clause data.
*/



class TsetlinMachine {
public:

struct Prediction
{
    std::vector<int>        digit;
    std::vector<float>      confidence;
};
struct Clause {
        std::vector<int>            _automataStates;    // Using 32bit integer represent states.
        std::unordered_set<int>     _inclusions;        // Store all literal that include in clause.

        int _state;                                     // Stand for its vote.
        Clause()
        {
            _state = 0;
        }
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
    const Prediction        &predict(const std::vector<int> &inputStates);
    void                    learn(const std::vector<int> &targetOutputStates);
    void                    getPositiveClauses(std::vector<TsetlinMachine::Clause> *outsideBuffer);
    void                    getNegativeClauses(std::vector<TsetlinMachine::Clause> *outsideBuffer);
    
    TsetlinMachine(int numInputs, int clausesPerOutput, int numOutputs,
                   float s, int T,std::mt19937 &rng);
};

std::vector<int> hardMax(TsetlinMachine::Prediction p);