#ifndef INCLUDED_METADATA_CONFIG_HELPER_H
#define INCLUDED_METADATA_CONFIG_HELPER_H
#include <string>

namespace clairvoyantmeta{
struct RedisConf{
    int maxConnections;
    int timeout;
    std::string serverAddr;
    int port;
};
struct MetadataConfig{
    int maxThreads;
    int maxQueue;
    RedisConf redisConf;
    int queryBatchSize;
};

MetadataConfig getMetadataConfig(const std::string& filename);
}

#endif
