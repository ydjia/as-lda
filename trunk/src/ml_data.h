/// @Brief: read dataset from file
/// @Date: 2012Äê5ÔÂ28ÈÕ 11:18:27
/// @Author: wangben

#ifndef __ML_DATA_H__
#define __ML_DATA_H__

#include <vector>
#include <string>
#include <set>
#include <map>

class TextData
{
public:
    TextData(){}

    ~TextData(){}

    bool ReadDocuments(const std::string& input_file);
    bool ReadVocab(const std::string& input_file);

public:

    std::vector< std::vector<unsigned int> > m_docs;
    std::map< unsigned int, std::string > m_vocab;
    
}; //end of class TextData


#endif /* __ML_DATA_H__ */
