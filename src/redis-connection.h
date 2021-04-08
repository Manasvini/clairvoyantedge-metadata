#ifndef INCLUDED_REDISCONNECTION_H
#define INCLUDED_REDISCONNECTION_H

#include <sw/redis++/redis++.h>

namespace clairvoyantedge{
class RedisConnection{

public:
    RedisConnection();
    RedisConnection(const int poolSize, const std::string& host, const int port, const int timeout);

    std::shared_ptr<sw::redis::Redis> getConnection();

private:
    std::shared_ptr<sw::redis::Redis> m_conn_p;

};
}

#endif
