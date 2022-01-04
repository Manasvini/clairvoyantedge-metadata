#include "redis-connection.h"

#include <chrono>
#include <iostream>

namespace clairvoyantmeta{

template<typename T>
RedisConnection<T>::RedisConnection():
    RedisConnection(1, "localhost", 6379, 60000)
{}

template<typename T>
RedisConnection<T>::RedisConnection(const int poolSize, const std::string& host, const int port, const int timeout){
    m_options.host = host;
    m_options.port = port;
    m_options.socket_timeout = std::chrono::milliseconds(timeout);
    m_poolOptions.size = poolSize;

    m_conn_p = std::make_shared<T>(T(m_options, m_poolOptions));
}

template<typename T>
RedisConnection<T>::RedisConnection(RedisConnection<T>& conn) {
    m_options.host =  conn.m_options.host;
    m_options.port =  conn.m_options.port;
    m_options.socket_timeout = conn.m_options.socket_timeout;
    m_poolOptions.size = conn.m_poolOptions.size;
    m_conn_p = std::make_shared<T>(T(m_options, m_poolOptions));
}

template<typename T>
std::shared_ptr<T> RedisConnection<T>::getConnection(){
    return m_conn_p;
}

template class RedisConnection<sw::redis::Redis>;
template class RedisConnection<sw::redis::RedisCluster>;


}
