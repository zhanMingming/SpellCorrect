#ifndef INCLUDE_CACHE_H
#define INCLUDE_CACHE_H

#include<unordered_map>
#include<vector>
#include<string>

class CacheData
{
public:
    CacheData()
    :data_vec()
    {}
    ~CacheData() {}
 
    CacheData(const std::vector<std::pair<std::string, int>> &data_vec_)
    :data_vec(data_vec_)
    {}

    std::vector<std::pair<std::string, int>> &getDataVec() {
        return data_vec;
    }

private:
    std::vector<std::pair<std::string, int>> data_vec;

};

class Cache
{
public:
    typedef std::unordered_map<std::string, CacheData> cache_map_type;
    Cache();
    ~Cache();
    void addQueryResultToCache(const std::string &keyWord, CacheData &data);
    CacheData searchCacheData(const std::string &keyword);
    void loadCacheFileToMemory();
    void writeCacheToDisk();     

    cache_map_type & getCacheDataMap() {
        return cache_map;
    }

 
private:
   cache_map_type  cache_map;  

};



#endif

