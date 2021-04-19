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
        std::shared_ptr<grpc::Channel> channel_p = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
        m_stub_p = MetadataServer::NewStub(channel_p);
        m_conn_p = m_connWrapper.getConnection();
    
    }
protected:
    std::unique_ptr<MetadataServer::Stub> m_stub_p;
    std::shared_ptr<sw::redis::RedisCluster> m_conn_p;
    RedisConnection<sw::redis::RedisCluster> m_connWrapper;
};

TEST_F(MetadataTestClient, AddVideoTest){
    AddVideoInfoRequest request;
    Video* video = request.add_videos();
    video->set_videoid("v1");
    Segment* segment = video->add_segments();
    segment->set_segmentid("1");
    segment->set_popularity(1.0);
    segment->set_lastaccessed(1);
    segment->set_size(1);
    segment->add_nodeids("node1");
    StatusResponse response;
    grpc::ClientContext context;
    grpc::Status status = m_stub_p->addVideoInfo(&context, request, &response);
    EXPECT_TRUE(status.ok());
    EXPECT_TRUE(RedisMessages<sw::redis::RedisCluster>::exists("v1", m_conn_p));
    EXPECT_TRUE(RedisMessages<sw::redis::RedisCluster>::exists("1", m_conn_p));
}

TEST_F(MetadataTestClient, GetVideoTest){
    GetVideoInfoRequest request;
    request.set_videoid("v1");
    GetVideoInfoResponse response;
    grpc::ClientContext context;
    grpc::Status status = m_stub_p->getVideoInfo(&context, request, &response);
    EXPECT_TRUE(status.ok());
    EXPECT_EQ(response.video().videoid(), "v1");
    EXPECT_TRUE(response.video().segments_size() > 0); 
}

TEST_F(MetadataTestClient, SegmentUpdateTest){
    UpdateSegmentInfoRequest request;
    SegmentOperation* segmentOp = request.add_segmentoperations();
    segmentOp->set_operation("ADD");
    Segment* segment = segmentOp->mutable_segment();
    segment->set_segmentid("11");
    segment->set_popularity(1.0);
    segment->set_lastaccessed(1);
    segment->set_size(1);
    segment->add_nodeids("node1");
    StatusResponse response;
    grpc::ClientContext context;
    grpc::Status status = m_stub_p->updateSegmentInfo(&context, request, &response);
    EXPECT_TRUE(status.ok());
    EXPECT_TRUE(RedisMessages<sw::redis::RedisCluster>::exists("11", m_conn_p));
}
TEST_F(MetadataTestClient, SegmentDelTest){
    UpdateSegmentInfoRequest request;
    SegmentOperation* segmentOp = request.add_segmentoperations();
    segmentOp->set_operation("DEL");
    Segment* segment = segmentOp->mutable_segment();
    segment->set_segmentid("11");
    segment->set_popularity(1.0);
    segment->set_lastaccessed(1);
    segment->set_size(1);
    segment->add_nodeids("node1");
    StatusResponse response;
    grpc::ClientContext context;
    grpc::Status status = m_stub_p->updateSegmentInfo(&context, request, &response);
    EXPECT_TRUE(status.ok());
    EXPECT_FALSE(RedisMessages<sw::redis::RedisCluster>::exists("11", m_conn_p));
}
}
