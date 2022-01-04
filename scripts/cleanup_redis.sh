#!/bin/bash

pkill -9 redis-server
rm -rf cluster/*
rm dump.rdb
