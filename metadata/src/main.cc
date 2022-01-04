#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <iostream>
#include <cstdlib>
#include <signal.h>
#include <unistd.h>

#include "metadata-config-helper.h"
#include "metadata-server-impl.h"
#include <glog/logging.h>
#include <gflags/gflags.h>
DEFINE_string(config, "", "Config file for metadata manager");

void shutdownHook(int s){
    google::ShutdownGoogleLogging();
    exit(1);
}
int main(int argc, char* argv[]){
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = shutdownHook;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    //TODO: change it to config based input
    gflags::ParseCommandLineFlags(&argc, &argv, true); 

    FLAGS_log_dir="./logs";
    google::InitGoogleLogging(argv[0]);
    
    LOG(INFO) <<  "Starting cloud meta server";
    clairvoyantmeta::MetadataConfig config = clairvoyantmeta::getMetadataConfig(FLAGS_config);
    std::string address("0.0.0.0:50051");
    clairvoyantmeta::MetadataServerImpl service(address, config);

    //grpc::EnableDefaultHealthCheckService(true);
    //grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    //grpc::ServerBuilder builder;
    //// Listen on the given address without any authentication mechanism.
    //builder.AddListeningPort(address, grpc::InsecureServerCredentials());
    //// Register "service" as the instance through which we'll communicate with
    //// clients. In this case it corresponds to an *synchronous* service.
    //builder.RegisterService(&service);
    //// Finally assemble the server.
    //std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    //std::cout << "Server listening on " << address << std::endl;    
    //server->Wait();
    service.handleRequests();
    return 0;
}

