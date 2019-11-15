#include "Index.h"
#include <iostream>
int main(void)
{
    Index *p = Index::getInstance();
    Index::IndexVecType &vec = p->getIndexVec();
    Index::IndexMapType &idx = p->getIndexMap();
    for (int index = 0; index != 100; ++index) {
        std::cout << vec[index].first << ":" << vec[index].second << std::endl;
    }
    for (auto &iter : idx) {
        std::cout << iter.first << " :";
        for (auto &i : iter.second) {
            std::cout << i << ' ';
        }
        std::cout << std::endl;
    }

}
 
