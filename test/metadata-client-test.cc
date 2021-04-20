#include "gtest/gtest.h"
#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

#include "clairvoyantedge.grpc.pb.h"
#include "redis-connection.h"
#include "redis-messages.h"
using ::testing::Test;

namespace clairvoyantedge{
class MetadataTestClient : public ::testing::Test{
public:
    MetadataTestClient():
        m_connWrapper(1, "127.0.0.1", 7000, 60000){
        m_channel_p = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
        m_stub_p = MetadataServer::NewStub(m_channel_p);
        m_conn_p = m_connWrapper.getConnection();
    
    }
protected:
    virtual void TearDown(){
//        m_channel_p->close();
    }
    std::unique_ptr<MetadataServer::Stub> m_stub_p;
    std::shared_ptr<grpc::Channel> m_channel_p;
    std::shared_ptr<sw::redis::RedisCluster> m_conn_p;
    RedisConnection<sw::redis::RedisCluster> m_connWrapper;
};

TEST_F(MetadataTestClient, AddVideoTest){
    Request request;

    AddVideoInfoRequest* addRequest = request.mutable_addvideoinforequest();
    Video* video = addRequest->add_videos();
    video->set_videoid("v1");
    Segment* segment = video->add_segments();
    segment->set_segmentid("1");
    segment->set_popularity(1.0);
    segment->set_lastaccessed(1);
    segment->set_size(1);
    segment->add_nodeids("node1");
    Response response;
    grpc::ClientContext context;
    grpc::Status status;
    grpc::CompletionQueue cq;
    EXPECT_TRUE(request.has_addvideoinforequest());
    std::unique_ptr<grpc::ClientAsyncResponseReader<Response>> rpc(m_stub_p->PrepareAsynchandleRequest(&context, request, &cq));
    rpc->StartCall();
    rpc->Finish(&response, &status, (void*)1);
    void *got_tag;
    bool ok = false;
    GPR_ASSERT(cq.Next(&got_tag, &ok));
    GPR_ASSERT(got_tag == (void*)1);
    GPR_ASSERT(ok); 
    
    EXPECT_TRUE(response.has_statusresponse());
    std::cout << "code is " << status.error_code() << " msg is " << status.error_message();
    EXPECT_TRUE(status.ok());
    EXPECT_TRUE(RedisMessages<sw::redis::RedisCluster>::exists("v1", m_conn_p));
    EXPECT_TRUE(RedisMessages<sw::redis::RedisCluster>::exists("1", m_conn_p));
}

TEST_F(MetadataTestClient, GetVideoTest){
    Request request;
    GetVideoInfoRequest* getRequest = request.mutable_getvideoinforequest();
    getRequest->set_videoid("v1");
  
    Response response;
    grpc::ClientContext context;
    grpc::Status status;
    grpc::CompletionQueue cq;
    std::unique_ptr<grpc::ClientAsyncResponseReader<Response>> rpc(m_stub_p->PrepareAsynchandleRequest(&context, request, &cq));
    rpc->StartCall();
    rpc->Finish(&response, &status, (void*)1);
    void *got_tag;
    bool ok = false;
    GPR_ASSERT(cq.Next(&got_tag, &ok));
    GPR_ASSERT(got_tag == (void*)1);
    GPR_ASSERT(ok); 
    
    EXPECT_TRUE(status.ok());
    EXPECT_EQ(response.getvideoinforesponse().video().videoid(), "v1");
    EXPECT_TRUE(response.getvideoinforesponse().video().segments_size() > 0); 
}

TEST_F(MetadataTestClient, SegmentUpdateTest){
    Request request;
    UpdateSegmentInfoRequest *updateRequest = request.mutable_updatesegmentinforequest();
    SegmentOperation* segmentOp = updateRequest->add_segmentoperations();
    segmentOp->set_operation("ADD");
    Segment* segment = segmentOp->mutable_segment();
    segment->set_segmentid("11");
    segment->set_popularity(1.0);
    segment->set_lastaccessed(1);
    segment->set_size(1);
    segment->add_nodeids("node1");
    
    Response response;
    grpc::ClientContext context;
    grpc::Status status;
    grpc::CompletionQueue cq;
    std::unique_ptr<grpc::ClientAsyncResponseReader<Response>> rpc(m_stub_p->PrepareAsynchandleRequest(&context, request, &cq));
    rpc->StartCall();
    rpc->Finish(&response, &status, (void*)1);
    void *got_tag;
    bool ok = false;
    GPR_ASSERT(cq.Next(&got_tag, &ok));
    GPR_ASSERT(got_tag == (void*)1);
    GPR_ASSERT(ok); 
    
    EXPECT_TRUE(status.ok());
    EXPECT_TRUE(RedisMessages<sw::redis::RedisCluster>::exists("11", m_conn_p));
}
TEST_F(MetadataTestClient, SegmentDelTest){
    Request request;
    UpdateSegmentInfoRequest* updateRequest = request.mutable_updatesegmentinforequest();
    SegmentOperation* segmentOp = updateRequest->add_segmentoperations();
    segmentOp->set_operation("DEL");
    Segment* segment = segmentOp->mutable_segment();
    segment->set_segmentid("11");
    segment->set_popularity(1.0);
    segment->set_lastaccessed(1);
    segment->set_size(1);
    segment->add_nodeids("node1");
    Response response;
    grpc::ClientContext context;
    grpc::Status status;
    grpc::CompletionQueue cq;
    std::unique_ptr<grpc::ClientAsyncResponseReader<Response>> rpc(m_stub_p->PrepareAsynchandleRequest(&context, request, &cq));
    rpc->StartCall();
    rpc->Finish(&response, &status, (void*)1);
    void *got_tag;
    bool ok = false;
    GPR_ASSERT(cq.Next(&got_tag, &ok));
    GPR_ASSERT(got_tag == (void*)1);
    GPR_ASSERT(ok); 
    
    EXPECT_TRUE(status.ok());
    EXPECT_FALSE(RedisMessages<sw::redis::RedisCluster>::exists("11", m_conn_p));
}
}
