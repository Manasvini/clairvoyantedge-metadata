#include "redis-connection.h"

#include <chrono>
#include <iostream>

namespace clairvoyantedge{

template<typename T>
RedisConnection<T>::RedisConnection():
    RedisConnection(1, "localhost", 6379, 60000)
{}

template<typename T>
RedisConnection<T>::RedisConnection(const int poolSize, const std::string& host, const int port, const int timeout){
    sw::redis::ConnectionOptions options;
    options.host = host;
    options.port = port;
    options.socket_timeout = std::chrono::milliseconds(timeout);
    sw::redis::ConnectionPoolOptions poolOptions;
    poolOptions.size = poolSize;

    m_conn_p = std::make_shared<T>(T(options, poolOptions));
}

template<typename T>
std::shared_ptr<T> RedisConnection<T>::getConnection(){
    return m_conn_p;
}

template class RedisConnection<sw::redis::Redis>;
template class RedisConnection<sw::redis::RedisCluster>;


}
