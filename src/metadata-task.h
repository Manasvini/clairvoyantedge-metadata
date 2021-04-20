#ifndef INCLUDED_METADATA_TASK_H
#define INCLUDED_METADATA_TASK_H

#include "unagi_task.h"

#include "clairvoyantedge.grpc.pb.h"
#include "redis-connection.h"
namespace clairvoyantedge{
class MetadataTask: public unagi::Task{
public:
    MetadataTask(grpc::ServerAsyncResponseWriter<Response>* responder,
                 const Request* request,
                 Response* response,
                 const std::shared_ptr<sw::redis::RedisCluster>& conn,
                 void* tag);
    virtual void execute();

    
private:
    grpc::Status getVideoInfo(
                           const GetVideoInfoRequest* request,
                           Response* response);

    grpc::Status getNearestNodes( 
                           const GetNearestNodesRequest* request,
                           GetNearestNodesResponse* response);
    
    grpc::Status updateSegmentInfo(
                           const UpdateSegmentInfoRequest* request,
                           Response* response);
    
    grpc::Status addVideoInfo( 
                           const AddVideoInfoRequest* request,
                           Response* response);
    
    grpc::Status addNodeToRoute( 
                           const AddNodeToRouteRequest* request,
                           StatusResponse* response);

    const Request* m_request_p;
    Response* m_response_p;
    grpc::ServerAsyncResponseWriter<Response>* m_responder_p; 
    std::shared_ptr<sw::redis::RedisCluster> m_conn_p;
    void *m_tag;
};
}
#endif
