#!/bin/bash
IP=172.31.44.121

echo "Deploying CPU Service"
docker build -t setchallenge/cpu_service:v1 -f Dockerfile_cpu .
docker tag setchallenge/cpu_service:v1 $IP:50000/setchallenge/cpu_service
docker push $IP:50000/setchallenge/cpu_service
docker run --rm -p 10002:10002 --name cpu_service $IP:50000/setchallenge/cpu_service&
echo "==> Done!"

echo "Deploying RAM Service"
docker build -t setchallenge/ram_service:v1 -f Dockerfile_ram .
docker tag setchallenge/ram_service:v1 $IP:50000/setchallenge/ram_service
docker push $IP:50000/setchallenge/ram_service
docker run --rm -p 10001:10001 --name ram_service $IP:50000/setchallenge/ram_service&
echo "==> Done!"

echo "Deploying IP Gateway Service"
docker build -t setchallenge/gateway_service:v1 -f Dockerfile_gateway .
docker tag setchallenge/gateway_service:v1 $IP:50000/setchallenge/gateway_service
docker push $IP:50000/setchallenge/gateway_service
docker run --rm -p 10003:10003 --name gateway_service $IP:50000/setchallenge/gateway_service&
echo "==> Done!"

echo "Deploying Latency Service"
docker build -t setchallenge/latency_service:v1 -f Dockerfile_latency .
docker tag setchallenge/latency_service:v1 $IP:50000/setchallenge/latency_service
docker push $IP:50000/setchallenge/latency_service
docker run --rm -p 10000:10000 --name latency_service $IP:50000/setchallenge/latency_service&
echo "==> Done!"

echo "Deploying Gateway Server"
docker build -t setchallenge/server_gateway:v1 -f Dockerfile_servergtw .
docker tag setchallenge/server_gateway:v1 $IP:50000/setchallenge/server_gateway
docker push $IP:50000/setchallenge/server_gateway
docker run --rm -p 20000:20000 --name server_gateway $IP:50000/setchallenge/server_gateway&
echo "==> Done!"