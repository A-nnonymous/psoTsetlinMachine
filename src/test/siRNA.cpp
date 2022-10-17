#include "io.h"
#include <random>
#include "TsetlinMachine.h"

inline std::vector<int> hardMax(TsetlinMachine::Prediction p)
{
    float max_confidence = 0;
    int max_confidence_idx = 0;
    std::vector<int> result(p.digit.size(),0);
    for(int idx = 0; idx < p.digit.size(); idx++)
    {
        if(p.confidence[idx] > max_confidence)
        {
            max_confidence = p.confidence[idx];
            max_confidence_idx = idx;
        }
    }
    result[max_confidence_idx]=1;
    return result; 
}

int main() {

    std::mt19937                    rng(time(nullptr));
    int                             train_data_size = 1229;
    int                             test_data_size = 139;
    int                             input_features = 21;
    int                             input_bit_per_feature = 4;
    int                             output_size = 4;
    int                             input_size = input_bit_per_feature * input_features;
    int                             clausePerOutput = 500;
    int                             clauseTotal = output_size * clausePerOutput;

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
    

    TsetlinMachine  tm(input_size,clausePerOutput,output_size,8.4f,343,rng);
    
    float                           avgErr = 1.0f;
    float                           precision; 
    int                             trainset_correct, testset_correct;
    float                           best_test_accuracy = 0;
    std::vector<std::vector<int>>   this_test(test_data_size, std::vector<int>(4,0));
    std::vector<std::vector<int>>   best_test(test_data_size, std::vector<int>(4,0));
    std::vector<int>                hardMaxedResult; hardMaxedResult.resize(4,0);

    for (int epoch = 0; epoch < 50; epoch++) {

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
    for (int i = 0; i < test_data_size; i++)
    {
        std::cout<< "Real: ";
        for(int j =0; j < test_scores[i].size(); j++)
        {
            std::cout << test_scores[i][j]<<" ";
        }
        std::cout<< "\t\tPredicted:";
        for(int k =0; k < test_scores[i].size(); k++)
        {
            std::cout << best_test[i][k]<<" ";
        }
        std::cout<<std::endl;

    }
    std::cout<<"BEST TEST ACCURACY: "<< best_test_accuracy <<std::endl;

    modelOutput(tm,bestPositiveClauses,bestNegativeClauses,best_test_accuracy);

    
    return 0;
}
