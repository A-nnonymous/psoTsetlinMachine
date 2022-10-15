# PSO optimized Tsetlin Machine

An implementation of the Tsetlin machine in C++,using multi-thread PSO algorithm to optimize hyper parameters.

Contains a simple XOR demo copied from [reference](https://github.com/222464/TsetlinMachine.git).


The paper this implementation is based on is available [here](https://arxiv.org/abs/1804.01508).

The default setup of my machine is 96 cores with AVX-512F instruction set , so modify ./test/pso_siRNA.cpp and ./CMakeLists to make best fit for your machine.