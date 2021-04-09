#ifndef INCLUDED_REDISMESSAGES_H
#define INCLUDED_REDISMESSAGES_H

#include "redis-connection.h"

#include <unordered_map>
#include <vector>
#include <unordered_set>

namespace clairvoyantedge{

struct LocationInfo{
        double lon;
        double lat;
        std::string value;
    };
    struct LocationInfoResult{
        LocationInfo info;
        double distance;
    };

template <typename T>
class RedisMessages{
public:

    // hashmap methods
    static std::unordered_map<std::string, std::string> hgetall(const std::string& key, 
                                                                const std::shared_ptr<T>& conn_p);

    static void hmset(const std::string& key,
                      const std::unordered_map<std::string, std::string>& kvps,
                      const std::shared_ptr<T>& conn_p);

    // string methods
    static std::string get(const std::string& key, const std::shared_ptr<T>& conn_p);

    static void set(const std::string& key, 
                    const std::string& value,
                    const std::shared_ptr<T>& conn_p);

    // set methods
    static std::unordered_set<std::string> smembers(const std::string& key,
                                             const std::shared_ptr<T>& conn_p);

    static void sadd(const std::string& key, 
                     const std::unordered_set<std::string>& vals,
                     const std::shared_ptr<T>& conn_p);

    static void srem(const std::string& key, 
                     const std::vector<std::string>& vals,
                     const std::shared_ptr<T>& conn_p);

    // geo methods
    
    static std::vector<LocationInfoResult> georadius(const std::string& key, 
                                                     const double lon, const double lat,
                                                     const double radius,
                                                     const int maxItems,
                                                     const std::shared_ptr<T>& conn_p);

    static void geoadd(const std::string& key, 
                       const std::vector<LocationInfo>& info,
                       const std::shared_ptr<T>& conn_p);

    
};

}
#endif
