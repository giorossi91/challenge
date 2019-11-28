#!/bin/bash

sudo apt-get install docker.io

sudo echo "{ \"insecure-registries\":[\"172.31.44.121:50000\"]}" > /etc/docker/daemon.json
sudo systemctl restart docker.service
sudo usermod -aG docker $USER

echo "Fai logout e rientra!"

