#ifndef INCLUDE_QUERY_H
#define INCLUDE_QUERY_H
#include "MutexLock.h"
#include "Cache.h"
#include <string>
#include <vector>

class Data
{
public:
    Data() {}
    Data(int distance_, int freq_, const std::string &word_)
    :distance(distance_),
    freq(freq_),
    word(word_)
    {}

    ~Data(){}
    
    friend bool operator < (const Data &left, const Data & right)
    {
        if (left.distance != right.distance ) {
            return left.distance > right.distance;
        } else {
            return left.freq < right.freq;
        }
    }
 
    int distance;
    int freq;
    std::string word;
};

class Query
{
public:
    static Query *getInstance();
    std::vector<std::pair<std::string, int>> getSimilarWords(const std::string & keyword, Cache  &cache);
    

private:
    Query() {}
    ~Query() {}

    static MutexLock lock;
    static Query *instance;
   
};


#endif

