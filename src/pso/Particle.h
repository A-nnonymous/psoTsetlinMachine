#pragma once
#include <random>

typedef struct velocity{
    float   s;
    int     T;
    velocity(float sIn, int TIn)
    {
        s = sIn;
        T = TIn;
    }
}velocity;

typedef struct targetArgs{
    float      s;
    int        T;
    targetArgs(float sIn, int TIn)
    {
        s = sIn;
        T = TIn;
    };
    targetArgs()
    {
        s = 0.0f;
        T = 0;
    }
}targetArgs;

typedef struct particleLimits{
    float   vTMax,  vsMax;
    float   sMin,   sMax;
    int     TMin,   TMax;
    particleLimits(float vTMaxIn, float vsMaxIn,
                    float sMinIn,float sMaxIn,
                    int TMinIn,int TMaxIn)
    {
        vTMax = vTMaxIn;    vsMax = vsMaxIn;
        sMin = sMinIn;      sMax = sMaxIn;
        TMin = TMinIn;      TMax = TMaxIn;
    };
}particleLimits;

typedef struct evaluateJobArgs
{
    int             numInputs, clausesPerOutput, numOutputs;
    int             T;
    int             epoch_max;
    float           s;
    std::mt19937    rng;
    evaluateJobArgs(int ni, int cpo, int no,
                    int epo, std::mt19937 &rng)
    {
        numInputs = ni;clausesPerOutput = cpo;numOutputs = no; 
        epoch_max = epo;
        this->rng = rng;
    }
    evaluateJobArgs()
    {
        numInputs = clausesPerOutput = numOutputs = T = epoch_max = 0;
        s = 0.0f;
    }
}evaluateJobArgs;

class Particle{
private:
    const float             _ego,_superEgo;
    const float             _omega;
    const float             _dt;
    const particleLimits    _limits;

    std::mt19937            _rd;
    velocity                *_velocity;
    targetArgs              *_pbPosition;
    float                   _pbValue;
    float                   _currentValue;
    float                   (*_evaluateJob)(evaluateJobArgs);

    evaluateJobArgs         _uniqueArgs;
    void                    uniqueInit();
public:
    void                    update(targetArgs gbPosition); //  modify particle itself and return the
    void                    evaluate();                     //  run the tsetlin process and evaluate args, renew value.
    targetArgs              getArgs();
    float                   getCurrentValue();
    Particle(   particleLimits limits,
                float omega, float egoFactor, float dt,
                float fun(evaluateJobArgs),
                evaluateJobArgs commonArgs);

    Particle operator=(Particle)
    {
        return *this;
    }
};