#include "Diction.h"
#include <iostream>

int main(void)
{
    int num = 0;
    Diction *dict = Diction::getInstance();
    std::map<std::string, int> & dict_map =  dict->getDictMap();
    std::map<std::string, int>::iterator iter = dict_map.begin();
    for (; iter != dict_map.end(); ++iter) {
        ++num;
        std::cout << iter->first << ":" << iter->second << std::endl;
    }


}

