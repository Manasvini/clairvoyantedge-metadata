#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>

#include "clairvoyant_meta.grpc.pb.h"
#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>


std::vector<clairvoyantmeta::Segment> getSegments(const std::string& filename, const std::string& videoid){
    std::vector<clairvoyantmeta::Segment> segments;

    std::ifstream file(filename);
    std::string line;
    std::getline(file, line);

    while(std::getline(file, line)){
        std::stringstream ss(line);
        std::string val;
        std::vector<std::string> vals;
        while(getline(ss, val, ',')){
            vals.push_back(val);
        }
        if(vals.size() == 2){
            //std::cout << vals[0] <<"," << vals[1] <<"\n";
            clairvoyantmeta::Segment segment;
            segment.set_segmentid(videoid + "_" + vals[0]);
            segment.set_popularity(0);
            segment.set_lastaccessed(0);
            segment.add_nodeids("1");
            segment.set_size((long)std::stol(vals[1]) );
            segments.push_back(segment);
        }
    }
    return segments;
}

int main(int argc, char* argv[]) {
    std::unique_ptr<clairvoyantmeta::MetadataServer::Stub> stub_p;
    std::shared_ptr<grpc::Channel> channel_p;
    //std::cout << "Usage: ./data_uploader filename videoid\n";
    channel_p = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
    stub_p = clairvoyantmeta::MetadataServer::NewStub(channel_p);
    
    std::vector<clairvoyantmeta::Segment> segments = getSegments(argv[1], argv[2]);
    clairvoyantmeta::Request request;
    clairvoyantmeta::AddVideoInfoRequest* addReq = request.mutable_addvideoinforequest();
    clairvoyantmeta::Video* video = addReq->add_videos();
    video->set_videoid(argv[2]);
    for(const auto& seg: segments){
        clairvoyantmeta::Segment* segment = video->add_segments();
        *segment = seg;
    }
    clairvoyantmeta::Response response;
    std::cout << "vid id = " << video->videoid() << "\n";
    grpc::ClientContext context;
    grpc::Status status;
    grpc::CompletionQueue cq;
    std::unique_ptr<grpc::ClientAsyncResponseReader<clairvoyantmeta::Response>> rpc(stub_p->PrepareAsynchandleRequest(&context, request, &cq));
    rpc->StartCall();
    rpc->Finish(&response, &status, (void*)1);
    void *got_tag;
    bool ok = false;
    GPR_ASSERT(cq.Next(&got_tag, &ok));
    GPR_ASSERT(got_tag == (void*)1);
    GPR_ASSERT(ok); 
    
}
