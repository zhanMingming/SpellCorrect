#ifndef INCLUDE_EDIT_H
#define INCLUDE_EDIT_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
using namespace std;

static std::size_t nBytesCode(const char ch)
{
	std::size_t nBytes = 0;
	if(ch & (1 << 7))//   1000 0000
	{//对中文进行处理
		if((ch & 0xF0) == 0xC0 || (ch & 0xF0) == 0xD0 ) // 1111 0000
#if 0
								   110x xxxx
								   110x 0000
								   1101 0000 D0
								   1100 0000 C0
#endif
		{//2字节编码
			nBytes += 2;	
		}
		else if((ch & 0xF0) == 0xE0)
		{//3字节编码
			nBytes += 3;
		}
		else if((ch & 0xF0) == 0xF0)
		{//4字节编码
			nBytes += 4;
		}
		else if((ch & 0xFF) == 0xF8) // 1111 1111 
									 // 1111 10xx 
									 // 1111 10xx
		{//5字节编码
			nBytes += 5;
		}
		else if((ch & 0xFF) == 0xFE)
		{//6字节编码
			nBytes += 6;
		}
	}
	else{//1字节编码或英文
		nBytes += 1;
	}
	return nBytes;
}

static std::vector<std::string> splitWords(const std::string &word)
{
    std::vector<std::string> res;
    for (int index= 0; index != word.size();) {
        int nbytes = nBytesCode(word[index]);
        std::string tmp = word.substr(index, nbytes);
        index += nbytes;
        res.push_back(tmp);
    }
    return res; 
}
static std::vector<std::string> splitEnglish(const std::string word)
{
    std::vector<std::string> res;
    int start = 0;
    for (int index = 0; index != word.size(); ++index) {
        if (word[index] == ' ') {
            res.push_back(word.substr(start, index));
            start = index + 1;
        }
    }
    res.push_back(word.substr(start));
    return res;
}
static bool isNothing(const std::string &str)
{
    std::ifstream  ifs("yuqi.txt");
    std::map<std::string, int>  map_vec;
    std::string line;
    while(getline(ifs, line)) {
       std::istringstream iss(line);
       std::string word;
       iss >> word;
        ++map_vec[word];
    }
    ifs.close();
    std::map<std::string,int>::iterator iter;
    iter = map_vec.find(str);
    if (iter != map_vec.end()) {
        return true;
    } else return false; 
}

static std::size_t length(const std::string &str)
{//获取字符串的长度
	std::size_t ilen = 0;
	std::size_t sz = str.size();
	for(std::size_t idx = 0; idx < sz; ++idx)
	{
		int nBytes = nBytesCode(str[idx]);
		idx += (nBytes - 1);
		++ilen;
	}
	return ilen;
}
static std::size_t length_str(const std::string &str)
{
    if (isNothing(str)) {
        return 0;
    }
    std::size_t ilen = 0;
    std::size_t sz = str.size();
    for (std::size_t idx = 0; idx < sz; ++idx) {
        int nByt = nBytesCode(str[idx]);
        if (nByt != 1) {
            return length(str);
        }
    }
    
    return 1;
}

static int tripleMin(const int &a, const int &b, const int &c)
{
	return (a < b ?	(a < c ? a : c) : (b < c ? b : c));
}


static int calcDistance(const std::string &lhs, const std::string &rhs)
{
	std::size_t lhsLen = length(lhs);
	std::size_t rhsLen = length(rhs);
	
	int editDisArray[lhsLen + 1][rhsLen + 1];

	for(std::size_t idx = 0; idx <= lhsLen; ++idx)
	{
		editDisArray[idx][0] = idx;
	}

	for(std::size_t idx = 0; idx <= rhsLen; ++idx)
	{
		editDisArray[0][idx] = idx;
	}
	
	std::string sublhs, subrhs;

	for(std::size_t iIdx = 1, lhsIdx = 0; iIdx <= lhsLen; ++iIdx)
	{
		size_t nBytes = nBytesCode(lhs[lhsIdx]);
		sublhs = lhs.substr(lhsIdx, nBytes);
		lhsIdx += nBytes;

		for(std::size_t jIdx = 1, rhsIdx = 0; jIdx <= rhsLen; ++jIdx)
		{
			nBytes = nBytesCode(rhs[rhsIdx]);
			subrhs = rhs.substr(rhsIdx, nBytes);
			rhsIdx += nBytes;

			if(sublhs == subrhs)
			{
				editDisArray[iIdx][jIdx] = editDisArray[iIdx - 1][jIdx - 1];
			}
			else
			{
				editDisArray[iIdx][jIdx] = tripleMin(
					editDisArray[iIdx][jIdx -1] + 1,
					editDisArray[iIdx - 1][jIdx] + 1,
					editDisArray[iIdx - 1][jIdx - 1] + 1) ;

			}
		}
	}
	return editDisArray[lhsLen][rhsLen];
}

#endif

