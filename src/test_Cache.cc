#include "Cache.h"
#include <iostream>
int main(void)
{
    Cache cache;
    cache.loadCacheFileToMemory();
    Cache::cache_map_type cache_map = cache.getCacheDataMap();
    for (auto &iter : cache_map) {
        std::cout << iter.first << " ";
        for (auto & iter2 : iter.second.getDataVec()) {
            std::cout << iter2.first << ":" << iter2.second << " ";
        }
        std::cout << "\n";
    }   
    
}

