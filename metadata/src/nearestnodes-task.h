#ifndef INCLUDED_NEAREST_NODES_TASK_H
#define INCLUDED_NEAREST_NODES_TASK_H

#include "unagi_task.h"
#include "nearest-neighbor-info.h"
#include "clairvoyant_meta.grpc.pb.h"
#include "redis-connection.h"
#include <vector>

namespace clairvoyantmeta{
class NearestNodesTask: public unagi::Task{
public:
    NearestNodesTask(const std::vector<Position>& points, const int pos_idx, const std::shared_ptr<NearestNeighborInfo>& nearestNeighborInfo, const std::shared_ptr<sw::redis::RedisCluster>& conn_p);
    virtual void execute();

private:
    std::shared_ptr<NearestNeighborInfo> m_nearestNeighborInfo_p;
    std::shared_ptr<sw::redis::RedisCluster> m_conn_p;
    std::vector<Position> m_points;
    int m_idx;
};
}
#endif
