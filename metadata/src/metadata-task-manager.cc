#include "metadata-task-manager.h"
#include "metadata-task.h"
#include "nearestnodes-task.h"
#include "nearest-neighbor-info.h"
#include "metadata-config-helper.h"

#include <glog/logging.h>
#include "unagi_task.h"
#include <chrono>
#include <thread>

namespace clairvoyantmeta{
MetadataTaskManager::MetadataTaskManager(const MetadataConfig& cfg):
    m_config(cfg),
    
//unagi::ThreadPool& threadPool, const std::shared_ptr<sw::redis::RedisCluster>& conn_p):
    m_threadPool(cfg.maxThreads, cfg.maxQueue),
    m_connWrapper(cfg.redisConf.maxConnections, cfg.redisConf.serverAddr, cfg.redisConf.port, cfg.redisConf.timeout)
    //m_conn_p(conn_p)
{
    m_conn_p = m_connWrapper.getConnection();
    if(!m_conn_p){
        LOG(INFO)<< "OOps";
    }
    m_threadPool.start();
}

void MetadataTaskManager::handleTask(grpc::ServerAsyncResponseWriter<Response>* responder_p, const Request* request_p, Response& response,  void* tag){
    if(!request_p || !responder_p){
        LOG(INFO) << "\n NULL";
    }
    else if(request_p->has_getnearestnodesrequest()){
        LOG(INFO) << "\nget nn req";
        processGetNearestNodes(responder_p, request_p, response, tag);
    }
    else{
        std::shared_ptr<unagi::Task> task_p = std::make_shared<MetadataTask>(responder_p, request_p, response, m_conn_p, tag);
//        task_p->execute();
        m_threadPool.enqueueJob(task_p);
    }
}

void MetadataTaskManager::processGetNearestNodes(grpc::ServerAsyncResponseWriter<Response>* responder_p, const Request* request_p, Response& response, void* tag){
    std::shared_ptr<NearestNeighborInfo> info_p = std::make_shared<NearestNeighborInfo>(request_p->getnearestnodesrequest().positions_size(), responder_p, request_p, response, tag);
    int batchSize = m_config.queryBatchSize;
    int j = 0;
    long numPts = request_p->getnearestnodesrequest().positions_size();
    long numBatches = numPts / batchSize + ( numPts % batchSize > 0 ? 1: 0);
    long start = 0;
    LOG(INFO) << "numbatches = " << numBatches << " total pts = " << numPts;
    for(long i = 0; i < numBatches; ++i){
        std::vector<Position> pts;
        for(long j = i * batchSize; j < std::min(numPts, (i+1)* batchSize); ++j){
            pts.push_back(request_p->getnearestnodesrequest().positions(j));
        } 
        LOG(INFO) << "batch " << i ;     
        std::shared_ptr<unagi::Task> task_p = std::make_shared<NearestNodesTask>(pts, i*batchSize, info_p, m_conn_p);
        m_threadPool.enqueueJob(task_p);
//        task_p->execute();
    }
    LOG(INFO) << "Added all tasks in " << numBatches << " batches";
}


}
