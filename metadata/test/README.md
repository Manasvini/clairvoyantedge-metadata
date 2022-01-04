# Test instructions



```
  # Clean up
  bash <root>/scripts/cleanup_redis.sh

  # Start cluster
  mkdir cluster
  bash <root>/scripts/start_redis_cluster.sh 3

  # Start gRPC Server
  cd <root>/metadata/build/
  ./clairvoyantedge-metadata

  # Start test
  cd <root>/metadata/build/test
  ./metadata-tester
```
