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

    std::vector<std::vector<int>>   train_seqs(train_data_size,     std::vector<int>(input_size, 0));
    parse_huesken_seqs("/home/data/siRNA/e2s/e2s_training_seq.csv", train_seqs);
    std::vector<std::vector<int>>   test_seqs(test_data_size,       std::vector<int>(input_size, 0));
    parse_huesken_seqs("/home/data/siRNA/e2s/e2s_test_seq.csv", test_seqs);
    std::vector<std::vector<int>>   train_scores(train_data_size,   std::vector(output_size, 0));
    parse_huesken_scores("/home/data/siRNA/e2s/e2s_training_efficiency.csv", train_scores);
    std::vector<std::vector<int>>   test_scores(test_data_size,     std::vector(output_size, 0));
    parse_huesken_scores("/home/data/siRNA/e2s/e2s_test_efficiency.csv", test_scores);

    float                           precision; 
    int                             trainset_correct, testset_correct;
    float                           best_test_accuracy = 0;
    std::vector<int>                hardMaxedResult(output_size, 0);
    std::vector<std::vector<int>>   this_test(test_data_size, std::vector<int>(output_size, 0));
    std::vector<std::vector<int>>   best_test(test_data_size, std::vector<int>(output_size, 0));

    for (int epoch = 0; epoch < args.epoch_max; epoch++) {

        trainset_correct = 0; testset_correct = 0;

        for (int trainIdx = 0; trainIdx < train_data_size; trainIdx++)
        {
            TsetlinMachine::Prediction    train_predict = tm.predict(train_seqs[trainIdx]);
            hardMaxedResult = hardMax(train_predict);
            if(hardMaxedResult == train_scores[trainIdx])trainset_correct++;
            tm.learn(train_scores[trainIdx]);
        }
        for(int testIdx = 0; testIdx < test_data_size; testIdx++)
        {
            TsetlinMachine::Prediction    test_predict = tm.predict(test_seqs[testIdx]);
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

void repetition(targetArgs bestArgs, evaluateJobArgs jobArgs)
{
    std::mt19937                    rng(std::random_device{}());
    int                             train_data_size = 1229;
    int                             test_data_size = 139;
    int                             input_features = jobArgs.numInputs;
    int                             input_bit_per_feature = 4;
    int                             output_size = jobArgs.numOutputs;
    int                             input_size = input_bit_per_feature * input_features;
    int                             clausePerOutput = jobArgs.clausesPerOutput;

    std::vector<std::vector<int>>   train_seqs(train_data_size, std::vector<int>(input_size, 0));
    std::vector<std::vector<int>>   train_scores(train_data_size, std::vector(output_size, 0));
    std::vector<std::vector<int>>   test_seqs(test_data_size, std::vector<int>(input_size, 0));
    std::vector<std::vector<int>>   test_scores(test_data_size, std::vector(output_size, 0));
    parse_huesken_seqs("/home/data/siRNA/e2s/e2s_training_seq.csv", train_seqs);
    parse_huesken_scores("/home/data/siRNA/e2s/e2s_training_efficiency.csv", train_scores);
    parse_huesken_seqs("/home/data/siRNA/e2s/e2s_test_seq.csv", test_seqs);
    parse_huesken_scores("/home/data/siRNA/e2s/e2s_test_efficiency.csv", test_scores);
    std::vector<TsetlinMachine::Clause> bestPositiveClauses;
    std::vector<TsetlinMachine::Clause> bestNegativeClauses;
    std::string outputpath = "/home/output/";
    

    TsetlinMachine  tm(input_size,clausePerOutput,output_size,bestArgs.s,bestArgs.T,rng);
    
    float                           precision; 
    int                             trainset_correct, testset_correct;
    float                           best_test_accuracy = 0;
    std::vector<std::vector<int>>   this_test(test_data_size, std::vector<int>(4,0));
    std::vector<std::vector<int>>   best_test(test_data_size, std::vector<int>(4,0));
    std::vector<int>                hardMaxedResult; hardMaxedResult.resize(4,0);

    for (int epoch = 0; epoch < jobArgs.epoch_max; epoch++) {

        trainset_correct = 0; testset_correct = 0;

        for (int trainIdx = 0; trainIdx < train_data_size; trainIdx++)
        {
            TsetlinMachine::Prediction    train_predict = tm.predict(train_seqs[trainIdx]);
            hardMaxedResult = hardMax(train_predict);
            if(hardMaxedResult == train_scores[trainIdx])trainset_correct++;
            tm.learn(train_scores[trainIdx]);
        }
        for(int testIdx = 0; testIdx < test_data_size; testIdx++)
        {
            TsetlinMachine::Prediction    test_predict = tm.predict(test_seqs[testIdx]);
            hardMaxedResult = hardMax(test_predict);
            this_test[testIdx] = hardMaxedResult;
            if(hardMaxedResult == test_scores[testIdx])testset_correct++;
        }

        float this_test_accuracy = testset_correct/(float)test_data_size;
        if(this_test_accuracy > best_test_accuracy)
        {
            best_test=this_test;
            best_test_accuracy = this_test_accuracy;
            tm.getPositiveClauses(&bestPositiveClauses);
            tm.getNegativeClauses(&bestNegativeClauses);
        }

        std::cout<<"Epoch "<< epoch+1 <<" training accuracy: "<< trainset_correct/(float)train_data_size
                                    <<" \t\ttesting accuracy:"<< this_test_accuracy <<std::endl;
    }
    std::cout<<"BEST TEST ACCURACY: "<< best_test_accuracy <<std::endl;
    
    modelOutput(tm,bestPositiveClauses,bestNegativeClauses,best_test_accuracy, outputpath);
}

int main(int argc, char const *argv[])
{
    // Tsetlin Machine common arguments.
    int     inputBitNum = 84;
    int     clausePerOutput = 1000;
    int     outputBitNum = 4;
    int     epochNum = 100;
    std::mt19937 rng(std::random_device{}());

    // PSO algorithm particle limit arguments.
    float   sMin = 0.0f,    sMax = 40.0f;
    int     TMin = 0.5 * clausePerOutput;   // For aggressive feedback.
    int     TMax = 2.0 * clausePerOutput;   // For noise control.
    
    // PSO algorithm environment arguments.
    int             particleNum = 94;
    int             maxIter = 100;
    float           egoFactor = 2.0f;       // C1 = egoFactor, C2 = 1/egoFactor.
    float           convThreshold = 0.002f;
    float           omega = 0.9f;
    float           dt = 0.01f;

    float           expectedNoiseRatio = 2; // Expected worst condition to get to optima.
    float           vTMax = (TMax - TMin) / expectedNoiseRatio;
    float           vsMax = (TMax - TMin) / expectedNoiseRatio;

    particleLimits  argsLimit(   vTMax,  vsMax,
                            sMin,   sMax,
                            TMin,   TMax  );

    evaluateJobArgs testFunArgs(    inputBitNum, clausePerOutput, outputBitNum,
                                    epochNum, rng   );

    // Constructing enviroment.
    Environment testEnv(    particleNum, maxIter,
                            egoFactor, convThreshold, omega, dt,
                            argsLimit,
                            testFun, testFunArgs);
    targetArgs result = testEnv.optimize();
    std::cout<<"Received best arg_s: "<<result.s<<", arg_T: "<<result.T<<" start retrain..."<<std::endl;
    repetition(result, testFunArgs);
    return 0;
}
