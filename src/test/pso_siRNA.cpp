#include "pso.h"
#include "TsetlinMachine.h"
#include "io.h"


float testFun(evaluateJobArgs args)
{
    std::vector<int> testinput;
    testinput.resize(args.numInputs, 1);
    TsetlinMachine tm(  args.numInputs, args.clausesPerOutput,  args.numOutputs,
                        args.s,         args.T,                 args.rng);
    

    
    int                             train_data_size = 1229;
    int                             test_data_size = 139;
    int                             input_features = 21;
    int                             input_bit_per_feature = 4;
    int                             output_size = 4;
    int                             input_size = input_bit_per_feature * input_features;

    std::vector<std::vector<int>>   train_seqs(train_data_size, std::vector<int>(input_size, 0));
    std::vector<std::vector<int>>   train_scores(train_data_size, std::vector(output_size, 0));
    std::vector<std::vector<int>>   test_seqs(test_data_size, std::vector<int>(input_size, 0));
    std::vector<std::vector<int>>   test_scores(test_data_size, std::vector(output_size, 0));
    parse_huesken_seqs("/home/data/siRNA/e2s/e2s_training_seq.csv", train_seqs);
    parse_huesken_scores("/home/data/siRNA/e2s/e2s_training_efficiency.csv", train_scores);
    parse_huesken_seqs("/home/data/siRNA/e2s/e2s_test_seq.csv", test_seqs);
    parse_huesken_scores("/home/data/siRNA/e2s/e2s_test_efficiency.csv", test_scores);
    

    float                           precision; 
    int                             trainset_correct, testset_correct;
    float                           best_test_accuracy = 0;
    std::vector<std::vector<int>>   this_test(test_data_size, std::vector<int>(4,0));
    std::vector<std::vector<int>>   best_test(test_data_size, std::vector<int>(4,0));
    std::vector<int>                hardMaxedResult; hardMaxedResult.resize(4,0);

    for (int epoch = 0; epoch < args.epoch_max; epoch++) {

        trainset_correct = 0; testset_correct = 0;

        for (int trainIdx = 0; trainIdx < train_data_size; trainIdx++)
        {
            Prediction    train_predict = tm.predict(train_seqs[trainIdx]);
            hardMaxedResult = hardMax(train_predict);
            if(hardMaxedResult == train_scores[trainIdx])trainset_correct++;
            tm.learn(train_scores[trainIdx]);
        }
        for(int testIdx = 0; testIdx < test_data_size; testIdx++)
        {
            Prediction    test_predict = tm.predict(test_seqs[testIdx]);
            hardMaxedResult = hardMax(test_predict);
            this_test[testIdx] = hardMaxedResult;
            if(hardMaxedResult == test_scores[testIdx])testset_correct++;
        }

        float this_test_accuracy = testset_correct/(float)test_data_size;
        if(this_test_accuracy > best_test_accuracy)
        {
            best_test=this_test;
            best_test_accuracy = this_test_accuracy;
        }
    }

    return best_test_accuracy;

}

int main(int argc, char const *argv[])
{
    // Tsetlin Machine common arguments.
    int     inputBitNum = 84;
    int     clausePerOutput = 1000;
    int     outputBitNum = 4;
    int     epochNum = 100;
    std::mt19937 rng(time(nullptr));

    // PSO algorithm particle limit arguments.
    float   vTMax = 20.0f,  vsMax = 2.0f;
    float   sMin = 2.0f,    sMax = 20.0f;
    int     TMin = 200,     TMax = 2000;
    
    // PSO algorithm environment arguments.
    int             particleNum = 94;
    int             maxIter = 100;
    int             threadNum = 94;
    float           egoFactor = 1.0f;
    float           convThreshold = 0.002f;
    float           omega = 0.8f;
    float           dt = 0.01f;

    particleLimits  argsLimit(   vTMax,  vsMax,
                            sMin,   sMax,
                            TMin,   TMax  );

    evaluateJobArgs testFunArgs(    inputBitNum, clausePerOutput, outputBitNum,
                                    epochNum, rng   );

    // Constructing enviroment.
    Environment testEnv(    particleNum, maxIter, threadNum,
                            egoFactor, convThreshold, omega, dt,
                            argsLimit,
                            testFun, testFunArgs);
    testEnv.optimize();
    return 0;
}
