#include "metadata-config-helper.h"
#include<yaml-cpp/yaml.h>

namespace clairvoyantmeta{
MetadataConfig getMetadataConfig(const std::string& filename){
    YAML::Node config = YAML::LoadFile(filename.c_str());
    MetadataConfig mcfg;
    if(config["maxThreads"]){
        mcfg.maxThreads = config["maxThreads"].as<int>();
    }
    if(config["maxQueue"]){
        mcfg.maxQueue = config["maxQueue"].as<int>();
    }
    if(config["queryBatchSize"]){
        mcfg.queryBatchSize = config["queryBatchSize"].as<int>();
    }
    if(config["redisConfig"]){
        RedisConf rconf;
        auto redisConfig = config["redisConfig"];
        if(redisConfig["maxConnections"]){
            rconf.maxConnections = redisConfig["maxConnections"].as<int>();
        }
        if(redisConfig["timeout"]){
            rconf.timeout = redisConfig["timeout"].as<int>();
        }
        if(redisConfig["serverAddr"]){
            rconf.serverAddr = redisConfig["serverAddr"].as<std::string>();
        }
        if(redisConfig["port"]){
            rconf.port = redisConfig["port"].as<int>();
        }
        mcfg.redisConf = rconf;
    }
    if(config["queryBatchSize"]){
        mcfg.queryBatchSize = config["queryBatchSize"].as<int>();
    }
    return mcfg;

}

}
