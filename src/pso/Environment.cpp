#include "Environment.h"
#include <chrono>
void Environment::printBest()
{
    std::cout<<"\t+++++++++++++++++++++++++++++++++++++++"<<std::endl;
    std::cout<<"\tBest argument 's' is optimized to: "<< _gbPosition.s<<std::endl;
    std::cout<<"\tBest argument 'T' is optimized to: "<< _gbPosition.T<<std::endl;
    std::cout<<"\tThis argument set has value of: "<<_gbValue<<std::endl;
    std::cout<<"\t+++++++++++++++++++++++++++++++++++++++"<<std::endl;
}
void Environment::exploitation()    // evaluate each position in multithread.
{
    _gbValueLast = _gbValue;
    float bestParticleValue = _gbValue;
    float thisParticleValue = 0;
    targetArgs bestParticleArgs(0,0);
    bool touched = false;           // Flag of dirty best record.

    /////////////////Multithread evaluation////////////////
    std::vector<std::thread>        threadpool;
    // Using all available thread by default.
    for(auto    thisParticle =  _particles.begin();
                thisParticle != _particles.end();
                thisParticle ++
        )
    {
        std::thread th(&Particle::evaluate,thisParticle);
        threadpool.push_back(move(th));
    }
    for(auto &thread : threadpool)  thread.join();
    /////////////////Multithread evaluation////////////////

    for(auto    thisParticle =  _particles.begin();
                thisParticle != _particles.end();
                thisParticle ++
        )
    {
        thisParticleValue = (*thisParticle).getCurrentValue();
        if(thisParticleValue > bestParticleValue)
        {
            bestParticleValue = thisParticleValue;
            bestParticleArgs = (*thisParticle).getArgs();
            touched = true;
        }
    }

    if(touched)
    {
        _gbPosition = bestParticleArgs;
        _gbValue = bestParticleValue;
    }
    return;
}

bool Environment::judgeConverge()
{
    return (std::abs(_gbValue - _gbValueLast)<_convThreshold);
}


void Environment::exploration()
{
    for(auto thisParticle = _particles.begin();
            thisParticle!=_particles.end();
            thisParticle++)
    {
        (*thisParticle).update(_gbPosition);
    }
    std::cout<<"\n\tArguments of all particles succesfully updated."<<std::endl;
    
}


targetArgs Environment::optimize()
{
    int it = 0;
    while (it++ < _maxIter)
    {
        std::cout<<"\n***Iteration "<<it<<" started :"<<std::endl;
        auto start = std::chrono::steady_clock::now();
        exploitation();
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = end-start;
        std::cout<<"\tA batch of evaluation is completed, ";
        std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";

        if(judgeConverge())
        {
            printBest();
            std::cout<<"Global optima converged, optimization completed."<<std::endl;
            return std::move(_gbPosition);
        }
        exploration();
        printBest();
    }
    std::cout<<"\n\n****Max iteration time exceeded****"<<std::endl;
    std::cout<<"FINAL RESULT:"<<std::endl;
    printBest();
    return std::move(_gbPosition);
}


Environment::Environment(   int particleNum, int maxIter,
                            float egoFactor, float convThreshold,float omega, float dt,
                            particleLimits limits,
                            float evaluateFunc(evaluateJobArgs), evaluateJobArgs jobArgs):
_particleNum(particleNum),
_maxIter(maxIter),
_convThreshold(convThreshold),
_limits(limits)
{
    particleLimits lm = limits;
    _gbValue = -(__FLT_MAX__);
    _gbValueLast = 0.0f;
    targetArgs _gbPosition(0.0f,0);
    _evaluateFunc = evaluateFunc;
    for(int i=0; i<particleNum; i++)
    {
        _particles.push_back(Particle(lm, omega, egoFactor, dt, _evaluateFunc, jobArgs));
    }
}


