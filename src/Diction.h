#ifndef INCLUDE_DICTION_H
#define INCLUDE_DICTION_H
#include <string>
#include <map>
#include <set>
#include "MutexLock.h"


class Diction
{
public:
    static Diction *getInstance();
    
    std::map<std::string, int> & getDictMap()
    {
        return dict_map; 
        
    }
    
    std::map<std::string, int> & getEnglishMap()
    {
        return dict_map_en;
    }
private:
    Diction();
    ~Diction() {}
    void createCHDict();
    void createENDict();
    void getExcludeSet();
    void getSelfDat();
    void getSelfDatEn();
    void traverseDir(const char *dir, bool isEnglish);
    void readFileToMap(const char *file, bool isEnglish);
private:
    std::map<std::string, int> dict_map;
    std::set<std::string> exclude_set;
    std::map<std::string, int> dict_map_en;
    static Diction *instance;
    static MutexLock lock;
     
    
}; 

#endif

