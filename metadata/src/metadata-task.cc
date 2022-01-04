#include "metadata-task.h"
#include "redis-messages.h"
#include "redis-connection.h"
#include <sw/redis++/redis++.h>
#include <glog/logging.h>
#include <chrono>

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
}
MetadataTask::MetadataTask(grpc::ServerAsyncResponseWriter<Response>* responder, 
                          const Request* request, Response& response,
                          const std::shared_ptr<sw::redis::RedisCluster>& conn, void* tag ):
    m_request_p(request),
    m_response(response),
    m_responder_p(responder),
    m_conn_p(conn),
    m_tag(tag){
}

void MetadataTask::execute(){
    std::cout << "running task";
    if(!m_request_p || !m_responder_p){
        LOG(INFO) << "\n NULL";
    }
    if(m_request_p->has_getvideoinforequest()){
        LOG(INFO) <<"\n got get video task";
        getVideoInfo(&m_request_p->getvideoinforequest(), &m_response);
    }
    else if(m_request_p->has_addvideoinforequest()){
        LOG(INFO) << "\n got add video req\n";
        addVideoInfo(&(m_request_p->addvideoinforequest()), &m_response);
    }
    else if(m_request_p->has_updatesegmentinforequest()){
        LOG(INFO) <<"\n got update segment req";
        updateSegmentInfo(&m_request_p->updatesegmentinforequest(), &m_response);
    }
    else if(m_request_p->has_getnearestnodesrequest()){
        LOG(INFO) << "\nget nn req";
        getNearestNodes(&m_request_p->getnearestnodesrequest(), &m_response);
    }
    else if(m_request_p->has_addrouterequest()){
        LOG(INFO) << "got add route\n";
        addRoute(&m_request_p->addrouterequest(), &m_response);
        LOG(INFO) << "response has " << m_response.statusresponse().statuses_size()<<"\n";
    }
    else if(m_request_p->has_deleterouterequest()){
        deleteRoute(&m_request_p->deleterouterequest(), &m_response);
    }
    else if(m_request_p->has_getrouterequest()){
        getRoute(&m_request_p->getrouterequest(), &m_response);
    }
    
    m_responder_p->Finish(m_response, grpc::Status::OK, m_tag);

}

grpc::Status MetadataTask::getVideoInfo( 
                                              const GetVideoInfoRequest* request,
                                              Response* response){
    std::vector<std::string> segmentIds = RedisMessages<sw::redis::RedisCluster>::lrange(request->videoid(), m_conn_p);
    Video *video = response->mutable_getvideoinforesponse()->mutable_video();
    video->set_videoid(request->videoid());
    
    std::vector<Segment> updates;
    LOG(INFO) << "Size of segments received: " << segmentIds.size();
    for(const auto& segmentId : segmentIds){
        std::unordered_map<std::string, std::string> segmentProps = RedisMessages<sw::redis::RedisCluster>::hgetall(segmentId, m_conn_p);
        auto it = segmentProps.find("size");
        if(it == segmentProps.end()){
            continue;
        }
        int segmentSize = std::stoi(it->second);
        it = segmentProps.find("popularity");
        if(it ==segmentProps.end()){
            continue;
        }
        double popularity = std::stod(it->second);
        it = segmentProps.find("last_accessed");
        if(it == segmentProps.end()){
            continue;
        }
        long lastAccessed = std::stol(it->second);
        Segment* segment = video->add_segments();
        segment->set_segmentid(segmentId);
        segment->set_popularity(popularity);
        segment->set_size(segmentSize);
        segment->set_lastaccessed(lastAccessed);
 
        unsigned int now = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
         
        Segment toUpdate;
        toUpdate.CopyFrom(*segment);
        toUpdate.set_popularity(popularity + 1);
        toUpdate.set_lastaccessed(now);
        updates.push_back(toUpdate);
         
    }
    for(auto& segment: updates){
        UpdateSegmentInfoRequest req; 
        Response resp;  
        auto segmentOp = req.add_segmentoperations();
        auto segment_p = segmentOp->mutable_segment();

        segment_p->CopyFrom(segment);
        segmentOp->set_operation("UPDATE");
        updateSegmentInfo(&req, &resp);
    }
    ///grpc::ServerAsyncResponseWriter<GetVideoInfoResponse> responder(context);
    return grpc::Status::OK;
}


grpc::Status MetadataTask::getNearestNodes(
                                              const GetNearestNodesRequest* request,
                                              Response* response){
    //FIXME: hardcoded values, move it to macro definitions or config file for more control
    double radius = 30.0; //metres
    int maxItems = 1;

    const std::string key = "nodes"; //prefix for node keys in the metadatabase
    LOG(INFO) << "Number of pts in Request: " << request->positions_size();

    GetNearestNodesResponse *nnResp = response->mutable_getnearestnodesresponse();
    for(int i = 0; i < request->positions_size(); ++i){
        double lat = request->positions(i).latitude();
        double lon = request->positions(i).longitude();
        std::vector<LocationInfoResult> results = RedisMessages<sw::redis::RedisCluster>::georadius(key,
                                                                                                  lon,
                                                                                                  lat,
                                                                                                  radius,
                                                                                                  maxItems,
                                                                                                  m_conn_p);
        NodeInfo* info = nnResp->add_nodes();
        Position *node_p = info->mutable_node_pos();
        info->set_nodeid("");

        if(results.size() > 0){
            LOG(INFO) << "Number of nodes in Response: " << results.size(); //should be 1 as per current maxItems value.

            node_p->set_latitude(results[0].info.lat);
            node_p->set_longitude(results[0].info.lon);
            info->set_distance(results[0].distance);
            info->set_user_pos_idx(i);

            info->set_nodeid(results[0].info.value);

            std::unordered_map<std::string, std::string> nodeProps = RedisMessages<sw::redis::RedisCluster>::hgetall(info->nodeid(), m_conn_p);
            LOG(INFO) << "Node ID: " << results[0].info.value
                      << " | Props: " << jsonify_map(nodeProps);


            if(nodeProps.find("address") != nodeProps.end()){
                info->set_address(nodeProps["address"]);
            }
           // else{
           //     //LOG(ERROR) << "Node Address not found";
           //     
           // }
        }
        //else{
        //    //LOG(INFO) << "No nearby nodes for pt";
        //}
    }
    return grpc::Status::OK;
}


grpc::Status MetadataTask::updateSegmentInfo( 
                                              const UpdateSegmentInfoRequest* request,
                                              Response* response){
    for(int i = 0; i < request->segmentoperations_size(); ++i){
        if(request->segmentoperations(i).operation().compare("ADD") == 0 || request->segmentoperations(i).operation().compare("UPDATE") == 0){
            std::unordered_map<std::string, std::string> segmentInfoMap;
            segmentInfoMap["size"] = std::to_string(request->segmentoperations(i).segment().size());
            segmentInfoMap["popularity"] = std::to_string(request->segmentoperations(i).segment().popularity());
            segmentInfoMap["last_accessed"] = std::to_string(request->segmentoperations(i).segment().lastaccessed());
            RedisMessages<sw::redis::RedisCluster>::hmset(request->segmentoperations(i).segment().segmentid(), segmentInfoMap, m_conn_p);
        }
        else{
            RedisMessages<sw::redis::RedisCluster>::del(request->segmentoperations(i).segment().segmentid(), m_conn_p);
        }
    }
    StatusResponse* status = response->mutable_statusresponse();
    status->add_statuses("OK");
    return grpc::Status::OK;

}

grpc::Status MetadataTask::addVideoInfo( 
                                              const AddVideoInfoRequest* request,
                                              Response* response){
    for(int i = 0; i < request->videos_size(); ++i){
        std::vector<std::string> segmentIds; 
        for(int j = 0; j < request->videos(i).segments_size(); ++j){
            segmentIds.push_back(request->videos(i).segments(j).segmentid());
            std::unordered_map<std::string, std::string> segmentInfoMap;
            segmentInfoMap["size"] = std::to_string(request->videos(i).segments(j).size());
            segmentInfoMap["popularity"] = std::to_string(request->videos(i).segments(j).popularity());
            segmentInfoMap["last_accessed"] = std::to_string(request->videos(i).segments(j).lastaccessed());
             LOG(INFO) << "vid id " << request->videos(i).videoid() << " seg " << request->videos(i).segments(j).segmentid() <<"\n";
 
            RedisMessages<sw::redis::RedisCluster>::hmset(request->videos(i).segments(j).segmentid(), segmentInfoMap, m_conn_p);
        }
        RedisMessages<sw::redis::RedisCluster>::lpush(request->videos(i).videoid(), segmentIds, m_conn_p);
    } 
    StatusResponse* status = response->mutable_statusresponse();
    status->add_statuses("OK");
    return grpc::Status::OK;
}

grpc::Status MetadataTask::addRoute( 
                                              const AddRouteRequest* request,
                                              Response* response){
    RedisMessages<sw::redis::RedisCluster>::set(request->routeid(),request->status(),  m_conn_p);
    StatusResponse* status = response->mutable_statusresponse();
    status->add_statuses("OK");
    std::cout << "Status has " << response->statusresponse().statuses_size() <<"\n";
    return grpc::Status::OK;
}

grpc::Status MetadataTask::deleteRoute( 
                                              const DeleteRouteRequest* request,
                                              Response* response){
    RedisMessages<sw::redis::RedisCluster>::del(request->routeid(), m_conn_p);
    StatusResponse* status = response->mutable_statusresponse();
    status->add_statuses("OK");
    return grpc::Status::OK;
}

grpc::Status MetadataTask::getRoute( 
                                              const GetRouteRequest* request,
                                              Response* response){
    bool exists = RedisMessages<sw::redis::RedisCluster>::exists(request->routeid(), m_conn_p);
    StatusResponse* status = response->mutable_statusresponse();

    std::cout << "got get req " << request->routeid() << " exists = " << exists <<"\n";
    std::string statusStr = "NONE";
    if(exists){
        statusStr = RedisMessages<sw::redis::RedisCluster>::get(request->routeid(), m_conn_p);
    }
    status->add_statuses(statusStr);
    return grpc::Status::OK;
}


}
