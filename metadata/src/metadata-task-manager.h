#ifndef INCLUDED_METADATA_TASK_MANAGER_H
#define INCLUDED_METADATA_TASK_MANAGER_H

#include "clairvoyant_meta.grpc.pb.h"
#include "unagi_task.h"

#include "redis-connection.h"
#include "unagi_threadpool.h"
#include "metadata-config-helper.h"
namespace clairvoyantmeta{
class MetadataTaskManager {
public:
//    MetadataTaskManager(unagi::ThreadPool& threadPool, const std::shared_ptr<sw::redis::RedisCluster>& conn_p);
    MetadataTaskManager(const MetadataConfig& config);  
    void handleTask(grpc::ServerAsyncResponseWriter<Response>* responder, const Request* request, Response& response, void* tag);

    ~MetadataTaskManager(){
        m_threadPool.stop();
    }    
private:
    RedisConnection<sw::redis::RedisCluster> m_connWrapper;
    std::shared_ptr<sw::redis::RedisCluster> m_conn_p;
    unagi::ThreadPool m_threadPool;
    void processGetNearestNodes(grpc::ServerAsyncResponseWriter<Response>* responder_p, const Request* request_p, Response& response, void* tag);
    MetadataConfig m_config;
};

}
#endif
