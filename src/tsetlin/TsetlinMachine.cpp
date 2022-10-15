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

#include "TsetlinMachine.h"
#include <iostream>

const Prediction  &TsetlinMachine::predict(const std::vector<int> &inputStates) 
{
    // A prediction (or says, forward propagation) of tsetlin machine.
    _inputStates = inputStates;

    for (int outputIdx = 0; outputIdx < _outputSize; outputIdx++) 
    {
        int sum = 0;

        for (int clauseIdx = 0;
             clauseIdx < _clausePerOutput; 
             clauseIdx++) 
        {
            int state = 1;
            for (std::unordered_set<int>::const_iterator    cit = _outputs[outputIdx]._clauses[clauseIdx]._inclusions.begin(); 
                    cit != _outputs[outputIdx]._clauses[clauseIdx]._inclusions.end();
                    cit++) 
            {
                int automataIdx = *cit; // Active automata index.

                if (automataIdx >= _inputSize)
                    state = state && !_inputStates[automataIdx - _inputSize];  
                else
                    state = state && _inputStates[automataIdx];
            }

            _outputs[outputIdx]._clauses[clauseIdx]._state = state;

            sum += (clauseIdx % 2 == 0 ? state : -state);   // For negative clause interleaved.
        }

        _outputs[outputIdx]._sum = sum;

        _results.digit[outputIdx] = sum > 0;      // Output 1 or 0 for certain idx.
        _results.confidence[outputIdx] = sum / (float)_clausePerOutput;
    }

    return _results;   // return all output array bitwise.
}

void TsetlinMachine::inclusionUpdate(int outputIdx, int clauseIdx, int automataIdx) 
{
    bool isToInclude = _outputs[outputIdx]
                            ._clauses[clauseIdx]
                            ._automataStates[automataIdx] > 0;
    std::unordered_set<int>::iterator it = _outputs[outputIdx]._clauses[clauseIdx]
                                                                ._inclusions
                                                                .find(automataIdx);

    if(isToInclude && 
        (it == _outputs[outputIdx]._clauses[clauseIdx]._inclusions.end())
        )
        _outputs[outputIdx]._clauses[clauseIdx]
                                ._inclusions.emplace(automataIdx);
    
    if(!isToInclude && 
        (it != _outputs[outputIdx]._clauses[clauseIdx]._inclusions.end())
        )
        _outputs[outputIdx]._clauses[clauseIdx]
                                ._inclusions.erase(automataIdx);
}

void TsetlinMachine::feedbackTypeI(int outputIdx, int clauseIdx)
{
    std::uniform_real_distribution<float> dist01(0.0f, 1.0f);

    int clauseState = _outputs[outputIdx]._clauses[clauseIdx]._state;
    for (int automataIdx = 0; 
        automataIdx < (_inputSize * 2); 
        automataIdx++) 
    {
        bool isPositiveLiteral = (automataIdx >= _inputSize ? false : true);
       
        int literalState = (isPositiveLiteral ? 
                    _inputStates[automataIdx] : !_inputStates[automataIdx - _inputSize]);
        if(clauseState) // Good Match for some pattern 
        {
            if(literalState)    // Good literal, whatever included or not, ascend it.
            {
                if (dist01(_rng) < _sInvConj) // Assume that s>1, so _sInvConj>0.5, perfer to action.
                {
                    _outputs[outputIdx]._clauses[clauseIdx]._automataStates[automataIdx] += 1;  // State ascendence.
                    inclusionUpdate(outputIdx, clauseIdx, automataIdx);
                }
            }
            else                // Noisy literal, reinforce exclusion if not included(because can't include)
            {
                if (dist01(_rng) < _sInv) 
                {
                    _outputs[outputIdx]
                        ._clauses[clauseIdx]
                        ._automataStates[automataIdx] -= 1;
                    inclusionUpdate(outputIdx, clauseIdx, automataIdx);
                }
            }
        }
        else            // Bad match in this clause ,Prefer to exclude no matter whether literal.
        {
            if (dist01(_rng) < _sInv) 
            {
                _outputs[outputIdx]
                    ._clauses[clauseIdx]
                    ._automataStates[automataIdx] -= 1;
                inclusionUpdate(outputIdx, clauseIdx, automataIdx);
            }
        }
    }
}

void TsetlinMachine::feedbackTypeII(int outputIdx, int clauseIdx) 
{
    int clauseState = _outputs[outputIdx]._clauses[clauseIdx]._state;

    for (int automataIdx = 0; 
            automataIdx < (_inputSize * 2); 
            automataIdx++) 
    {
        int input = (automataIdx >= _inputSize ? 
                    !_inputStates[automataIdx - _inputSize] : 
                    _inputStates[automataIdx]);

        int inclusion = _outputs[outputIdx]._clauses[clauseIdx]._automataStates[automataIdx] > 0;

        if (clauseState && !input && !inclusion) 
        {
            _outputs[outputIdx]
                ._clauses[clauseIdx]
                ._automataStates[automataIdx] += 1;
            inclusionUpdate(outputIdx, clauseIdx, automataIdx);
        }
    }
}

void TsetlinMachine::learn(const std::vector<int> &targetOutputStates)
{
    std::uniform_real_distribution<float>   dist01(0.0f, 1.0f);
    int                                     T = this->_arg_T;
    
    for (int outputIdx = 0; outputIdx < _outputSize; outputIdx++) 
    {
        int     clampedSum = std::min(T, std::max(-T, _outputs[outputIdx]._sum));
        float   rescaleFactor = 1.0f / static_cast<float>(2 * T);

        float   probFeedBack0 = (T - clampedSum) * rescaleFactor; // The larger the T is, the less biased sum influences
        float   probFeedBack1 = (T + clampedSum) * rescaleFactor;
        
        int     sum = 0;

        for (int clauseIdx = 0; clauseIdx < _clausePerOutput; clauseIdx++) {
            if (clauseIdx % 2 == 0) {   // Positive clauses
                if (targetOutputStates[outputIdx]) {
                    if (dist01(_rng) < probFeedBack0)
                        feedbackTypeI(outputIdx, clauseIdx);
                }
                else {
                    if (dist01(_rng) < probFeedBack1)
                        feedbackTypeII(outputIdx, clauseIdx);
                }
            }
            else {                      // Negative clauses
                if (targetOutputStates[outputIdx]) {
                    if (dist01(_rng) < probFeedBack0)
                        feedbackTypeII(outputIdx, clauseIdx);
                }
                else {
                    if (dist01(_rng) < probFeedBack1)
                        feedbackTypeI(outputIdx, clauseIdx);
                }
            }
        }
    }
}
std::vector<int> hardMax(Prediction p)
{
    float max_confidence = 0;
    int max_confidence_idx = 0;
    std::vector<int> result(p.digit.size(),0);
    for(int idx = 0; idx < p.digit.size(); idx++)
    {
        if(p.confidence[idx] > max_confidence)
        {
            max_confidence = p.confidence[idx];
            max_confidence_idx = idx;
        }
    }
    result[max_confidence_idx]=1;
    return result;
}