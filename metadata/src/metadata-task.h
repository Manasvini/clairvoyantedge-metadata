#ifndef INCLUDED_METADATA_TASK_H
#define INCLUDED_METADATA_TASK_H

#include "unagi_task.h"

#include "clairvoyant_meta.grpc.pb.h"
#include "redis-connection.h"
namespace clairvoyantmeta{
class MetadataTask: public unagi::Task{
public:
    MetadataTask(grpc::ServerAsyncResponseWriter<Response>* responder,
                 const Request* request,
                 Response& response, 
                 const std::shared_ptr<sw::redis::RedisCluster>& conn,
                 void* tag);
    virtual void execute();

    
private:
    grpc::Status getVideoInfo(
                           const GetVideoInfoRequest* request,
                           Response* response);

    grpc::Status getNearestNodes( 
                           const GetNearestNodesRequest* request,
                           Response* response);
    
    grpc::Status updateSegmentInfo(
                           const UpdateSegmentInfoRequest* request,
                           Response* response);
    
    grpc::Status addVideoInfo( 
                           const AddVideoInfoRequest* request,
                           Response* response);
    
    grpc::Status addRoute( 
                           const AddRouteRequest* request,
                           Response* response);

    grpc::Status deleteRoute( 
                           const DeleteRouteRequest* request,
                           Response* response);

    grpc::Status getRoute( 
                           const GetRouteRequest* request,
                           Response* response);


    const Request* m_request_p;
    grpc::ServerAsyncResponseWriter<Response>* m_responder_p; 
    std::shared_ptr<sw::redis::RedisCluster> m_conn_p;
    void *m_tag;
    Response m_response;
};
}
#endif
