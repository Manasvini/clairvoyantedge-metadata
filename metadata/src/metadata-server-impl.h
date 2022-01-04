#ifndef INCLUDED_METADATASERVERIMPL_H
#define INCLUDED_METADATASERVERIMPL_H
#include <grpc/grpc.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <grpcpp/security/server_credentials.h>

#include "clairvoyant_meta.grpc.pb.h"

#include "redis-messages.h"
#include "redis-connection.h"

#include "unagi_threadpool.h"
#include "unagi_task.h"

#include <sw/redis++/redis++.h>

#include <memory>

#include "metadata-task.h"
#include "metadata-task-manager.h"
#include "metadata-config-helper.h"
namespace clairvoyantmeta {
class MetadataServerImpl{
// final: public MetadataServer::Service {
public:
    MetadataServerImpl(const std::string& serverAddress, const MetadataConfig& config):
 //       m_connWrapper(5, "127.0.0.1", 7000, 60000),
//        m_threadpool(5, 10000),
//        m_conn_p(m_connWrapper.getConnection()),
        m_manager(config)
{
        //m_conn_p = m_connWrapper.getConnection();
  //      m_threadpool.start();
        
        grpc::ServerBuilder builder;
        builder.AddListeningPort(serverAddress, grpc::InsecureServerCredentials());
        builder.RegisterService(&m_service);
        m_reqQueue_p = builder.AddCompletionQueue();
        m_server_p = builder.BuildAndStart();
        std::cout << "Server running on " << serverAddress;
        handleRequests();

    }
    void handleRequests();

    ~MetadataServerImpl(){
        //m_threadpool.stop();
        m_server_p->Shutdown();
        m_reqQueue_p->Shutdown();
    }
//    void deliverResponse();

private:
 //   RedisConnection<sw::redis::RedisCluster> m_connWrapper;
//    std::shared_ptr<sw::redis::RedisCluster> m_conn_p;
 //   unagi::ThreadPool m_threadpool;
    MetadataServer::AsyncService m_service;
    std::unique_ptr<grpc::ServerCompletionQueue> m_reqQueue_p;
    std::unique_ptr<grpc::Server> m_server_p;
    MetadataTaskManager m_manager;
    class CallData {
    public:
        CallData(MetadataServer::AsyncService* service, 
                 grpc::ServerCompletionQueue* cq, 
     //            const std::shared_ptr<sw::redis::RedisCluster>& conn_p,
                 MetadataTaskManager& manager)
            : m_service_p(service),
              m_cq_p(cq), 
              m_responder(&m_context), 
              status_(CREATE), 
    //          m_conn_p(conn_p),
              m_manager(manager) {
              proceed();
        }

        void proceed() {
            if (status_ == CREATE) {
                std::cout <<"created\n";
                status_ = PROCESS;

                m_service_p->RequesthandleRequest(&m_context, &m_request, &m_responder, m_cq_p, m_cq_p,
                                  this);
            } 
            else if (status_ == PROCESS) {
                new CallData(m_service_p, m_cq_p, m_manager);
                //std::shared_ptr<unagi::Task> task_p = std::make_shared<MetadataTask>(&m_responder, &m_request, m_conn_p, this);
                status_ = FINISH;
                //task_p->execute();
                //m_threadpool.enqueueJob(task_p);
                Response response;
                m_manager.handleTask(&m_responder, &m_request, response, this);
                //m_responder.Finish(m_response, grpc::Status::OK, this);
            }
            else {
                GPR_ASSERT(status_ == FINISH);
                delete this;
            }
        }

   private:
        MetadataServer::AsyncService* m_service_p;
        grpc::ServerCompletionQueue* m_cq_p;
        grpc::ServerContext m_context;
        MetadataTaskManager& m_manager;
//        std::shared_ptr<sw::redis::RedisCluster> m_conn_p;
        Request m_request;
        //unagi::ThreadPool& m_threadpool;
        grpc::ServerAsyncResponseWriter<Response> m_responder;

        enum CallStatus { CREATE, PROCESS, FINISH };
        CallStatus status_;  // The current serving state.
  };
};


}
#endif
