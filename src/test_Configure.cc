#include "Configure.h"
#include <iostream>
#include <string>
int main(void)
{
    Configure *p = Configure::getInstance();
    std::string ip = p->getConfigByName("ip");
    std::string ipp = p->getConfigByName("ipp");
    std::string port1 = p->getConfigByName("potr");
    std::string port = p->getConfigByName("port");
    std::cout << ip << std::endl;
    std::cout << ipp << std::endl;
    std::cout << port1 << std::endl;
    std::cout << port << std::endl;
}

