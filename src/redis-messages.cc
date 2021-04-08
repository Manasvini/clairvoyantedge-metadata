#include "redis-messages.h"

#include <sw/redis++/redis++.h>
namespace clairvoyantedge{
template<typename T>
std::unordered_map<std::string, std::string> RedisMessages<T>::hgetall(const std::string& key, 
                                                    const std::shared_ptr<T>& conn_p){
    std::unordered_map<std::string, std::string> values;
    return values;
}

template class RedisMessages<sw::redis::Redis>;
template class RedisMessages<sw::redis::RedisCluster>;


}
