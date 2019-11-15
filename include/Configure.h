#ifndef INCLUDE_CONFIGURE_H_
#define INCLUDE_CONFIGURE_H_
#include <map>
#include <string>
#include "MutexLock.h"

class Configure
{
public:
	static Configure *getInstance();
    std::string getConfigByName(const std::string &name);
    static MutexLock lock_;
private:
	Configure();
    virtual ~Configure();
    //load config
    void loadConfigureToMap(const std::string &path);
    std::map<std::string, std::string> configure_map_;
    static Configure* config_;
};
#endif
