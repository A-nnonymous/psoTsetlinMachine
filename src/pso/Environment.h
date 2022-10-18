# pragma once
#include <atomic>
#include <iostream>
#include <thread>
#include <climits>
#include "Particle.h"

class Environment{
private:
    const particleLimits    _limits;
    const int               _particleNum;
    const int               _maxIter;
    const float             _convThreshold;
    float                   (*_evaluateFunc)(evaluateJobArgs);
    float                   _gbValueLast;
    std::atomic<float>      _gbValue;
    std::vector<Particle>   _particles;
    targetArgs              _gbPosition;

    void                    exploitation();           // Evaluate each particle`s position using self-defined function.
    bool                    judgeConverge();        // Judge if global best change no more than threshold.
    void                    exploration();          // Change all particles position depends on their velocity.
    void                    printBest();           // Return the best result if converge or off-iter.

public:
    targetArgs              optimize();

    Environment(int particleNum, int iterNum,
                float egoFactor, float convThreshold, float omega, float dt,
                particleLimits limits,
                float evaluateFunc(evaluateJobArgs), evaluateJobArgs jobArgs);
};
