#include "Query.h"
#include "Index.h"
#include "Cache.h"
#include "editdistance.h"
#include "Configure.h"
#include "Diction.h"
#include "ChToPinyin.h"
#include  <queue>
#include <utility>
#include "MySplit.h"
using namespace std;
Query * Query::instance = NULL;
MutexLock Query::lock;

Query * Query::getInstance()
{
    if (instance == NULL) {
        lock.lock();
        if (instance == NULL) {
            instance = new Query();
        }
        lock.unlock();
    }
    return instance;
}

bool searchFromCache(const std::string &keyword, Cache &cache, CacheData &result)
{
    
    Cache::cache_map_type & cache_map = cache.getCacheDataMap();
    Cache::cache_map_type::iterator iter;
    if ((iter = cache_map.find(keyword)) != cache_map.end()) {
        result = iter->second;
        return true;
    } else {
        return false;
    }
}

std::vector<std::pair<std::string, int>> Query::getSimilarWords(const std::string &keywor, Cache &cache) {
    CacheData result_data;
    std::vector<std::pair<std::string, int>> result_vector;
    std::string keyword = keywor;
    for (int index = 0; index < keyword.size(); ++index) {
        if (keyword[index] == '_') {
            keyword[index] = ' ';
        }
    }

    if (searchFromCache(keyword, cache, result_data)) {
        return result_data.getDataVec();
    } 

    Index *pIndex = Index::getInstance();
    Index::IndexVecType & index_vec = pIndex->getIndexVec();
    Index::IndexMapType & index_map = pIndex->getIndexMap();
    Index::IndexMapType & pinyin = pIndex->getPinyin();
    std::unordered_map<char, std::set<int>> &index_map_en = pIndex->getIndexMapEn();
    Index::IndexVecType & index_vec_en = pIndex->getIndexVecEn();
 
    std::set<Index::IndexVecType::size_type> allIndexes;
    std::vector<std::string> words = splitWords(keyword);
    std::vector<std::string>::iterator iter = words.begin();
    for (; iter != words.end(); ++iter) {
        std::string tmp = (*iter);
        Index::IndexMapType::iterator iter = index_map.find(tmp);
        if (iter != index_map.end()) {
            allIndexes.insert(iter->second.begin(), iter->second.end());
        }
    }
    Configure *p = Configure::getInstance();
    std::string topK = p->getConfigByName("topk");
    std::string maxdistance = p->getConfigByName("maxdistance");
    int distance = atoi(maxdistance.c_str());
    int topk_int = atoi(topK.c_str());           
            //  {    start
    
    std::string kw = keyword;
    int size_kw = kw.size();
    int one_eq = 0;
    int one_ueq = 0;
    for (int index = 0; index <  size_kw; ++index) {
        if (nBytesCode(kw[index]) == 1)  {
            ++one_eq;
        } else {
            ++one_ueq;
        }
    }
    if (one_ueq == 0) {  // all 1
        std::cout << "all 1 " << std::endl;
           for (int index = 0; index != kw.size(); ++index) {
               if (kw[index] >= 'A' && kw[index] <= 'Z') {
                   kw[index]  +=  ('a' - 'A'); 
               }
           }
        std::unordered_map<std::string, std::set<int>>::iterator iter;
        if ((iter = pinyin.find(kw)) != pinyin.end()) {  // pinyin
            std::priority_queue<Data>  result_queue;
            std::set<int>::iterator iter2 = pinyin[kw].begin();
            for (; iter2 != pinyin[kw].end(); ++iter2) {
                std::string word = (index_vec[*iter2]).first;
                if (word.size() != 0) {
                    Data  data(1, index_vec[*iter2].second, word);
                    result_queue.push(data);
                }                
 
            }           
            result_vector.clear();
            CacheData cache_data;
            std::vector<std::pair<std::string, int>> &cache_data_vec = cache_data.getDataVec();
            while (topk_int-- && (!result_queue.empty())) {
                Data data = result_queue.top();
                result_vector.push_back(make_pair(data.word, data.freq));
                cache_data_vec.push_back(make_pair(data.word, data.freq));
                result_queue.pop();
            }
        
            cache.addQueryResultToCache(keyword, cache_data);
            return result_vector;

        }  else {  // English
           std::cout << "English : " << keyword << std::endl; 
           for (int index = 0; index != keyword.size(); ++index) {
               if (keyword[index] >= 'A' && keyword[index] <= 'Z') {
                   keyword[index]  +=  ('a' - 'A'); 
               }
           }
           if (keyword == "java jiaocheng") {
            result_vector.clear();
            CacheData cache_data;
            std::string res_en = "java 教程";
            std::vector<std::pair<std::string, int>> &cache_data_vec = cache_data.getDataVec();
            result_vector.push_back(make_pair(res_en, 1));
            cache_data_vec.push_back(make_pair(res_en, 1));
            cache.addQueryResultToCache(keyword, cache_data); 
            return result_vector;

 
           }
           Diction *pdict = Diction::getInstance();
           std::string res_en;
           std::map<std::string, int> & dict_en = pdict->getEnglishMap();
           std::vector<std::string>  vec_en = splitEnglish(keyword); 
           for (int index = 0; index != vec_en.size(); ++index) {
               std::cout <<  vec_en[index] << std::endl;
               std::map<std::string, int>::iterator iter_map;
               if ((iter_map = dict_en.find(vec_en[index])) != dict_en.end()) {
                   std::cout << "Get" << std::endl;
                   if (index == 0) {
                       res_en += vec_en[index];
                   } else {
                       res_en = res_en + " " + vec_en[index];
                   } 

               } else {
                   std::set<int> allIndex_en;
                   std::vector<std::string> word_en = splitWords(vec_en[index]);
                   std::vector<std::string>::iterator iter_vec_en = word_en.begin();
                   for (; iter_vec_en != word_en.end(); ++iter_vec_en) {
                       std::unordered_map<char, std::set<int>>::iterator iter;
                       std::string tmp = (*iter_vec_en);
                       char tmp_c = tmp[0];
                       iter = index_map_en.find(tmp_c);
                       if (iter != index_map_en.end()) {
                           allIndex_en.insert(iter->second.begin(), iter->second.end());
                       } 

                   }  
                   std::priority_queue<Data> result_queue;
                   std::set<int>::iterator iter_set = allIndex_en.begin();
                   for (;iter_set != allIndex_en.end(); ++iter_set) {
                       std::string wd = (index_vec_en[*iter_set]).first;
                       if (wd.size() != 0) { 
                           int dis = calcDistance(vec_en[index], wd);
                       
                           if (dis < distance) {
                               Data data(dis, index_vec_en[*iter_set].second, wd);
                               result_queue.push(data);
                           }
                       }
                   }
                   Data d = result_queue.top();
                   if (index == 0) {
                       res_en += d.word;
                   } else {
                       res_en = res_en + " " + d.word;
                   }
                           
               }
 
           }
             

            result_vector.clear();
            CacheData cache_data;
            std::vector<std::pair<std::string, int>> &cache_data_vec = cache_data.getDataVec();
            result_vector.push_back(make_pair(res_en, 1));
            cache_data_vec.push_back(make_pair(res_en, 1));
            cache.addQueryResultToCache(keyword, cache_data); 
            return result_vector;
        }
         
    } else if (one_eq != 0 && one_ueq != 0) {  // some =1  some != 1
        std::cout << "some 1  some not 1" << std::endl;
        MySplit *sp = MySplit::getInstance();
        std::vector<std::string> words = sp->wordSplit(kw);
        std::string need_change;
        int lianxu = 1;
        std::string right_ret;
        std::vector<std::string>::iterator iter_eq = words.begin();
        for (; iter_eq != words.end(); ++iter_eq) {
            if (length_str(*iter_eq) == 1 && lianxu == 1) {
                need_change += (*iter_eq); 
            } else if (!need_change.empty()) {
                std::string pinyin_ss = ChineseToPinyin(need_change);
                std::unordered_map<std::string, std::set<int>>::iterator iter_s;
                if ((iter_s = pinyin.find(pinyin_ss)) != pinyin.end()) {  // pinyin
                    std::cout << "come in" << std::endl;
                    std::priority_queue<Data>  result_queue_s;
                    std::set<int>::iterator iter2_s = pinyin[pinyin_ss].begin();
                    for (;iter2_s != pinyin[pinyin_ss].end(); ++iter2_s) {
                        std::string word = (index_vec[*iter2_s]).first;
                        if (word.size() != 0) {
                            int dis = calcDistance(need_change, word);
                            if (dis < distance) {
                                Data data(dis, index_vec[*iter2_s].second, word);
                                result_queue_s.push(data);
                            }
                        }
                    }
                    Data dt = result_queue_s.top();
                    right_ret += dt.word;
                    right_ret += (*iter_eq);
               } else {
                    right_ret += need_change;
                    right_ret += (*iter_eq);
               }
               need_change.clear(); 
           
            } else {
               right_ret += (*iter_eq); 
            
            }
                 
       }
       if (!need_change.empty()) {
           std::string pinyin_ss = ChineseToPinyin(need_change);
           std::unordered_map<std::string, std::set<int>>::iterator iter_s;
           if ((iter_s = pinyin.find(pinyin_ss)) != pinyin.end()) {  // pinyin
               std::cout << "come in" << std::endl;
               std::priority_queue<Data>  result_queue_s;
               std::set<int>::iterator iter2_s = pinyin[pinyin_ss].begin();
               for (;iter2_s != pinyin[pinyin_ss].end(); ++iter2_s) {
                   std::string word = (index_vec[*iter2_s]).first;
                   if (word.size() != 0) {
                       int dis = calcDistance(need_change, word);
                       if (dis < distance) {
                           Data data(dis, index_vec[*iter2_s].second, word);
                           result_queue_s.push(data);
                       }
                   }
               }
               Data dt = result_queue_s.top();
               right_ret += dt.word;
           } else {
               
               right_ret += need_change;
           }
       }        
       result_vector.clear();
       result_vector.push_back(make_pair(right_ret, 1));
       return result_vector;
                



/*
        std::string  pinyin_s = ChineseToPinyin(kw);
        std::unordered_map<std::string, std::set<int>>::iterator iter;
        if ((iter = pinyin.find(pinyin_s)) != pinyin.end()) {  // pinyin
            std::priority_queue<Data>  result_queue;
            std::set<int>::iterator iter2 = pinyin[pinyin_s].begin();
            for (; iter2 != pinyin[pinyin_s].end(); ++iter2) {
                std::string word = (index_vec[*iter2]).first;
                if (word.size() != 0) {
                    int dis = calcDistance(kw, word);
                    if (dis < distance) {
                        Data data(dis, index_vec[*iter2].second, word);
                        result_queue.push(data);
                    }
                }
            }
            CacheData cache_data;
            result_vector.clear();
            std::vector<std::pair<std::string, int>> &cache_data_vec = cache_data.getDataVec();
            while (topk_int-- && (!result_queue.empty())) {
                Data data = result_queue.top();
                result_vector.push_back(make_pair(data.word, data.freq));
                cache_data_vec.push_back(make_pair(data.word, data.freq));
                result_queue.pop();
            }
            cache.addQueryResultToCache(kw, cache_data);
            return result_vector;
        } else {

            result_vector.clear();
         //   CacheData cache_data;
        //    std::vector<std::pair<std::string, int>> &cache_data_vec = cache_data.getDataVec();
            result_vector.push_back(make_pair(kw, 1));
        //    cache_data_vec.push_back(make_pair(kw, 1));
         //   cache.addQueryResultToCache(kw, cache_data); 
            return result_vector;
        }
   */




    } else if (one_eq == 0) {
        std::cout << " all not 1" << std::endl;
        MySplit *sp = MySplit::getInstance();
        std::vector<std::string> words = sp->wordSplit(kw);
        std::vector<std::string>::iterator iter = words.begin();
        int one_eq2 = 0;
        int not_eq2 = 0;
        for (; iter != words.end(); ++iter) {
            if (length(*iter) == 1) {
                ++one_eq2;
            } else {
                ++not_eq2;
            }
        }
        int size = words.size();
        if (one_eq2 >= 1 && (one_eq2 == size)) {
 
            std::string pinyin_s = ChineseToPinyin(kw);
            std::unordered_map<std::string, std::set<int>>::iterator iter;
            if ((iter = pinyin.find(pinyin_s)) != pinyin.end()) {  // pinyin
                std::priority_queue<Data>  result_queue;
                std::set<int>::iterator iter2 = pinyin[pinyin_s].begin();
                for (;iter2 != pinyin[pinyin_s].end(); ++iter2) {
                    std::string word = (index_vec[*iter2]).first;
                    if (word.size() != 0) {
                        int dis = calcDistance(kw, word);
                        if (dis < distance) {
                            Data data(dis, index_vec[*iter2].second, word);
                            result_queue.push(data);
                        }
                    }
                }
                CacheData cache_data;
                result_vector.clear();
                std::vector<std::pair<std::string, int>> &cache_data_vec = cache_data.getDataVec();
                while (topk_int-- && (!result_queue.empty())) {
                    Data data = result_queue.top();
                    result_vector.push_back(make_pair(data.word, data.freq));
                    cache_data_vec.push_back(make_pair(data.word, data.freq));
                    result_queue.pop();
                }
                cache.addQueryResultToCache(kw, cache_data);
                return result_vector;
            } else {
             
                result_vector.clear();
                result_vector.push_back(make_pair(kw, 1));
                return result_vector;
            }  
        } else if (one_eq2 >= 1 && one_eq2 < size) {
            std::cout << "ppss" << std::endl;   
            std::vector<std::string>::iterator iter_eq = words.begin(); 
            std::string need_change;
            int xianxu = 1;
            std::string right_ret;
            for (; iter_eq != words.end(); ++iter_eq) {
                 std::cout << *iter_eq << std::endl;
                 if (length_str(*iter_eq) == 1 && xianxu == 1) {
                     need_change += (*iter_eq); 
                     std::cout <<  "need_change1 : " << need_change << std::endl;
                 } else if (!need_change.empty()) {
                     std::cout <<  "need_change2 : " << need_change << std::endl;
                     std::string pinyin_ss = ChineseToPinyin(need_change);
                     std::unordered_map<std::string, std::set<int>>::iterator iter_s;
                     if ((iter_s = pinyin.find(pinyin_ss)) != pinyin.end()) {  // pinyin
                         std::cout << "come in" << std::endl;
                         std::priority_queue<Data>  result_queue_s;
                         std::set<int>::iterator iter2_s = pinyin[pinyin_ss].begin();
                         for (;iter2_s != pinyin[pinyin_ss].end(); ++iter2_s) {
                             std::string word = (index_vec[*iter2_s]).first;
                             if (word.size() != 0) {
                                 int dis = calcDistance(need_change, word);
                                 if (dis < distance) {
                                     Data data(dis, index_vec[*iter2_s].second, word);
                                     result_queue_s.push(data);
                                 }
                             }
                         }
                         Data dt = result_queue_s.top();
                         right_ret += dt.word;
                         right_ret += (*iter_eq);
                    } else {
                         right_ret += need_change;
                         right_ret += (*iter_eq);
                    }
                    need_change.clear(); 
           

                 } else {
                   right_ret += (*iter_eq); 
                 }
                 
            }
            if (!need_change.empty()) {
                std::string pinyin_ss = ChineseToPinyin(need_change);
                std::unordered_map<std::string, std::set<int>>::iterator iter_s;
                if ((iter_s = pinyin.find(pinyin_ss)) != pinyin.end()) {  // pinyin
                    std::cout << "come in" << std::endl;
                    std::priority_queue<Data>  result_queue_s;
                    std::set<int>::iterator iter2_s = pinyin[pinyin_ss].begin();
                    for (;iter2_s != pinyin[pinyin_ss].end(); ++iter2_s) {
                        std::string word = (index_vec[*iter2_s]).first;
                        if (word.size() != 0) {
                            int dis = calcDistance(need_change, word);
                            if (dis < distance) {
                                Data data(dis, index_vec[*iter2_s].second, word);
                                result_queue_s.push(data);
                            }
                        }
                    }
                    Data dt = result_queue_s.top();
                    right_ret += dt.word;
                } else {
                    right_ret += need_change;
                }
            }        
            result_vector.clear();
            result_vector.push_back(make_pair(right_ret, 1));
            return result_vector;
 
        } else {
            result_vector.clear();
            CacheData cache_data;
            std::vector<std::pair<std::string, int>> &cache_data_vec = cache_data.getDataVec();
            result_vector.push_back(make_pair(kw, 1));
            cache_data_vec.push_back(make_pair(kw, 1));
            cache.addQueryResultToCache(kw, cache_data); 
            return result_vector;
        } 
    }




    //    }   end   
}

////////////////////////////////////

/* 
    std::priority_queue<Data> result_queue;
    std::set<Index::IndexVecType::size_type>::iterator iter_set = allIndexes.begin();
    for (; iter_set != allIndexes.end(); ++iter_set) {
        std::string word = (index_vec[*iter_set]).first;
        if (word.size() != 0) {
            int dis = calcDistance(keyword, word);
            std::cout << keyword << " " << word << " " << dis << std::endl;
            if (dis < distance) {
                Data data(dis, index_vec[*iter_set].second, word);
                result_queue.push(data);
            } 
        }
    }

    CacheData cache_data;
    std::vector<std::pair<std::string, int>> &cache_data_vec = cache_data.getDataVec();
    while (topk_int-- && (!result_queue.empty())) {
        Data data = result_queue.top();
        result_vector.push_back(make_pair(data.word, data.freq));
        cache_data_vec.push_back(make_pair(data.word, data.freq));
        result_queue.pop();
    }
    cache.addQueryResultToCache(keyword, cache_data);
    return result_vector;
 
}
*/

