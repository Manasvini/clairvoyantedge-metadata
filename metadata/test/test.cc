#include "gtest/gtest.h"
#include "redis-messages.h"
#include "redis-connection.h"

using ::testing::Test;
namespace clairvoyantmeta{
class MetadataTester : public ::testing::Test{
public:
    MetadataTester(){
        m_conn_p = m_connWrapper.getConnection();
        m_conn_p->flushall();
    }
protected:
    RedisConnection<sw::redis::Redis> m_connWrapper;
    std::shared_ptr<sw::redis::Redis> m_conn_p;

};

TEST_F(MetadataTester, PingTest){
    std::string resp = "PONG";
    EXPECT_EQ(resp.compare(m_conn_p->ping()), 0 );
}

TEST_F(MetadataTester, CreationTest){
    RedisConnection<sw::redis::Redis> connWrapper(1, "127.0.0.1", 6379, 60000);

}


TEST_F(MetadataTester, SetGetTest){
    RedisMessages<sw::redis::Redis>::set("test", "hello", m_conn_p);
    std::string val = "hello";
    EXPECT_EQ(RedisMessages<sw::redis::Redis>::get("test", m_conn_p), val);
}

TEST_F(MetadataTester, InvalidGetTest){
    EXPECT_EQ(RedisMessages<sw::redis::Redis>::get("test1", m_conn_p), "");
    EXPECT_FALSE(RedisMessages<sw::redis::Redis>::exists("test1", m_conn_p));
}

TEST_F(MetadataTester, DelTest){
    RedisMessages<sw::redis::Redis>::set("test", "hello", m_conn_p);
    EXPECT_TRUE(RedisMessages<sw::redis::Redis>::exists("test", m_conn_p));
    RedisMessages<sw::redis::Redis>::del("test", m_conn_p);
    EXPECT_FALSE(RedisMessages<sw::redis::Redis>::exists("test", m_conn_p));
}

TEST_F(MetadataTester, InvalidDelTest){
    EXPECT_FALSE(RedisMessages<sw::redis::Redis>::exists("test1", m_conn_p));
    RedisMessages<sw::redis::Redis>::del("test1", m_conn_p);
    EXPECT_FALSE(RedisMessages<sw::redis::Redis>::exists("test1", m_conn_p));
}


TEST_F(MetadataTester, HashSetGetTest){
    std::unordered_map<std::string, std::string> hmap;
    hmap["one"] = "1";
    hmap["two"] = "2";
    std::string key = "testhmap";
    RedisMessages<sw::redis::Redis>::hmset(key, hmap,  m_conn_p);
    EXPECT_EQ(RedisMessages<sw::redis::Redis>::hgetall(key, m_conn_p).size(), 2);
}

TEST_F(MetadataTester, HashUpdateTest){
    std::unordered_map<std::string, std::string> hmap;
    hmap["one"] = "1";
    hmap["two"] = "2";
    std::string key = "testhmap";
    RedisMessages<sw::redis::Redis>::hmset(key, hmap,  m_conn_p);
    EXPECT_EQ(RedisMessages<sw::redis::Redis>::hgetall(key, m_conn_p).size(), 2);
    hmap["three"] = "3";
    RedisMessages<sw::redis::Redis>::hmset(key, hmap,  m_conn_p);
    EXPECT_EQ(RedisMessages<sw::redis::Redis>::hgetall(key, m_conn_p).size(), 3);

}

TEST_F(MetadataTester, InvalidHashGetTest){
    std::string key = "testhmap1";
    EXPECT_EQ(RedisMessages<sw::redis::Redis>::hgetall(key, m_conn_p).size(), 0);
    EXPECT_FALSE(RedisMessages<sw::redis::Redis>::exists(key, m_conn_p));
}

TEST_F(MetadataTester, HashDelTest){
    std::string key = "testhmap";
    std::unordered_map<std::string, std::string> hmap;
    hmap["one"] = "1";
    hmap["two"] = "2";
    RedisMessages<sw::redis::Redis>::hmset(key, hmap,  m_conn_p);
    EXPECT_TRUE(RedisMessages<sw::redis::Redis>::exists(key, m_conn_p));
    RedisMessages<sw::redis::Redis>::del(key, m_conn_p);
    EXPECT_FALSE(RedisMessages<sw::redis::Redis>::exists(key, m_conn_p));
}

TEST_F(MetadataTester, InvalidHashDelTest){
    std::string key = "testhmap1";
    EXPECT_FALSE(RedisMessages<sw::redis::Redis>::exists(key, m_conn_p));
    RedisMessages<sw::redis::Redis>::del(key, m_conn_p);
    EXPECT_FALSE(RedisMessages<sw::redis::Redis>::exists(key, m_conn_p));
}

TEST_F(MetadataTester, SetAddMembersTest){
    std::string key = "testset";
    std::unordered_set<std::string> members;
    members.insert("one");
    members.insert("two");
    RedisMessages<sw::redis::Redis>::sadd(key, members, m_conn_p);
    EXPECT_EQ(RedisMessages<sw::redis::Redis>::smembers(key, m_conn_p).size(), 2);
    
}

TEST_F(MetadataTester, InvalidSetTest){
    std::string key = "testset1";
    EXPECT_EQ(RedisMessages<sw::redis::Redis>::smembers(key, m_conn_p).size(), 0);
    EXPECT_FALSE(RedisMessages<sw::redis::Redis>::exists(key, m_conn_p));
}

TEST_F(MetadataTester, SetRemoveMembersTest){
    std::string key = "testset";
    std::unordered_set<std::string> members;
    members.insert("one");
    members.insert("two");
    RedisMessages<sw::redis::Redis>::sadd(key, members, m_conn_p);
    EXPECT_EQ(RedisMessages<sw::redis::Redis>::smembers(key, m_conn_p).size(), 2);
    std::vector<std::string> itemsToErase;
    itemsToErase.push_back("one");
    RedisMessages<sw::redis::Redis>::srem(key, itemsToErase, m_conn_p);
    std::unordered_set<std::string> res = RedisMessages<sw::redis::Redis>::smembers(key, m_conn_p);
    EXPECT_EQ(res.size(), 1);
    EXPECT_EQ(*res.begin(), "two");

}

TEST_F(MetadataTester, SetRemoveInvalidMembersTest){
    std::string key = "testset";
    std::unordered_set<std::string> members;
    members.insert("one");
    members.insert("two");
    RedisMessages<sw::redis::Redis>::sadd(key, members, m_conn_p);
    EXPECT_EQ(RedisMessages<sw::redis::Redis>::smembers(key, m_conn_p).size(), 2);
    std::vector<std::string> itemsToErase;
    itemsToErase.push_back("on1e");
    RedisMessages<sw::redis::Redis>::srem(key, itemsToErase, m_conn_p);
    std::unordered_set<std::string> res = RedisMessages<sw::redis::Redis>::smembers(key, m_conn_p);
    EXPECT_EQ(res.size(), 2);
    EXPECT_TRUE(RedisMessages<sw::redis::Redis>::exists(key, m_conn_p));
}

TEST_F(MetadataTester, SetRemoveAllMembersTest){
    std::string key = "testset";
    std::unordered_set<std::string> members;
    members.insert("one");
    members.insert("two");
    RedisMessages<sw::redis::Redis>::sadd(key, members, m_conn_p);
    EXPECT_EQ(RedisMessages<sw::redis::Redis>::smembers(key, m_conn_p).size(), 2);
    std::vector<std::string> itemsToErase;
    itemsToErase.push_back("one");
    itemsToErase.push_back("two");
    RedisMessages<sw::redis::Redis>::srem(key, itemsToErase, m_conn_p);
    std::unordered_set<std::string> res = RedisMessages<sw::redis::Redis>::smembers(key, m_conn_p);
    EXPECT_EQ(res.size(), 0);
    EXPECT_FALSE(RedisMessages<sw::redis::Redis>::exists(key, m_conn_p));

}

TEST_F(MetadataTester, SetRemoveInvalidKeyTest){
    std::string key = "testset1";
    std::vector<std::string> itemsToErase;
    itemsToErase.push_back("on1e");
    RedisMessages<sw::redis::Redis>::srem(key, itemsToErase, m_conn_p);
    std::unordered_set<std::string> res = RedisMessages<sw::redis::Redis>::smembers(key, m_conn_p);
    EXPECT_EQ(res.size(), 0);
    EXPECT_FALSE(RedisMessages<sw::redis::Redis>::exists(key, m_conn_p));
}

TEST_F(MetadataTester, SetDelTest){
    std::string key = "testset";
    std::unordered_set<std::string> members;
    members.insert("one");
    members.insert("two");
    RedisMessages<sw::redis::Redis>::sadd(key, members, m_conn_p);
    EXPECT_TRUE(RedisMessages<sw::redis::Redis>::exists(key, m_conn_p));
    RedisMessages<sw::redis::Redis>::del(key, m_conn_p);
    EXPECT_FALSE(RedisMessages<sw::redis::Redis>::exists(key, m_conn_p));
}

TEST_F(MetadataTester, SetDelInvalidTest){
    std::string key = "testset1";
    EXPECT_FALSE(RedisMessages<sw::redis::Redis>::exists(key, m_conn_p));
    RedisMessages<sw::redis::Redis>::del(key, m_conn_p);
    EXPECT_FALSE(RedisMessages<sw::redis::Redis>::exists(key, m_conn_p));
}

TEST_F(MetadataTester, GeoAddGeoRadiusTest){
    std::string key = "testgeo";
    std::vector<LocationInfo> infos;
    LocationInfo info;
    info.lon = 0;
    info.lat = 0;
    info.value = "testval";
    infos.push_back(info);
    RedisMessages<sw::redis::Redis>::geoadd(key, infos, m_conn_p);
    EXPECT_EQ(RedisMessages<sw::redis::Redis>::georadius(key, 0, //lon
                                                              0, //lat
                                                              5, // radius (m)
                                                              1, // items
                                                              m_conn_p).size(), 1);


}

TEST_F(MetadataTester, GeoAddGeoRadiusNoPointsTest){
    std::string key = "testgeo";
    std::vector<LocationInfo> infos;
    LocationInfo info;
    info.lon = 0;
    info.lat = 0;
    info.value = "testval";
    infos.push_back(info);
    RedisMessages<sw::redis::Redis>::geoadd(key, infos, m_conn_p);
    EXPECT_EQ(RedisMessages<sw::redis::Redis>::georadius(key, 1, //lon
                                                              0, //lat
                                                              5, // radius (m)
                                                              1, // items
                                                              m_conn_p).size(), 0);

    EXPECT_TRUE(RedisMessages<sw::redis::Redis>::exists(key, m_conn_p));


}

TEST_F(MetadataTester, GeoAddGeoRadiusInvalidKeyTest){
    std::string key = "testgeo1";
    EXPECT_EQ(RedisMessages<sw::redis::Redis>::georadius(key, 1, //lon
                                                              0, //lat
                                                              5, // radius (m)
                                                              1, // items
                                                              m_conn_p).size(), 0);


}

TEST_F(MetadataTester, GeoAddDelKeyTest){
    std::string key = "testgeo";
    std::vector<LocationInfo> infos;
    LocationInfo info;
    info.lon = 0;
    info.lat = 0;
    info.value = "testval";
    infos.push_back(info);
    RedisMessages<sw::redis::Redis>::geoadd(key, infos, m_conn_p);
    EXPECT_EQ(RedisMessages<sw::redis::Redis>::georadius(key, 0, //lon
                                                              0, //lat
                                                              5, // radius (m)
                                                              1, // items
                                                              m_conn_p).size(), 1);
    RedisMessages<sw::redis::Redis>::del(key, m_conn_p);
    EXPECT_FALSE(RedisMessages<sw::redis::Redis>::exists(key, m_conn_p));

}

TEST_F(MetadataTester, GeoDelinvalidKeyTest){
    std::string key = "testgeo1";
    EXPECT_FALSE(RedisMessages<sw::redis::Redis>::exists(key, m_conn_p));
    RedisMessages<sw::redis::Redis>::del(key, m_conn_p);
    EXPECT_FALSE(RedisMessages<sw::redis::Redis>::exists(key, m_conn_p));

}

TEST_F(MetadataTester, ListAddTest){
    std::string key = "testlist";
    std::vector<std::string> vals;
    vals.push_back("one");
    vals.push_back("one");
    RedisMessages<sw::redis::Redis>::lpush(key, vals, m_conn_p);
    EXPECT_EQ(RedisMessages<sw::redis::Redis>::lrange(key, m_conn_p).size(), 2);
}

TEST_F(MetadataTester, ListDelTest){
    std::string key = "testlist";
    std::vector<std::string> vals;
    vals.push_back("one");
    vals.push_back("one");
    RedisMessages<sw::redis::Redis>::lpush(key, vals, m_conn_p);
    EXPECT_TRUE(RedisMessages<sw::redis::Redis>::exists(key, m_conn_p));
    RedisMessages<sw::redis::Redis>::del(key, m_conn_p);
    EXPECT_FALSE(RedisMessages<sw::redis::Redis>::exists(key, m_conn_p));
}

}
