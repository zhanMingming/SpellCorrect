#include"MySplit.h"
#include<iostream>

int main(int argc, char **argv)
{
    std::cout << "bitch" << std::endl;
    MySplit *sp = MySplit::getInstance();
    std::string s = "我思你love sds";
    std::cout << s << std::endl;
    std::vector<std::string> res = sp->wordSplit(s);
    std::vector<std::string>::iterator iter = res.begin();
    for(;iter != res.end(); ++iter) {
        std::cout << *iter << std::endl;
    }
}


