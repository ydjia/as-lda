#include <iostream>
#include <string>
#include <time.h>
#include "lda.h"

using namespace std;

void Usage()
{
    cout << "as-lda by wangben 20130605" << std::endl;
    cout << "Usage: " << endl;
    cout << "  -c  corpus file, default './corpus.txt' " << std::endl;
    cout << "  -v  vocab file, default './vocab.txt' " << std::endl;
    cout << "  -e or -i  act type(e for estimate,i for inference): "  << std::endl;
    cout << "  -m  model files dir, default './models' "  << std::endl;
    cout << "  -z  pre model assignment file ( inference ) " << std::endl;
    cout << "  -a  hyperparameter alpha, default 500/topic_num" << std::endl;
    cout << "  -b  hyperparameter beta, default 0.1" << std::endl;
    cout << "  -k  topic number, default 100" << std::endl;
    cout << "  -n  max iteration number, default 1000" << std::endl;
    cout << std::endl;
}

int main(int argc, char* argv[])
{
    std::string corpus_file = "./corpus.txt";
    std::string vocab_file = "./vocab.txt";
    
    std::string act_type = "";
    std::string models_dir = "./models";
    std::string configure_file = "";

    std::string pre_assign = models_dir + "/model.z";

    //----model parameters
    unsigned int max_iter = 1000;
    unsigned int topic_num = 100;
    double alpha = 500/topic_num;
    double beta = 0.1;
    
    //----parse command line
    int opt_c;
    while ( (opt_c = getopt( argc, argv, "c:v:m:z:a:b:k:n:ei")) != EOF )
    {
        switch (opt_c)
        {
        case 'c':
            corpus_file = optarg;
            break;
        case 'v':
            vocab_file = optarg;
            break;
        case 'm':
            models_dir = optarg;
            break;
        case 'z':
            pre_assign = optarg;
            break;
        case 'e':
            act_type = "estimate";
            break;
        case 'i':
            act_type = "inference";
            break;
        case 'a':
            alpha = atof(optarg);
            break;
        case 'b':
            beta = atof(optarg);
            break;
        case 'k':
            topic_num = (unsigned int)atoi(optarg);
            break;
        case 'n':
            max_iter = (unsigned int)atoi(optarg);
            break;
        default:
            break;
        }
        
    }

    //check options
    if ( act_type.length() == 0 )
    {
        std::cerr << "wrong parameter!!" << endl;
        Usage();
        return 1;
    }
    
    LDA lda;
    lda.SetParameters(topic_num, max_iter, alpha, beta, models_dir);
    lda.InitCorpus(corpus_file, vocab_file);

    if (act_type == "estimate")
    {
        lda.Estimate();
    }
    else
    {
        lda.Inference(pre_assign);
    }
    
    return 0;
}


