#ifndef INCLUDED_REDISMESSAGES_H
#define INCLUDED_REDISMESSAGES_H

#include "redis-connection.h"

#include <unordered_map>
#include <vector>

namespace clairvoyantedge{


template <typename T>
class RedisMessages{
public:

    // hashmap methods
    static std::unordered_map<std::string, std::string> hgetall(const std::string& key, 
                                                                const std::shared_ptr<T>& conn_p);

    static void hmset(const std::string& key,
                      const std::vector<std::pair<std::string, std::string> >& kvps,
                      const std::shared_ptr<T>& conn_p);

    // string methods
    static std::string get(const std::string& key, const std::shared_ptr<T>& conn_p);

    static void set(const std::string& key, 
                    const std::string& value,
                    const std::shared_ptr<T>& conn_p);

    // set methods
    static std::vector<std::string> smembers(const std::string& key,
                                             const std::shared_ptr<T>& conn_p);

    static void sadd(const std::string& key, 
                     const std::vector<std::string>& vals,
                     const std::shared_ptr<T>& conn_p);

    static void srem(const std::string& key, 
                     const std::vector<std::string>& vals,
                     const std::shared_ptr<T>& conn_p);

    // geo methods
    struct LocationInfo{
        double lon;
        double lat;
        std::string value;
    };
    struct LocationInfoResult{
        LocationInfo info;
        double distance;
    };
    
    static std::vector<LocationInfoResult> georadius(const std::string& key, 
                                                     const double lon, const double lat,
                                                     const double radius,
                                                     const std::shared_ptr<T>& conn_p);

    static void geoadd(const std::string& key, 
                       const std::vector<LocationInfo>& info,
                       const std::shared_ptr<T>& conn_p);

    
};

}
#endif
