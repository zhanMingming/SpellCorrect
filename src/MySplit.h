#ifndef _MYSPLIT_H
#define _MYSPLIT_H
#include "../../split/include/NLPIR.h"
#include<string>
#include<vector>
#include<sstream>
#include<unistd.h>
#include<stdlib.h>
#include "MutexLock.h"

class MySplit
{
private:
	MySplit()
	{
		if(!NLPIR_Init("/home/zhan/c++/split", UTF8_CODE))
		{
//			printf("init fails\n");
			exit(EXIT_FAILURE);
		}
	}
public:
    static MySplit * getInstance(); 
	~MySplit()
	{
		NLPIR_Exit();
	}

	std::vector<std::string> wordSplit(std::string & txt)
	{
		std::vector<std::string> vec;
		const char *sResult;
		sResult = NLPIR_ParagraphProcess(txt.c_str(), 0);
		std::stringstream ss(sResult);
		std::string word;
		while(ss >> word)
		{
			vec.push_back(word);
		}

		return vec;
	}
static MySplit *instance;
static MutexLock lock;
};

#endif
