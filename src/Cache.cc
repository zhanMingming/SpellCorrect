#include "Cache.h"
#include "Configure.h"
#include <utility>
#include <sstream>
#include <fstream>
#include <iostream>
using namespace std;

Cache::Cache()
:cache_map()
{
    loadCacheFileToMemory();
}

Cache::~Cache()
{
    writeCacheToDisk();
}


void Cache::addQueryResultToCache(const std::string &keyword, CacheData &data)
{
    std::pair<std::string, CacheData> data_pair = make_pair(keyword, data);
    cache_map.insert(data_pair);     
}


CacheData Cache::searchCacheData(const std::string &keyword)
{
    cache_map_type::iterator iter;
    if ((iter = cache_map.find(keyword)) != cache_map.end()) {
        return iter->second;
    } else {
        return CacheData();
    }

}

void Cache::loadCacheFileToMemory()
{
    Configure *p = Configure::getInstance();
    std::string  home = p->getConfigByName("home");
    std::string  cache_file = p->getConfigByName("cache_file");
    ifstream ifs((home + cache_file).c_str());
    if (!(ifs.is_open())) {
        throw runtime_error("open cache_path file fail");
    }
    std::string line, keyword, pairword;
    int freq;
    while (getline(ifs, line)) {
        istringstream iss(line);
        iss >> keyword;
        std::vector<std::pair<std::string, int>> vec;
        while (iss >> pairword) {
            iss >> freq;
            vec.push_back(make_pair(pairword, freq));
        }
        CacheData  data(vec);
        cache_map.insert(make_pair(keyword, data));
    }
    ifs.close();
}
  
void Cache::writeCacheToDisk()
{
    Configure *p = Configure::getInstance();
    std::string home = p->getConfigByName("home");
    std::string cache_file = p->getConfigByName("cache_file");
    ofstream ofs((home + cache_file).c_str());
    if (!ofs.is_open()) {
        throw runtime_error("wirte cache_path file fail");
    }
    for (Cache::cache_map_type::iterator iter = cache_map.begin(); iter != cache_map.end(); ++iter) {
        ofs << iter->first << "\t";
        std::vector<pair<std::string, int>> & vec = iter->second.getDataVec();
        for(std::vector<pair<std::string, int>>::iterator iter = vec.begin(); iter != vec.end(); ++iter) {
            ofs << iter->first << " " << iter->second << " ";
        }
        ofs << "\n";
    }
    ofs.close();
}

         
