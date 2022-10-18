#include "pso.h"
#include "TsetlinMachine.h"
#include "io.h"


float concave(evaluateJobArgs pos)
{
    return -(pos.s * pos.s + pos.T * pos.T);
}

int main(int argc, char const *argv[])
{
    std::mt19937 rng(std::random_device{}());
    particleLimits limits(2.0,4,-1000,1000,-1000,1000);
    evaluateJobArgs dummy(1,1,1,1,rng);

    Environment test(   40,1000,
                        2.0f,0.01f,0.9f,0.001f,
                        limits,
                        concave,dummy);
    test.optimize();
    return 0;
}
