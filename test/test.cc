#include "gtest/gtest.h"
#include "redis-messages.h"
#include "redis-connection.h"

using ::testing::Test;

class MetadataTester : public ::testing::Test{

};

TEST_F(MetadataTester, DummyTest){
    clairvoyantedge::RedisConnection<sw::redis::Redis> conn;
    std::string resp = "PONG";
    EXPECT_EQ(resp.compare(conn.getConnection()->ping()), 0 );
}
