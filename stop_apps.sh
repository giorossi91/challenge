#!/bin/bash
IP=172.31.44.121

docker stop cpu_service
docker stop ram_service
docker stop gateway_service
docker stop latency_service
#docker stop server_gateway
