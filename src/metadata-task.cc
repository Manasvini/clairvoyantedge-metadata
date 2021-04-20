#include "metadata-task.h"
#include "redis-messages.h"
#include "redis-connection.h"
#include <sw/redis++/redis++.h>

namespace clairvoyantedge{
MetadataTask::MetadataTask(grpc::ServerAsyncResponseWriter<Response>* responder, 
                          const Request* request, 
                          Response* response,
                          const std::shared_ptr<sw::redis::RedisCluster>& conn, void* tag ):
    m_request_p(request),
    m_response_p(response),
    m_responder_p(responder),
    m_conn_p(conn),
    m_tag(tag){
}

void MetadataTask::execute(){
    std::cout << "running task";
    if(!m_request_p || !m_responder_p){
        std::cout << "\nboom";
    }
    if(m_request_p->has_getvideoinforequest()){
        std::cout<<"\n got get video task";
        getVideoInfo(&m_request_p->getvideoinforequest(), m_response_p);
    }
    else if(m_request_p->has_addvideoinforequest()){
        std::cout << "\n got add video req\n";
        addVideoInfo(&(m_request_p->addvideoinforequest()), m_response_p);
    }
    else if(m_request_p->has_updatesegmentinforequest()){
        std::cout <<"\n got update segment req";
        updateSegmentInfo(&m_request_p->updatesegmentinforequest(), m_response_p);
    }
    if(!m_response_p){
        std::cout << "boom2";
    }
    m_responder_p->Finish(*m_response_p, grpc::Status::OK, m_tag);
    std::cout << "done\n";

}

grpc::Status MetadataTask::getVideoInfo( 
                                              const GetVideoInfoRequest* request,
                                              Response* response){
    std::vector<std::string> segmentIds = RedisMessages<sw::redis::RedisCluster>::lrange(request->videoid(), m_conn_p);
    Video *video = response->mutable_getvideoinforesponse()->mutable_video();
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
    ///grpc::ServerAsyncResponseWriter<GetVideoInfoResponse> responder(context);
    return grpc::Status::OK;
}
grpc::Status MetadataTask::getNearestNodes(
                                              const GetNearestNodesRequest* request,
                                              GetNearestNodesResponse* response){
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
            RedisMessages<sw::redis::RedisCluster>::hmset(request->videos(i).segments(j).segmentid(), segmentInfoMap, m_conn_p);
        }
        RedisMessages<sw::redis::RedisCluster>::lpush(request->videos(i).videoid(), segmentIds, m_conn_p);
    } 
    StatusResponse* status = response->mutable_statusresponse();
    status->add_statuses("OK");
    return grpc::Status::OK;
}

grpc::Status MetadataTask::addNodeToRoute( 
                                              const AddNodeToRouteRequest* request,
                                              StatusResponse* response){
    return grpc::Status::OK;
}

}