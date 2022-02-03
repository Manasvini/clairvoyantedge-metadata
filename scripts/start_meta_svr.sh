#!/bin/bash
rm -rf logs
mkdir -p logs
./build/metadata/src/clairvoyantedge-metadata --logbuflevel=-1 --config=metadata/conf/metadata_manager_cfg.yaml
