#!/bin/bash

operations=("getInfo" "getCurrentKernelVersion" "getNumberOfPartitions" "sshdRunning")

while read -r host
do
    rand=$((RANDOM % 4))
    operation=${operations[rand]}
    echo "Request operation $operation at $host"
    echo -n $operation | nc $host 9696
done < hosts.txt