#!/bin/bash

docker run -d -p 50000:5000 --restart=always --name registry registry:latest&
docker run -p 9090:9090 -v /home/mercurio/git/challenge/prometheus:/etc/prometheus --restart=always --name=prometheus prom/prometheus --web.enable-admin-api --config.file /etc/prometheus/prometheus.yml&
docker run -d -p 4000:3000 --restart=always --name grafana grafana/grafana
docker run -d -p 9091:9091 --restart=always --name pushgateway prom/pushgateway
