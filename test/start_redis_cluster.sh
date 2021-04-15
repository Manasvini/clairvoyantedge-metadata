#!/bin/bash

NUM_NODES=$1
NUM_NODES_TOTAL=$((NUM_NODES * 2))
start_port=7000

#mkdir cluster
cd cluster
workdir=$PWD
echo $workdir
echo "starting redis cluster with total $NUM_NODES_TOTAL nodes"

address="192.168.123.50"
charcolon=":"
addresses=""
for ((i=0;i<NUM_NODES_TOTAL;i++));
do
    mkdir node_$i
    cd node_$i
    port_num=$((i+start_port))
    echo "starting node on $port_num"
    echo "port $port_num" > redis.conf
    echo "bind  0.0.0.0" >> redis.conf
    echo "cluster-enabled yes" >> redis.conf
    echo "cluster-config-file node.conf" >> redis.conf
    echo "cluster-node-timeout 5000" >> redis.conf
    echo "appendonly yes" >> redis.conf
    redis-server redis.conf --daemonize yes 
    cd $workdir
    addresses="$addresses $address$charcolon$port_num "
done

echo "addresses: $addresses"

redis-cli --cluster create $addresses --cluster-replicas 1 
 
    
