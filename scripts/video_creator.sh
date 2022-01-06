#!/bin/bash
data_uploader_path=$1
num_videos=$2
filename=$3

for  ((i=1;i<=$num_videos;i++)); 
do
     echo "video id: " $i
        $data_uploader_path $filename v$i
      done
