#include "lda.h"

#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include <fstream>

#include "text_tools.h"

using namespace TextTools;

LDA::LDA()
{
    m_is_estimate = true;
}

void LDA::SetParameters(
    unsigned int topic_num,
    unsigned int max_iter,
    double alpha,
    double beta,
    std::string model_dir
    )
{
    m_topic_num = topic_num;
    m_max_iter = max_iter;
    m_alpha = alpha;
    m_beta = beta;
    m_model_dir = model_dir;
}

void LDA::OutputParas()
{
    std::cout << "----LDA parameters----" << std::endl;
    std::cout << "#max iters :  " << m_max_iter << std::endl;
    std::cout << "#topic number :  " << m_topic_num << std::endl;
    std::cout << "#hyperparameter--alpha :  " << m_alpha << std::endl;
    std::cout << "#hyperparameter--beta :  " << m_beta << std::endl;
    std::cout << std::endl;
}

bool LDA::InitCorpus(
    const std::string& corpus_file , 
    const std::string& vocab_file
    )
{
    if ( !m_corpus.ReadDocuments(corpus_file) )
    {
        std::cout << "error:read corpus failed!" << corpus_file << std::endl;
        return false;
    }
    if ( !m_corpus.ReadVocab(vocab_file) )
    {
        std::cout << "error:read vocab failed!" << corpus_file << std::endl;
        return false;
    }

    return true;
}

bool LDA::LoadAssignment(
    const std::string& z_file
    )
{
    //init pre model statistics
    m_word_count = 0;
    unsigned int vocab_len = m_corpus.m_vocab.size(); // 
    
    m_Nw.clear();
    m_Nd.clear(); //useless for inference
    m_NAll.clear();
    
    m_Nw.resize(vocab_len);
    for (unsigned i = 0; i < vocab_len; ++i )
    {
        m_Nw[i].resize(m_topic_num);
    }

    m_NAll.resize(m_topic_num);
    //


    m_z.clear();

    std::ifstream fs;
    fs.open(z_file.c_str(), std::ios_base::in);

    if (fs.fail())
    {
        std::cerr << " Error ! The file isn't exist. " << z_file << std::endl;
        return false;
    }
	std::cout << "load previous assignment file :" << z_file << std::endl;
    std::string strLine;
    while (getline(fs, strLine))
    {
        if (!strLine.empty())
        {
            std::vector<unsigned int> doc_z;
        
            std::vector<std::string> vecResult;
            Split(strLine, '\t', vecResult,false);

            std::vector<std::string>::iterator iter = vecResult.begin();
            for ( ; iter != vecResult.end() ; ++iter )
            {
                unsigned int word_id = 0;
                unsigned int assignment = 0;
                
                int ret = sscanf(iter->c_str(),"%u:%u",&word_id,&assignment);
                assert(ret == 2);
                doc_z.push_back(assignment);

                m_Nw[word_id][assignment] += 1;
                m_NAll[assignment] += 1;
                m_word_count += 1;
            }

            if( doc_z.size() > 0 )
            {
                //todo optimize
                m_z.push_back(doc_z);
            }
            
        }
    }


    if (m_z.size() > 0)
    {
        std::cout << "read assignment size: " << m_z.size() << std::endl;
        return true;
    }
    else
    {
        return false;
    }
}

bool LDA::InitEstimate()
{
    m_word_count = 0;

    unsigned int docs_num = m_corpus.m_docs.size();
    unsigned int vocab_len = m_corpus.m_vocab.size();
    
    m_Nw.resize(vocab_len);
    for (unsigned i = 0; i < vocab_len; ++i )
    {
        m_Nw[i].resize(m_topic_num);
    }
    
    m_Nd.resize(docs_num);
    for (unsigned i = 0; i < docs_num; ++i )
    {
        m_Nd[i].resize(m_topic_num);
    }
    
    m_NAll.resize(m_topic_num);

    //random assignment
    srandom(time(0));
    m_z.resize(docs_num);
    for (unsigned m = 0; m < docs_num; ++m)
    {
        unsigned int doc_len = m_corpus.m_docs[m].size();
        m_z[m].resize(doc_len);
        for (unsigned n = 0; n < doc_len ; ++n )
        {
            unsigned int topic = (unsigned int)(((double)random() / RAND_MAX) * m_topic_num);
            m_z[m][n] = topic;

            unsigned int w = m_corpus.m_docs[m][n];
            m_Nw[w][topic] += 1;
            m_Nd[m][topic] += 1;
            m_NAll[topic] += 1;
            m_word_count += 1;
        }
    }

    m_prob.resize(m_topic_num);
    
    return true;
}

bool LDA::InitInference()
{
    m_inf_word_count = 0;

    unsigned int docs_num = m_corpus.m_docs.size();
    unsigned int vocab_len = m_corpus.m_vocab.size();
    
    m_inf_Nw.resize(vocab_len);
    for (unsigned i = 0; i < vocab_len; ++i )
    {
        m_inf_Nw[i].resize(m_topic_num);
    }
    
    m_inf_Nd.resize(docs_num);
    for (unsigned i = 0; i < docs_num; ++i )
    {
        m_inf_Nd[i].resize(m_topic_num);
    }
    
    m_inf_NAll.resize(m_topic_num);

    //random assignment
    srandom(time(0));
    m_inf_z.resize(docs_num);
    for (unsigned m = 0; m < docs_num; ++m)
    {
        unsigned int doc_len = m_corpus.m_docs[m].size();
        m_inf_z[m].resize(doc_len);
        for (unsigned n = 0; n < doc_len ; ++n )
        {
            unsigned int topic = (unsigned int)(((double)random() / RAND_MAX) * m_topic_num);
            m_inf_z[m][n] = topic;

            unsigned int w = m_corpus.m_docs[m][n];
            m_inf_Nw[w][topic] += 1;
            m_inf_Nd[m][topic] += 1;
            m_inf_NAll[topic] += 1;
            m_inf_word_count += 1;
        }
    }

    m_prob.resize(m_topic_num);
    
    
    return true;
}

unsigned int LDA::SamplingMultinomial(unsigned int m, unsigned int n)
{
    unsigned int old_topic = m_z[m][n];
    unsigned int w = m_corpus.m_docs[m][n];
    m_Nw[w][old_topic] -= 1;
    m_NAll[old_topic] -= 1;
    m_Nd[m][old_topic] -= 1;
    unsigned int doc_len = m_corpus.m_docs[m].size();

    //symmetric prior
    double beta_sum = m_beta * m_corpus.m_vocab.size();
    double alpha_sum = m_alpha * m_topic_num;

    // sampling by cumulative method
    for (unsigned int k = 0; k < m_topic_num; k++) {
       //as-prior
       double ratio = (m_NAll[k] + m_alpha)/(m_word_count - 1 + alpha_sum);
       double as_prior = ratio * alpha_sum;
        
	m_prob[k] = (m_Nw[w][k] + m_beta) / (m_NAll[k] + beta_sum) *
		    (m_Nd[m][k] + as_prior) / (doc_len -1 + alpha_sum);
    }
    for (unsigned int k = 1; k < m_topic_num; k++) {
	m_prob[k] += m_prob[k - 1];
    }
    double u = ((double)random() / RAND_MAX) * m_prob[m_topic_num - 1];
    
    unsigned int new_topic;
    for (new_topic = 0; new_topic < m_topic_num; new_topic++) {
	if (m_prob[new_topic] > u) {
	    break;
	}
    }
    
    m_Nw[w][new_topic] += 1;
    m_Nd[m][new_topic] += 1;
    m_NAll[new_topic] += 1;
    
    return new_topic;
}


unsigned int LDA::InfSamplingMultinomial(unsigned int m, unsigned int n)
{
    unsigned int old_topic = m_inf_z[m][n];
    unsigned int w = m_corpus.m_docs[m][n];
    
    m_inf_Nw[w][old_topic] -= 1;
    m_inf_NAll[old_topic] -= 1;
    m_inf_Nd[m][old_topic] -= 1;
    unsigned int doc_len = m_corpus.m_docs[m].size();

    //symmetric prior
    double beta_sum = m_beta * m_corpus.m_vocab.size();
    double alpha_sum = m_alpha * m_topic_num;

    // sampling by cumulative method
    for (unsigned int k = 0; k < m_topic_num; k++) {
       //as-prior
       double ratio = (m_inf_NAll[k] + m_NAll[k] + m_alpha)
            /(m_inf_word_count+ m_word_count - 1 + alpha_sum);
       double as_prior = ratio * alpha_sum;
        
	m_prob[k] = (m_inf_Nw[w][k] + m_Nw[w][k] + m_beta) / (m_inf_NAll[k] + m_NAll[k] + beta_sum) *
		    (m_inf_Nd[m][k] + as_prior) / (doc_len -1 + alpha_sum);
    }
    for (unsigned int k = 1; k < m_topic_num; k++) {
	m_prob[k] += m_prob[k - 1];
    }
    double u = ((double)random() / RAND_MAX) * m_prob[m_topic_num - 1];
    
    unsigned int new_topic;
    for (new_topic = 0; new_topic < m_topic_num; new_topic++) {
	if (m_prob[new_topic] > u) {
	    break;
	}
    }
    
    m_inf_Nw[w][new_topic] += 1;
    m_inf_Nd[m][new_topic] += 1;
    m_inf_NAll[new_topic] += 1;
    
    return new_topic;
}

bool LDA::Estimate()
{
    std::cout << "------Estimate------" << std::endl;
    OutputParas();
    m_is_estimate = true;
    InitEstimate();

    unsigned int docs_num = m_corpus.m_docs.size();

    for (unsigned int iter = 0; iter < m_max_iter; ++iter )
    {
        std::cout << "gibbs sampling iter: " << iter << std::endl;
        //sampling word-topic assignment
        for (unsigned int m = 0; m < docs_num ; ++m )
        {
            unsigned int doc_len = m_corpus.m_docs[m].size();
            for (unsigned int n = 0; n < doc_len ; ++n )
            {
                unsigned int new_topic = SamplingMultinomial(m,n);
                m_z[m][n] = new_topic;
            }
        }
    }

    CalculateResultParas();
    
    return true;
}

bool LDA::Inference(const std::string& pre_model)
{
    std::cout << "------Inference------" << std::endl;
    OutputParas();
    m_is_estimate = false;
    InitInference();
    if (!LoadAssignment(pre_model) )
    {
        return false;
    }
    
    unsigned int docs_num = m_corpus.m_docs.size();

    for (unsigned int iter = 0; iter < m_max_iter; ++iter )
    {
        std::cout << "gibbs sampling iter: " << iter << std::endl;
        //sampling word-topic assignment
        for (unsigned int m = 0; m < docs_num ; ++m )
        {
            unsigned int doc_len = m_corpus.m_docs[m].size();
            for (unsigned int n = 0; n < doc_len ; ++n )
            {
                unsigned int new_topic = InfSamplingMultinomial(m,n);
                m_inf_z[m][n] = new_topic;
            }
        }
    }

    InfCalculateResultParas();
    
    return true;
}

bool LDA::CalculateResultParas()
{
    std::cout << "saving results, base dir is " << m_model_dir << std::endl;
    
    std::string theta_file = m_model_dir + "/model.theta";
    std::ofstream ftheta(theta_file.c_str());

    if (!ftheta)
    {
        std::cerr << "save theta_file error! can't open " << theta_file << std::endl;
        return false;
    }

    std::string phi_file = m_model_dir + "/model.phi";
    std::ofstream fphi(phi_file.c_str());

    if (!fphi)
    {
        std::cerr << "save phi_file error! can't open " << phi_file << std::endl;
        return false;
    }

    std::string topic_file = m_model_dir + "/model.topic_size";
    std::ofstream ftopic(topic_file.c_str());

    if (!ftopic)
    {
        std::cerr << "save topic_file error! can't open " << topic_file << std::endl;
        return false;
    }

    unsigned int topic_sum = 0;
    for ( unsigned k = 0; k < m_topic_num; ++k )
    {
        topic_sum += m_NAll[k];
    }
    for ( unsigned k = 0; k < m_topic_num; ++k )
    {
        ftopic << m_NAll[k] << "\t" << float(m_NAll[k])/topic_sum << std::endl;
    }
    ftopic.close();

    
    unsigned int docs_num = m_corpus.m_docs.size();
    //save the resualt parameters : theta and phi
    for (unsigned int m = 0; m < docs_num ; m++) {
        
        unsigned int doc_len = m_corpus.m_docs[m].size();
        
        for (unsigned int k = 0; k < m_topic_num ; k++) {
            double alpha_sum = m_topic_num * m_alpha;
            
            double ratio = (m_NAll[k] + m_alpha)/(m_word_count + alpha_sum);
            double as_prior = ratio * alpha_sum;
            
            double theta = (m_Nd[m][k] + as_prior) / (doc_len + alpha_sum);
            
            if (k != 0)
            {
                ftheta << "\t";
            }
            ftheta << theta;
        }
        ftheta << std::endl;
    }
    unsigned int vocab_len = m_corpus.m_vocab.size();
    for (unsigned int k = 0; k < m_topic_num; k++) {
	for (unsigned int w = 0; w < vocab_len ; w++) {
	    double phi = (m_Nw[w][k] + m_beta) / (m_NAll[k] + vocab_len * m_beta);
            if (w != 0)
            {
                fphi << "\t";
            }
            fphi << phi;
	}
        fphi << std::endl;
    }

    ftheta.close();
    fphi.close();

    ///
    std::string z_file = m_model_dir + "/model.z";
    std::ofstream fz(z_file.c_str());

    if (!fz)
    {
        std::cerr << "save z_file error! can't open " << z_file << std::endl;
        return false;
    }
    
    for (unsigned int m = 0 ;m < docs_num ; ++m )
    {
        unsigned int row_len = m_z[m].size();
        for (unsigned int n = 0; n < row_len ; ++n )
        {
            fz << "\t" << m_corpus.m_docs[m][n] << ":" << m_z[m][n];
        }
        fz << std::endl;
    }
    fz.close();
    ///
    
    return true;
}

bool LDA::InfCalculateResultParas()
{
    std::cout << "saving results, base dir is " << m_model_dir << std::endl;
    
    std::string file_suffix = ".inf";

    std::string theta_file = m_model_dir + "/model.theta" + file_suffix;
    std::ofstream ftheta(theta_file.c_str());

    if (!ftheta)
    {
        std::cerr << "save theta_file error! can't open " << theta_file << std::endl;
        return false;
    }

    std::string phi_file = m_model_dir + "/model.phi"+ file_suffix;
    std::ofstream fphi(phi_file.c_str());

    if (!fphi)
    {
        std::cerr << "save phi_file error! can't open " << phi_file << std::endl;
        return false;
    }

    std::string topic_file = m_model_dir + "/model.topic_size"+ file_suffix;
    std::ofstream ftopic(topic_file.c_str());

    if (!ftopic)
    {
        std::cerr << "save topic_file error! can't open " << topic_file << std::endl;
        return false;
    }

    unsigned int topic_sum = 0;
    for ( unsigned k = 0; k < m_topic_num; ++k )
    {
        topic_sum += m_inf_NAll[k];
    }
    for ( unsigned k = 0; k < m_topic_num; ++k )
    {
        ftopic << m_inf_NAll[k] << "\t" << float(m_inf_NAll[k])/topic_sum << std::endl;
    }
    ftopic.close();

    
    unsigned int docs_num = m_corpus.m_docs.size();
    //save the resualt parameters : theta and phi
    for (unsigned int m = 0; m < docs_num ; m++) {
        
        unsigned int doc_len = m_corpus.m_docs[m].size();
        
        for (unsigned int k = 0; k < m_topic_num ; k++) {
            double alpha_sum = m_topic_num * m_alpha;

            double theta = 0;
            double ratio = (m_inf_NAll[k] + m_NAll[k] + m_alpha)
                                    /(m_inf_word_count+ m_word_count + alpha_sum);
            double as_prior = ratio * alpha_sum;
                
            theta = (m_inf_Nd[m][k] + as_prior) / (doc_len + alpha_sum);

            
            if (k != 0)
            {
                ftheta << "\t";
            }
            ftheta << theta;
        }
        ftheta << std::endl;
    }
    unsigned int vocab_len = m_corpus.m_vocab.size();
    for (unsigned int k = 0; k < m_topic_num; k++) {
	 for (unsigned int w = 0; w < vocab_len ; w++) {
            double phi = 0;
            phi = (m_inf_Nw[w][k] + m_Nw[w][k] + m_beta) 
                / (m_inf_NAll[k] + m_NAll[k] + vocab_len * m_beta);
	     
            if (w != 0)
            {
                fphi << "\t";
            }
            fphi << phi;
	}
        fphi << std::endl;
    }

    ftheta.close();
    fphi.close();

    ///
    std::string z_file = m_model_dir + "/model.z"+ file_suffix;
    std::ofstream fz(z_file.c_str());

    if (!fz)
    {
        std::cerr << "save z_file error! can't open " << z_file << std::endl;
        return false;
    }
    
    for (unsigned int m = 0 ;m < docs_num ; ++m )
    {
        unsigned int row_len = m_inf_z[m].size();
        for (unsigned int n = 0; n < row_len ; ++n )
        {
            fz << "\t" << m_corpus.m_docs[m][n] << ":" << m_inf_z[m][n];
        }
        fz << std::endl;
    }
    fz.close();
    ///
    
    return true;
}
