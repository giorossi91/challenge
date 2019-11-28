#!/bin/bash

curl -X POST -g "http://localhost:9090/api/v1/admin/tsdb/delete_series?match[]=cpu"
curl -X POST -g "http://localhost:9090/api/v1/admin/tsdb/delete_series?match[]=latency"
curl -X POST -g "http://localhost:9090/api/v1/admin/tsdb/delete_series?match[]=ram"
curl -X POST -g "http://localhost:9090/api/v1/admin/tsdb/delete_series?match[]=gateway"
