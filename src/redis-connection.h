#ifndef INCLUDED_REDISCONNECTION_H
#define INCLUDED_REDISCONNECTION_H

#include <sw/redis++/redis++.h>

namespace clairvoyantedge{

template<typename T>
class RedisConnection{

public:
    RedisConnection();

    RedisConnection(const int poolSize, const std::string& host, const int port, const int timeout);

    std::shared_ptr<T> getConnection();

private:
    std::shared_ptr<T> m_conn_p;

};
}

#endif
