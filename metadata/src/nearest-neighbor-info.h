#ifndef INCLUDED_NEAREST_NEIGHBOR_INFO_H
#define INCLUDED_NEAREST_NEIGHBOR_INFO_H

#include "clairvoyant_meta.grpc.pb.h"
#include <mutex>
#include <atomic>

namespace clairvoyantmeta{
class NearestNeighborInfo{
public:
    NearestNeighborInfo(const int size, grpc::ServerAsyncResponseWriter<Response>* responder, const Request* request, Response& response, void* tag);

    void addNode(const NodeInfo& nodeInfo);
    bool isComplete();
    //std::vector<NodeInfo> getNodes();

    void deliverResponse(); 
private:
    int m_expectedPoints;
    std::vector<NodeInfo> m_nodes;
    grpc::ServerAsyncResponseWriter<Response>* m_responder_p;
    const Request* m_request_p;
    void* m_tag_p;
    Response m_response;
    std::mutex m_mutex;
    std::atomic<bool> m_delivered;
};
}
#endif
