#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <errno.h>

#include "text_tools.h"
#include "ml_data.h"

using namespace TextTools;

bool TextData::ReadDocuments(const std::string& input_file)
{
    std::ifstream fs;
    fs.open(input_file.c_str(), std::ios_base::in);

    if (fs.fail())
    {
        std::cerr << " Sorry ! The file isn't exist. " << input_file << std::endl;
        return false;
    }
    std::cout << "Reading " << input_file << std::endl;
    std::string strLine;
    while (getline(fs, strLine))
    {
        if (!strLine.empty())
        {
            std::vector<unsigned int> doc;
        
            std::vector<std::string> vecResult;
            Split(strLine, '\t', vecResult,true);

            std::vector<std::string>::iterator iter = vecResult.begin();
            for ( ; iter != vecResult.end() ; ++iter )
            {
                std::stringstream sstream;
                sstream << *iter;
                unsigned int w;
                sstream >> w;
                doc.push_back(w);
            }
            
            if( doc.size() > 0 )
            {
                //todo optimize
                m_docs.push_back(doc);
            }
            
        }
    }
    
    if (m_docs.size() > 0)
    {
        std::cout << "read corpus size: " << m_docs.size() << std::endl;
        return true;
    }
    else
    {
        return false;
    }
    
}

bool TextData::ReadVocab(const std::string& input_file)
{
    std::ifstream fs;
    fs.open(input_file.c_str(), std::ios_base::in);

    if (fs.fail())
    {
        std::cerr << " Sorry ! The file isn't exist. " << input_file << std::endl;
        return false;
    }
    std::cout << "Reading " << input_file << std::endl;
    
    unsigned int count = 0;
    std::string strLine;
    while (getline(fs, strLine))
    {
        m_vocab[count] = strLine;
        count += 1;
    }
    
    if (m_vocab.size() > 0)
    {
        std::cout << "read vocab size: " << m_vocab.size() << std::endl;
        return true;
    }
    else
    {
        return false;
    }
}

