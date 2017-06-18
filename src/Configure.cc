#include "Configure.h"
#include <fstream>
#include <sstream>

using namespace std;

Configure* Configure::config_ = NULL;
MutexLock Configure::lock_;

Configure *Configure::getInstance(){
	if(config_==NULL){
		lock_.lock();
		if(config_==NULL){
			config_ = new Configure();
		}
		lock_.unlock();
	}
	return config_;
}

Configure::Configure()
:configure_map_() {

    loadConfigureToMap("/home/zhan/c++/spellCorrect/conf/config.conf");
}

Configure::~Configure() {

}

//load config
void Configure::loadConfigureToMap(const string &path){
	ifstream ifs(path.c_str());
    if (!ifs.is_open()) {
        throw runtime_error("open file fail");
    }

    std::string line;
	while (getline(ifs, line)){
        std::string key, value;
		istringstream iss(line);
        iss >> key;
        iss >> value;
		configure_map_[key] = value;
	}
}
//get Config by name
string Configure::getConfigByName(const string &name){
	map<string,string>::iterator value;
	if((value = configure_map_.find(name))!=configure_map_.end()){
		return value->second;
	}else{
		return string("not_found");
	}
}
