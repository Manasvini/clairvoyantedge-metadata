#!/bin/bash
pkill -SIGINT clairvoyant
kill `cat /tmp/edge_content_svr.pid`
./scripts/cleanup_redis.sh
sleep 1
#rm -rf logs/*
