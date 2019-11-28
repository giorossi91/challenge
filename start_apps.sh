#!/bin/bash
IP=172.31.44.121

docker run --rm -p 10002:10002 --name cpu_service $IP:50000/setchallenge/cpu_service&
docker run --rm -p 10001:10001 --name ram_service $IP:50000/setchallenge/ram_service&
docker run --rm -p 10003:10003 --name gateway_service $IP:50000/setchallenge/gateway_service&
docker run --rm -p 10000:10000 --name latency_service $IP:50000/setchallenge/latency_service&
docker run --rm -p 20000:20000 --name server_gateway $IP:50000/setchallenge/server_gateway&