/// @Brief: 
/// @Date: 2013Äê4ÔÂ25ÈÕ 12:30:55
/// @Author: garfieldwang

#ifndef __LDA_H__
#define __LDA_H__

#include <vector>
#include "ml_data.h"

class LDA
{
public:
typedef std::vector< std::vector<unsigned int> > T_MATRIX;

public:
    LDA();

    ~LDA(){}

    void SetParameters(
        unsigned int topic_num,
        unsigned int max_iter,
        double alpha,
        double beta,
        std::string model_dir
        );

    bool InitCorpus(
        const std::string& corpus_file , 
        const std::string& vocab_file
        );

    bool LoadAssignment(
        const std::string& z_file
        );

    bool Estimate();
    bool Inference(const std::string& pre_model);
    
private:
    void OutputParas();

    bool InitEstimate();
    bool InitInference();

    unsigned int SamplingMultinomial(
        unsigned int m, 
        unsigned int n
        );

    unsigned int InfSamplingMultinomial(
        unsigned int m, 
        unsigned int n
        );

    bool CalculateResultParas();
    bool InfCalculateResultParas();

private:
    //corpus
    TextData m_corpus;

    //p( w=i |topic=k )
    T_MATRIX m_Nw;  //m_Nw[i][k] = total number of instances of word i that assigned to topic k
    std::vector< unsigned int > m_NAll; //m_NAll[k] = total number of instances of all words that assigned to topic k

    //p( topic=k |doc=m )
    T_MATRIX m_Nd; //m_Nd[m][k] = total number of words in m that assigned to k

    //words assignment
    T_MATRIX m_z; //m_z[m][n] = the assignment of n'th word in document m

    //parameters
    unsigned int m_topic_num; // topic num
    unsigned int m_max_iter; // max gibbs sampling iteration

    //hyperparameters
    double m_alpha;
    double m_beta;

    std::vector<double> m_prob; //temp prob for sampling
    unsigned int m_word_count; //number of words in the corpus

    //for inference
   T_MATRIX m_inf_z;
   T_MATRIX m_inf_Nw;
   T_MATRIX m_inf_Nd;
   std::vector< unsigned int > m_inf_NAll;
   unsigned int m_inf_word_count;

   //flag
   bool m_is_estimate;

   std::string m_model_dir;
    
}; //end of class LDA

#endif /* __LDA_H__ */
