#include "metadata-server-impl.h"
namespace clairvoyantedge{


grpc::Status MetadataServerImpl::getVideoInfo(grpc::ServerContext* context, 
                                              const GetVideoInfoRequest* request,
                                              GetVideoInfoResponse* response){
    std::vector<std::string> segmentIds = RedisMessages<sw::redis::RedisCluster>::lrange(request->videoid(), m_conn_p);
    Video *video = response->mutable_video();
    video->set_videoid(request->videoid());
    
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
        
    }
    return grpc::Status::OK;
}
grpc::Status MetadataServerImpl::getNearestNodes(grpc::ServerContext* context, 
                                              const GetNearestNodesRequest* request,
                                              GetNearestNodesResponse* response){
    return grpc::Status::OK;
}
grpc::Status MetadataServerImpl::updateSegmentInfo(grpc::ServerContext* context, 
                                              const UpdateSegmentInfoRequest* request,
                                              StatusResponse* response){
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
    return grpc::Status::OK;
}
grpc::Status MetadataServerImpl::addVideoInfo(grpc::ServerContext* context, 
                                              const AddVideoInfoRequest* request,
                                              StatusResponse* response){
    for(int i = 0; i < request->videos_size(); ++i){
        std::vector<std::string> segmentIds; 
        for(int j = 0; j < request->videos(i).segments_size(); ++j){
            segmentIds.push_back(request->videos(i).segments(j).segmentid());
            std::unordered_map<std::string, std::string> segmentInfoMap;
            segmentInfoMap["size"] = std::to_string(request->videos(i).segments(j).size());
            segmentInfoMap["popularity"] = std::to_string(request->videos(i).segments(j).popularity());
            segmentInfoMap["last_accessed"] = std::to_string(request->videos(i).segments(j).lastaccessed());
            RedisMessages<sw::redis::RedisCluster>::hmset(request->videos(i).segments(j).segmentid(), segmentInfoMap, m_conn_p);
        }
        RedisMessages<sw::redis::RedisCluster>::lpush(request->videos(i).videoid(), segmentIds, m_conn_p);
    } 
    return grpc::Status::OK;
}
grpc::Status MetadataServerImpl::addNodeToRoute(grpc::ServerContext* context, 
                                              const AddNodeToRouteRequest* request,
                                              StatusResponse* response){
    return grpc::Status::OK;
}
}
