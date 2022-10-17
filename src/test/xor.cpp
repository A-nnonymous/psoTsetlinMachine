/*
MIT License

Copyright (c) 2018 Eric Laukien

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
#include <time.h>
// Select demo
//#define DEMO_WAVY

int main() {
    std::mt19937                    rng(time(nullptr));
    std::vector<std::vector<int>>   inputs = {
        { 0, 0 },
        { 0, 1 },
        { 1, 0 },
        { 1, 1 }
    };

    std::vector<std::vector<int>>   outputs = {
        { 0, 1 },
        { 1, 0 },
        { 1, 0 },
        { 0, 1 },
    };
    TsetlinMachine  tm(2,10,2,4.0f,4,rng);
    
    float           avgErr = 1.0f;
    clock_t start = clock();
    
    for (int e = 0; e < 1000; e++) {
        std::vector<int>                in = inputs[e % 4];
        TsetlinMachine::Prediction      out = tm.predict(in);
        bool                            correct = false;

        if (in[0] == in[1]) {
            if (out.digit[0] == 0 && out.digit[1] == 1)
                correct = true;
        }
        else {
            if (out.digit[0] == 1 && out.digit[1] == 0)
                correct = true;
        }

        avgErr = 0.99f * avgErr + 0.01f * !correct;

        std::cout << in[0] << " " << in[1] << " -> " << out.digit[0] << " " << out.digit[1] << " | " << (1-avgErr)*100 << "%\n";

        tm.learn(outputs[e % 4]);
    }

    clock_t duration = clock() - start;
    std::cout<< duration/(double)CLOCKS_PER_SEC <<std::endl;

    return 0;
}
