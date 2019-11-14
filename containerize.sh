#!/bin/bash

echo "Deploying CPU Service"
docker build -t setchallenge/cpu_service:v1 -f Dockerfile_cpu .
docker tag setchallenge/cpu_service:v1 localhost:50000/setchallenge/cpu_service
docker push localhost:50000/setchallenge/cpu_service
docker run --rm localhost:50000/setchallenge/cpu_service&
echo "==> Done!"

echo "Deploying RAM Service"
docker build -t setchallenge/ram_service:v1 -f Dockerfile_ram .
docker tag setchallenge/ram_service:v1 localhost:50000/setchallenge/ram_service
docker push localhost:50000/setchallenge/ram_service
docker run --rm localhost:50000/setchallenge/ram_service&
echo "==> Done!"

echo "Deploying IP Gateway Service"
docker build -t setchallenge/gateway_service:v1 -f Dockerfile_gateway .
docker tag setchallenge/gateway_service:v1 localhost:50000/setchallenge/gateway_service
docker push localhost:50000/setchallenge/gateway_service
docker run --rm localhost:50000/setchallenge/gateway_service&
echo "==> Done!"

echo "Deploying Latency Service"
docker build -t setchallenge/latency_service:v1 -f Dockerfile_latency .
docker tag setchallenge/latency_service:v1 localhost:50000/setchallenge/latency_service
docker push localhost:50000/setchallenge/latency_service
docker run --rm localhost:50000/setchallenge/latency_service&
echo "==> Done!"