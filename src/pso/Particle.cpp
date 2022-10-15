#include <iostream>
#include "Particle.h"

void Particle::update(targetArgs gbPosition)
{
    std::uniform_real_distribution<float> dist01(0.0f, 1.0f);
    float r1 = dist01(_rd);
    float r2 = dist01(_rd);
    _velocity->s = _omega * _velocity->s
                   + r1 * _ego * (_pbPosition->s - _uniqueArgs.s)
                   + r2 * _superEgo * (gbPosition.s - _uniqueArgs.s);
    _velocity->T = _omega * _velocity->T
                   + r1 * _ego * (_pbPosition->T - _uniqueArgs.T)
                   + r2 * _superEgo * (gbPosition.T - _uniqueArgs.T);

    _velocity->s = _velocity->s < _limits.vsMax ? _velocity->s : _limits.vsMax;
    _velocity->T = _velocity->T < _limits.vTMax ? _velocity->T : _limits.vTMax;

    _uniqueArgs.s += _velocity->s * _dt;
    _uniqueArgs.T += _velocity->T * _dt;

    _uniqueArgs.s = _uniqueArgs.s > _limits.sMax ? _limits.sMax : _uniqueArgs.s;
    _uniqueArgs.s = _uniqueArgs.s < _limits.sMin ? _limits.sMin : _uniqueArgs.s;
    _uniqueArgs.T = _uniqueArgs.T > _limits.TMax ? _limits.TMax : _uniqueArgs.T;
    _uniqueArgs.T = _uniqueArgs.T < _limits.TMin ? _limits.TMin : _uniqueArgs.T;
}

void Particle::evaluate()// change self value and return
{
    float thisValue = _evaluateJob(_uniqueArgs);
    _currentValue = thisValue;
    if(thisValue > _pbValue)
    {
        _pbValue = thisValue;
        _pbPosition->s = _uniqueArgs.s;
        _pbPosition->T = _uniqueArgs.T;
    }
}

targetArgs Particle::getArgs()
{
    targetArgs result(_uniqueArgs.s,_uniqueArgs.T);
    return result;
}
float Particle::getCurrentValue()
{
    return _currentValue;
}


Particle::Particle(particleLimits limits, float omega, float egoFactor, float dt, float fun(evaluateJobArgs), evaluateJobArgs commonArgs):
    _limits(limits),_omega(omega),_ego(egoFactor),_superEgo(1/egoFactor),_dt(dt)
{
        std::uniform_real_distribution<float>   distS(_limits.sMin, _limits.sMax);
        std::uniform_real_distribution<float>   distT(_limits.TMin, _limits.TMax);
        std::uniform_real_distribution<float>   distVs(0, _limits.vsMax);
        std::uniform_real_distribution<float>   distVT(0, _limits.vTMax);

        std::mt19937        _rd(std::random_device{}());
        this->_velocity = new velocity(distVs(_rd), distVT(_rd));
        this->_pbPosition = new targetArgs(-1,-1);  // Whatever.
        this->_pbValue = -1.0;
        this->_currentValue = 0.01f;
        this->_evaluateJob = fun;
        this->_uniqueArgs = commonArgs;
        _uniqueArgs.s = distS(_rd);_uniqueArgs.T = distT(_rd);
}