#include<iostream>
#include<string>
#include<fstream>
#include<map>
#include<cstring>
#include<sstream>

void strTrimCrlf(const char *str)
{
    char *p = (char*)&str[strlen(str)-1];
    while (*p == '\r' || *p == '\n') {
        *p-- = '\0';
    }
}
static bool isNothing(std::string str)
{
    std::ifstream  ifs("yuqi.txt");
    std::map<std::string, int>  map_vec;
    std::string line;
    while(getline(ifs, line)) {
        std::cout << line << std::endl;
        std::istringstream iss(line);
        std::string word;
        iss >> word;
        ++map_vec[word];
    }
    for (auto &i : map_vec) {
        std::cout << i.first << std::endl;
    }
    std::string ws = (*map_vec.begin()).first;
    std::cout << std::endl;
    auto iter = map_vec.find(ws);
    std::cout << "ws:" << ws  << std::endl;
    if (iter != map_vec.end()) {
        std::cout << "yes" << std::endl;
    }
    str="且";
    iter = map_vec.find(str);
    std::cout << "str:" << str  << std::endl;
    if (iter != map_vec.end()) {
        std::cout << "yes" << std::endl;
    }
     
}

int main(void)
{
    isNothing("且");
}

