#ifndef INCLUDED_METADATASERVERIMPL_H
#define INCLUDED_METADATASERVERIMPL_H
#include <grpc/grpc.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <grpcpp/security/server_credentials.h>

#include "clairvoyantedge.grpc.pb.h"

#include "redis-messages.h"
#include "redis-connection.h"
#include <sw/redis++/redis++.h>
#include <memory>

namespace clairvoyantedge {
class MetadataServerImpl final: public MetadataServer::Service {
public:
    MetadataServerImpl():
        m_connWrapper(1, "127.0.0.1", 7000, 60000){
        m_conn_p = m_connWrapper.getConnection();
    }

    grpc::Status getVideoInfo(grpc::ServerContext* context,
                           const GetVideoInfoRequest* request,
                           GetVideoInfoResponse* response) override;

    grpc::Status getNearestNodes(grpc::ServerContext* context, 
                           const GetNearestNodesRequest* request,
                           GetNearestNodesResponse* response) override;
    
    grpc::Status updateSegmentInfo(grpc::ServerContext* context,
                           const UpdateSegmentInfoRequest* request,
                           StatusResponse* response) override;
    
    grpc::Status addVideoInfo(grpc::ServerContext* context, 
                           const AddVideoInfoRequest* request,
                           StatusResponse* response) override;
    
    grpc::Status addNodeToRoute(grpc::ServerContext* context, 
                           const AddNodeToRouteRequest* request,
                           StatusResponse* response) override;

private:
    RedisConnection<sw::redis::RedisCluster> m_connWrapper;
    std::shared_ptr<sw::redis::RedisCluster> m_conn_p;

};

}
#endif
