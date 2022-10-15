# pragma once
#include <atomic>
#include <iostream>
#include <thread>
#include "Particle.h"
typedef struct sharedTargetArgs{
    float      s;
    int        T;
    sharedTargetArgs(float sIn, int TIn)
    {
        s = sIn;
        T = TIn;
    };
}sharedTargetArgs;

class Environment{
private:
    const particleLimits    _limits;
    const int               _particleNum;
    const int               _maxIter;
    const int               _threadNum;
    const float             _convThreshold;
    float                   (*_evaluateFunc)(evaluateJobArgs);
    targetArgs              _gbPosition;
    float                   _gbValueLast;
    std::atomic<float>      _gbValue;
    std::vector<Particle>   _particles;

    void                    exploitation();           // Evaluate each particle`s position using self-defined function.
    bool                    judgeConverge();        // Judge if global best change no more than threshold.
    void                    exploration();          // Change all particles position depends on their velocity.
    void                    printBest();           // Return the best result if converge or off-iter.

public:
    void                    optimize();

    Environment(int particleNum, int iterNum, int threadNum,
                float egoFactor, float convThreshold, float omega, float dt,
                particleLimits limits,
                float evaluateFunc(evaluateJobArgs), evaluateJobArgs jobArgs);
};
