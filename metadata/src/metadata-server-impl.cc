#include "metadata-server-impl.h"
#include "metadata-task.h"
#include "unagi_task.h"
#include <grpc/support/log.h>
namespace clairvoyantmeta{

void MetadataServerImpl::handleRequests(){

//    std::shared_ptr<unagi::Task> task_p = std::make_shared<MetadataTask>(&responder, &request, &response, m_conn_p, tag );
//    m_service.RequesthandleRequest(&ctx, &request, &responder, m_reqQueue_p.get(), m_reqQueue_p.get(), tag);
    new CallData(&m_service, m_reqQueue_p.get(), m_manager);
    void* tag;
    bool ok;
    int count = 0;
    while(true){
        
        //GPR_ASSERT(ok);
        std::cout << "before handle req "<< " ctr = " << count << "\n";
        GPR_ASSERT(m_reqQueue_p->Next(&tag, &ok)); 
        GPR_ASSERT(ok);
        //std::shared_ptr<unagi::Task> task_p = std::make_shared<MetadataTask>(&responder, &request, &response, m_conn_p, task_p.get() );
            //tag = task_p.get();
            std::cout << "tag and ok\n";
            ++count;
            //m_service.RequesthandleRequest(&ctx, &request, &responder, m_reqQueue_p.get(), m_reqQueue_p.get(), tag);
            //task_p = std::make_shared<MetadataTask>(&responder, &request, &response, m_conn_p, tag );
            // task_p->execute();
            //ctr++;
            ////m_threadpool.enqueueJob(task_p);
            //std::cout << "finished req\n";
            //ok = false;
            //break;
            static_cast<CallData*>(tag)->proceed();
        //GPR_ASSERT(m_reqQueue_p->Next(&tag, &ok));
        //GPR_ASSERT(ok);
        

    }
}

}
