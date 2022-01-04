#include "gtest/gtest.h"
#include "redis-messages.h"
#include "redis-connection.h"

using ::testing::Test;
namespace clairvoyantmeta{
class MetadataTesterCluster : public ::testing::Test{
public:
    MetadataTesterCluster():
        m_connWrapper(1, "127.0.0.1", 7000, 60000){
        m_conn_p = m_connWrapper.getConnection();
//        m_conn_p->flushall();
    }
protected:
    RedisConnection<sw::redis::RedisCluster> m_connWrapper;
    std::shared_ptr<sw::redis::RedisCluster> m_conn_p;

};

//TEST_F(MetadataTesterCluster, PingTest){
//    std::string resp = "PONG";
//    EXPECT_EQ(resp.compare(m_conn_p->ping()), 0 );
//}

TEST_F(MetadataTesterCluster, SetGetTest){
    RedisMessages<sw::redis::RedisCluster>::set("test", "hello", m_conn_p);
    std::string val = "hello";
    EXPECT_EQ(RedisMessages<sw::redis::RedisCluster>::get("test", m_conn_p), val);
}

TEST_F(MetadataTesterCluster, InvalidGetTest){
    EXPECT_EQ(RedisMessages<sw::redis::RedisCluster>::get("test1", m_conn_p), "");
    EXPECT_FALSE(RedisMessages<sw::redis::RedisCluster>::exists("test1", m_conn_p));
}

TEST_F(MetadataTesterCluster, DelTest){
    RedisMessages<sw::redis::RedisCluster>::set("test", "hello", m_conn_p);
    EXPECT_TRUE(RedisMessages<sw::redis::RedisCluster>::exists("test", m_conn_p));
    RedisMessages<sw::redis::RedisCluster>::del("test", m_conn_p);
    EXPECT_FALSE(RedisMessages<sw::redis::RedisCluster>::exists("test", m_conn_p));
}

TEST_F(MetadataTesterCluster, InvalidDelTest){
    EXPECT_FALSE(RedisMessages<sw::redis::RedisCluster>::exists("test1", m_conn_p));
    RedisMessages<sw::redis::RedisCluster>::del("test1", m_conn_p);
    EXPECT_FALSE(RedisMessages<sw::redis::RedisCluster>::exists("test1", m_conn_p));
}


TEST_F(MetadataTesterCluster, HashSetGetTest){
    std::unordered_map<std::string, std::string> hmap;
    hmap["one"] = "1";
    hmap["two"] = "2";
    std::string key = "testhmap";
    RedisMessages<sw::redis::RedisCluster>::hmset(key, hmap,  m_conn_p);
    EXPECT_EQ(RedisMessages<sw::redis::RedisCluster>::hgetall(key, m_conn_p).size(), 2);
}

TEST_F(MetadataTesterCluster, HashUpdateTest){
    std::unordered_map<std::string, std::string> hmap;
    hmap["one"] = "1";
    hmap["two"] = "2";
    std::string key = "testhmap";
    RedisMessages<sw::redis::RedisCluster>::hmset(key, hmap,  m_conn_p);
    EXPECT_EQ(RedisMessages<sw::redis::RedisCluster>::hgetall(key, m_conn_p).size(), 2);
    hmap["three"] = "3";
    RedisMessages<sw::redis::RedisCluster>::hmset(key, hmap,  m_conn_p);
    EXPECT_EQ(RedisMessages<sw::redis::RedisCluster>::hgetall(key, m_conn_p).size(), 3);

}

TEST_F(MetadataTesterCluster, InvalidHashGetTest){
    std::string key = "testhmap1";
    EXPECT_EQ(RedisMessages<sw::redis::RedisCluster>::hgetall(key, m_conn_p).size(), 0);
    EXPECT_FALSE(RedisMessages<sw::redis::RedisCluster>::exists(key, m_conn_p));
}

TEST_F(MetadataTesterCluster, HashDelTest){
    std::string key = "testhmap";
    std::unordered_map<std::string, std::string> hmap;
    hmap["one"] = "1";
    hmap["two"] = "2";
    RedisMessages<sw::redis::RedisCluster>::hmset(key, hmap,  m_conn_p);
    EXPECT_TRUE(RedisMessages<sw::redis::RedisCluster>::exists(key, m_conn_p));
    RedisMessages<sw::redis::RedisCluster>::del(key, m_conn_p);
    EXPECT_FALSE(RedisMessages<sw::redis::RedisCluster>::exists(key, m_conn_p));
}

TEST_F(MetadataTesterCluster, InvalidHashDelTest){
    std::string key = "testhmap1";
    EXPECT_FALSE(RedisMessages<sw::redis::RedisCluster>::exists(key, m_conn_p));
    RedisMessages<sw::redis::RedisCluster>::del(key, m_conn_p);
    EXPECT_FALSE(RedisMessages<sw::redis::RedisCluster>::exists(key, m_conn_p));
}

TEST_F(MetadataTesterCluster, SetAddMembersTest){
    std::string key = "testset";
    std::unordered_set<std::string> members;
    members.insert("one");
    members.insert("two");
    RedisMessages<sw::redis::RedisCluster>::sadd(key, members, m_conn_p);
    EXPECT_EQ(RedisMessages<sw::redis::RedisCluster>::smembers(key, m_conn_p).size(), 2);
    
}

TEST_F(MetadataTesterCluster, InvalidSetTest){
    std::string key = "testset1";
    EXPECT_EQ(RedisMessages<sw::redis::RedisCluster>::smembers(key, m_conn_p).size(), 0);
    EXPECT_FALSE(RedisMessages<sw::redis::RedisCluster>::exists(key, m_conn_p));
}

TEST_F(MetadataTesterCluster, SetRemoveMembersTest){
    std::string key = "testset";
    std::unordered_set<std::string> members;
    members.insert("one");
    members.insert("two");
    RedisMessages<sw::redis::RedisCluster>::sadd(key, members, m_conn_p);
    EXPECT_EQ(RedisMessages<sw::redis::RedisCluster>::smembers(key, m_conn_p).size(), 2);
    std::vector<std::string> itemsToErase;
    itemsToErase.push_back("one");
    RedisMessages<sw::redis::RedisCluster>::srem(key, itemsToErase, m_conn_p);
    std::unordered_set<std::string> res = RedisMessages<sw::redis::RedisCluster>::smembers(key, m_conn_p);
    EXPECT_EQ(res.size(), 1);
    EXPECT_EQ(*res.begin(), "two");

}

TEST_F(MetadataTesterCluster, SetRemoveInvalidMembersTest){
    std::string key = "testset";
    std::unordered_set<std::string> members;
    members.insert("one");
    members.insert("two");
    RedisMessages<sw::redis::RedisCluster>::sadd(key, members, m_conn_p);
    EXPECT_EQ(RedisMessages<sw::redis::RedisCluster>::smembers(key, m_conn_p).size(), 2);
    std::vector<std::string> itemsToErase;
    itemsToErase.push_back("on1e");
    RedisMessages<sw::redis::RedisCluster>::srem(key, itemsToErase, m_conn_p);
    std::unordered_set<std::string> res = RedisMessages<sw::redis::RedisCluster>::smembers(key, m_conn_p);
    EXPECT_EQ(res.size(), 2);
    EXPECT_TRUE(RedisMessages<sw::redis::RedisCluster>::exists(key, m_conn_p));
}

TEST_F(MetadataTesterCluster, SetRemoveAllMembersTest){
    std::string key = "testset";
    std::unordered_set<std::string> members;
    members.insert("one");
    members.insert("two");
    RedisMessages<sw::redis::RedisCluster>::sadd(key, members, m_conn_p);
    EXPECT_EQ(RedisMessages<sw::redis::RedisCluster>::smembers(key, m_conn_p).size(), 2);
    std::vector<std::string> itemsToErase;
    itemsToErase.push_back("one");
    itemsToErase.push_back("two");
    RedisMessages<sw::redis::RedisCluster>::srem(key, itemsToErase, m_conn_p);
    std::unordered_set<std::string> res = RedisMessages<sw::redis::RedisCluster>::smembers(key, m_conn_p);
    EXPECT_EQ(res.size(), 0);
    EXPECT_FALSE(RedisMessages<sw::redis::RedisCluster>::exists(key, m_conn_p));

}

TEST_F(MetadataTesterCluster, SetRemoveInvalidKeyTest){
    std::string key = "testset1";
    std::vector<std::string> itemsToErase;
    itemsToErase.push_back("on1e");
    RedisMessages<sw::redis::RedisCluster>::srem(key, itemsToErase, m_conn_p);
    std::unordered_set<std::string> res = RedisMessages<sw::redis::RedisCluster>::smembers(key, m_conn_p);
    EXPECT_EQ(res.size(), 0);
    EXPECT_FALSE(RedisMessages<sw::redis::RedisCluster>::exists(key, m_conn_p));
}

TEST_F(MetadataTesterCluster, SetDelTest){
    std::string key = "testset";
    std::unordered_set<std::string> members;
    members.insert("one");
    members.insert("two");
    RedisMessages<sw::redis::RedisCluster>::sadd(key, members, m_conn_p);
    EXPECT_TRUE(RedisMessages<sw::redis::RedisCluster>::exists(key, m_conn_p));
    RedisMessages<sw::redis::RedisCluster>::del(key, m_conn_p);
    EXPECT_FALSE(RedisMessages<sw::redis::RedisCluster>::exists(key, m_conn_p));
}

TEST_F(MetadataTesterCluster, SetDelInvalidTest){
    std::string key = "testset1";
    EXPECT_FALSE(RedisMessages<sw::redis::RedisCluster>::exists(key, m_conn_p));
    RedisMessages<sw::redis::RedisCluster>::del(key, m_conn_p);
    EXPECT_FALSE(RedisMessages<sw::redis::RedisCluster>::exists(key, m_conn_p));
}

TEST_F(MetadataTesterCluster, GeoAddGeoRadiusTest){
    std::string key = "testgeo";
    std::vector<LocationInfo> infos;
    LocationInfo info;
    info.lon = 0;
    info.lat = 0;
    info.value = "testval";
    infos.push_back(info);
    RedisMessages<sw::redis::RedisCluster>::geoadd(key, infos, m_conn_p);
    EXPECT_EQ(RedisMessages<sw::redis::RedisCluster>::georadius(key, 0, //lon
                                                              0, //lat
                                                              5, // radius (m)
                                                              1, // items
                                                              m_conn_p).size(), 1);


}

TEST_F(MetadataTesterCluster, GeoAddGeoRadiusNoPointsTest){
    std::string key = "testgeo";
    std::vector<LocationInfo> infos;
    LocationInfo info;
    info.lon = 0;
    info.lat = 0;
    info.value = "testval";
    infos.push_back(info);
    RedisMessages<sw::redis::RedisCluster>::geoadd(key, infos, m_conn_p);
    EXPECT_EQ(RedisMessages<sw::redis::RedisCluster>::georadius(key, 1, //lon
                                                              0, //lat
                                                              5, // radius (m)
                                                              1, // items
                                                              m_conn_p).size(), 0);

    EXPECT_TRUE(RedisMessages<sw::redis::RedisCluster>::exists(key, m_conn_p));


}

TEST_F(MetadataTesterCluster, GeoAddGeoRadiusInvalidKeyTest){
    std::string key = "testgeo1";
    EXPECT_EQ(RedisMessages<sw::redis::RedisCluster>::georadius(key, 1, //lon
                                                              0, //lat
                                                              5, // radius (m)
                                                              1, // items
                                                              m_conn_p).size(), 0);


}

TEST_F(MetadataTesterCluster, GeoAddDelKeyTest){
    std::string key = "testgeo";
    std::vector<LocationInfo> infos;
    LocationInfo info;
    info.lon = 0;
    info.lat = 0;
    info.value = "testval";
    infos.push_back(info);
    RedisMessages<sw::redis::RedisCluster>::geoadd(key, infos, m_conn_p);
    EXPECT_EQ(RedisMessages<sw::redis::RedisCluster>::georadius(key, 0, //lon
                                                              0, //lat
                                                              5, // radius (m)
                                                              1, // items
                                                              m_conn_p).size(), 1);
    RedisMessages<sw::redis::RedisCluster>::del(key, m_conn_p);
    EXPECT_FALSE(RedisMessages<sw::redis::RedisCluster>::exists(key, m_conn_p));

}

TEST_F(MetadataTesterCluster, GeoDelinvalidKeyTest){
    std::string key = "testgeo1";
    EXPECT_FALSE(RedisMessages<sw::redis::RedisCluster>::exists(key, m_conn_p));
    RedisMessages<sw::redis::RedisCluster>::del(key, m_conn_p);
    EXPECT_FALSE(RedisMessages<sw::redis::RedisCluster>::exists(key, m_conn_p));

}

TEST_F(MetadataTesterCluster, ListAddTest){
    std::string key = "testlist";
    std::vector<std::string> vals;
    vals.push_back("one");
    vals.push_back("one");
    RedisMessages<sw::redis::RedisCluster>::lpush(key, vals, m_conn_p);
    EXPECT_EQ(RedisMessages<sw::redis::RedisCluster>::lrange(key, m_conn_p).size(), 2);
}

TEST_F(MetadataTesterCluster, ListDelTest){
    std::string key = "testlist";
    std::vector<std::string> vals;
    vals.push_back("one");
    vals.push_back("one");
    RedisMessages<sw::redis::RedisCluster>::lpush(key, vals, m_conn_p);
    EXPECT_TRUE(RedisMessages<sw::redis::RedisCluster>::exists(key, m_conn_p));
    RedisMessages<sw::redis::RedisCluster>::del(key, m_conn_p);
    EXPECT_FALSE(RedisMessages<sw::redis::RedisCluster>::exists(key, m_conn_p));
}

}
