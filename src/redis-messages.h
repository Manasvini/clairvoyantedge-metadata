#ifndef INCLUDED_REDISMESSAGES_H
#define INCLUDED_REDISMESSAGES_H

#include "redis-connection.h"

#include <unordered_map>


namespace clairvoyantedge{

template <typename T>
class RedisMessages{
public:

    static std::unordered_map<std::string, std::string> hgetall(const std::string& key, const std::shared_ptr<T>& conn_p);

};

}
#endif
