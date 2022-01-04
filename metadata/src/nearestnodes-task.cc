#include "nearestnodes-task.h"

#include "redis-messages.h"

#include <glog/logging.h>
#include <thread>
#include <chrono>
#include <time.h>
#include <stdlib.h>
#include "unagi_stopwatch.h"
namespace clairvoyantmeta{
namespace{
std::string jsonify_map(const std::unordered_map<std::string, std::string> & map) {
    std::stringstream ss;
    ss << "{";
    const std::string quote="\"";
    for(const auto & kv : map) {
        ss <<  quote << kv.first << quote << ":"
           <<  quote << kv.second << quote << ",";
    }
    ss << "}";
    return ss.str();
}

const int NUM_KEYS(1000);
}

NearestNodesTask::NearestNodesTask(const std::vector<Position>& points, const int pos_idx,  const std::shared_ptr<NearestNeighborInfo>& nearestNeighborInfo_p, const std::shared_ptr<sw::redis::RedisCluster>& conn_p):
    m_points(points),
    m_nearestNeighborInfo_p(nearestNeighborInfo_p),
    m_conn_p(conn_p),
    m_idx(pos_idx)
{
}
    
void NearestNodesTask::execute(){
    const double radius = 30.0; //metres
    const int maxItems = 1;
    unagi::Stopwatch sw;
    sw.start();
    LOG(INFO) << "Batch starts at : " << m_idx << " num points = " << m_points.size() << " this thread id is " << std::this_thread::get_id();
   for(int i = 0; i < m_points.size(); ++i){
        const std::string key = "nodes" + std::to_string(rand() % NUM_KEYS + 1); 
        double lat = m_points[i].latitude();
        double lon = m_points[i].longitude();
        std::vector<LocationInfoResult> results = RedisMessages<sw::redis::RedisCluster>::georadius(key,
                                                                                              lon,
                                                                                              lat,
                                                                                              radius,
                                                                                              maxItems,
                                                                                              m_conn_p);
        NodeInfo info;
        Position *node_p = info.mutable_node_pos();
        info.set_nodeid("");
        info.set_user_pos_idx(m_idx + i);


        if(results.size() > 0){
            //LOG(INFO) << "Number of nodes in Response: " << results.size(); //should be 1 as per current maxItems value.

            node_p->set_latitude(results[0].info.lat);
            node_p->set_longitude(results[0].info.lon);
            info.set_distance(results[0].distance);

            info.set_nodeid(results[0].info.value);

            std::unordered_map<std::string, std::string> nodeProps = RedisMessages<sw::redis::RedisCluster>::hgetall(info.nodeid(), m_conn_p);
            LOG(INFO) << "Node ID: " << results[0].info.value
                      << " | Props: " << jsonify_map(nodeProps);


            if(nodeProps.find("address") != nodeProps.end()){
                info.set_address(nodeProps["address"]);
            }
           // else{
           //     //LOG(ERROR) << "Node Address not found";
           //     
           // }
        }
        m_nearestNeighborInfo_p->addNode(info);
//        break;
    //else{
    //    //LOG(INFO) << "No nearby nodes for pt";
    //}
    }
    sw.stop();
    LOG(INFO) << "querying for " << m_points.size() << " points took " << sw.getElapsedTime() << " ms";    
    if(m_nearestNeighborInfo_p->isComplete()){
//        LOG(INFO) << "Deliver response";
        m_nearestNeighborInfo_p->deliverResponse();
    }
}


}
