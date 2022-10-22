# PSO optimized Tsetlin Machine(deprecated)

An implementation of the Tsetlin machine in C++,using multi-thread PSO algorithm to optimize hyper parameters.

Contains a simple XOR demo copied from [reference](https://github.com/222464/TsetlinMachine.git).


The paper this implementation is based on is available [here](https://arxiv.org/abs/1804.01508).

The default setup of my machine is 96 cores with AVX-512F instruction set , so modify ./test/pso_siRNA.cpp and ./CMakeLists to make best fit for your machine.

## **Notice:** There's some premature code in very early stage of this work, probably may take a lot of time to fix and rearrange the whole project, so I decide to reconstruct it in another work later and announce this repo deprecated.

## [Further work](https://github.com/A-nnonymous/RSATsetlinMachine) may introduce RSA(Reptile Search Algorithm) to optimize tsetlin machine, with usage of modern threadpool and probably adding boosting & dropout to tsetlin machine algorithm itself.
