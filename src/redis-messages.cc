#include "redis-messages.h"

#include <sw/redis++/redis++.h>
namespace clairvoyantedge{
template<typename T>
std::unordered_map<std::string, std::string> RedisMessages<T>::hgetall(const std::string& key, 
                                                    const std::shared_ptr<T>& conn_p){
    std::unordered_map<std::string, std::string> items;
    conn_p->hgetall(key, std::inserter(items, items.begin()));
    return items;
}

template<typename T>
void RedisMessages<T>::hmset(const std::string& key,
                             const std::unordered_map<std::string, std::string>& kvps,
                             const std::shared_ptr<T>& conn_p){
    conn_p->hmset(key, kvps.begin(), kvps.end());
}

template<typename T>
std::string RedisMessages<T>::get(const std::string& key, const std::shared_ptr<T>& conn_p){
    std::optional<std::string> val =  conn_p->get(key);
    return val.value_or("");
}

template<typename T>
void RedisMessages<T>::set(const std::string& key, 
                           const std::string& value,
                           const std::shared_ptr<T>& conn_p){
    conn_p->set(key, value);
}

template<typename T>
std::unordered_set<std::string> RedisMessages<T>::smembers(const std::string& key,
                                                           const std::shared_ptr<T>& conn_p){
    std::unordered_set<std::string> values;
    conn_p->smembers(key, std::inserter(values, values.begin()));
    return values;
}

template<typename T>
void RedisMessages<T>::sadd(const std::string& key, 
                            const std::unordered_set<std::string>& vals,
                            const std::shared_ptr<T>& conn_p){
    conn_p->sadd(key, vals.begin(), vals.end());
}

template<typename T>
void RedisMessages<T>::srem(const std::string& key, 
                            const std::vector<std::string>& vals,
                            const std::shared_ptr<T>& conn_p){
    conn_p->srem(key, vals.begin(), vals.end());
}

template<typename T>
std::vector<LocationInfoResult> RedisMessages<T>::georadius(const std::string& key, 
                                                            const double lon, const double lat,
                                                            const double radius,
                                                            const int maxItems,
                                                            const std::shared_ptr<T>& conn_p){
    std::pair<double, double> lonlat = std::make_pair(lon, lat);
    std::vector<std::tuple<std::string, double, std::pair<double, double> > > items;
    conn_p->georadius(key, lonlat, radius, sw::redis::GeoUnit::M, maxItems, true/*asc order*/, std::back_inserter(items)); 
    
    std::vector<LocationInfoResult> results;
    for(const auto& item: items){
        LocationInfoResult result;
        LocationInfo info;
        info.lon = std::get<2>(item).first;
        info.lat = std::get<2>(item).second;
        info.value = std::get<0>(item);
        result.info = info;
        result.distance = std::get<1>(item);
        results.push_back(result);
    }
    return results;
}

template<typename T>
void RedisMessages<T>::geoadd(const std::string& key, 
                              const std::vector<LocationInfo>& infos,
                              const std::shared_ptr<T>& conn_p){
    std::vector<std::tuple<std::string, double, double> > items;
    for(const auto& info: infos){
        std::tuple<std::string, double, double> value = std::make_tuple(info.value, info.lon, info.lat);
        items.push_back(value);
    }
    conn_p->geoadd(key, items.begin(), items.end());
}

template<typename T>
bool RedisMessages<T>::exists(const std::string& key, const std::shared_ptr<T>& conn_p){
    return conn_p->exists(key) == 1;
}

template<typename T>
void RedisMessages<T>::del(const std::string& key, const std::shared_ptr<T>& conn_p){
    conn_p->del(key);
}

template class RedisMessages<sw::redis::Redis>;
template class RedisMessages<sw::redis::RedisCluster>;

}


