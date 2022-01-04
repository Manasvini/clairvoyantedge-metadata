#ifndef INCLUDED_REDISCONNECTION_H
#define INCLUDED_REDISCONNECTION_H

#include <sw/redis++/redis++.h>
#include <sw/redis++/errors.h>

namespace clairvoyantmeta{

template<typename T>
class RedisConnection{

public:
    RedisConnection();

    RedisConnection(const int poolSize, const std::string& host, const int port, const int timeout);
    RedisConnection(RedisConnection<T>& conn); //copy constructor

    std::shared_ptr<T> getConnection();

private:
    std::shared_ptr<T> m_conn_p;
    sw::redis::ConnectionOptions m_options;
    sw::redis::ConnectionPoolOptions m_poolOptions;

};
}

#endif
