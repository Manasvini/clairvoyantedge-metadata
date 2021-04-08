#include "redis-connection.h"

#include <chrono>
#include <iostream>

namespace clairvoyantedge{

RedisConnection::RedisConnection():
    RedisConnection(1, "localhost", 6379, 60000)
{}

RedisConnection::RedisConnection(const int poolSize, const std::string& host, const int port, const int timeout){
    sw::redis::ConnectionOptions options;
    options.host = host;
    options.port = port;
    options.socket_timeout = std::chrono::milliseconds(timeout);
    sw::redis::ConnectionPoolOptions poolOptions;
    poolOptions.size = poolSize;

    m_conn_p = std::make_shared<sw::redis::Redis>(sw::redis::Redis(options, poolOptions));
}

std::shared_ptr<sw::redis::Redis> RedisConnection::getConnection(){
    return m_conn_p;
}

}
