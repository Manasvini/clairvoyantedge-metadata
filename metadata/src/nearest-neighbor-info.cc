#include "nearest-neighbor-info.h"
#include <glog/logging.h>

namespace clairvoyantmeta{

namespace {
    bool compareTo(const NodeInfo& a, const NodeInfo& b){
        return a.user_pos_idx() < b.user_pos_idx();
    }
}
NearestNeighborInfo::NearestNeighborInfo(const int size, grpc::ServerAsyncResponseWriter<Response>* responder, const Request* request, Response& response, void* tag):
    m_expectedPoints(size),
    m_request_p(request),
    m_responder_p(responder),
    m_response(response),
    m_tag_p(tag),
    m_delivered(false)
{}

bool NearestNeighborInfo::isComplete(){
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_expectedPoints == m_nodes.size() && !m_delivered;
}

//std::vector<NodeInfo> NearestNeighborInfo::getNodes(){
//    auto nodesList = m_nodes.getList();
//    std::vector<NodeInfo> nodes(nodesList.begin(), nodesList.end());
//    return nodes;
//}

void NearestNeighborInfo::addNode(const NodeInfo& node){
    std::lock_guard<std::mutex> lock(m_mutex);
    m_nodes.push_back(node);
}

void NearestNeighborInfo::deliverResponse(){
    std::lock_guard<std::mutex> lock(m_mutex);
    
    LOG(INFO) << "Have " << m_nodes.size() << " points in response";
    auto nnresp = m_response.mutable_getnearestnodesresponse();
    std::sort(m_nodes.begin(), m_nodes.end(), compareTo);
    for(auto node: m_nodes){
        auto node_p = nnresp->add_nodes();
        node_p->CopyFrom(node);
        LOG(INFO) << " id is " << node_p->nodeid() << " addr is " << node_p->address();
    }
    if(!m_tag_p){
        std::cout << "OOPS";
    }
    m_responder_p->Finish(m_response, grpc::Status::OK, m_tag_p);
    LOG(INFO) << "Done delivering response";
    m_delivered = true;
}
}
