OPT:=-Wall -Wextra -O0 -g2
INC_PATH:=-Irestbed/inc
LIB_PATH:=-Lrestbed/lib
LIBS:=-lrestbed
BUILD_PATH:=build
IP:=172.31.44.121

all: env
	g++ $(OPT) $(INC_PATH) $(LIB_PATH) -o $(BUILD_PATH)/get_cpu_load get_cpu_load.cpp $(LIBS)
	g++ $(OPT) $(INC_PATH) $(LIB_PATH) -o $(BUILD_PATH)/get_host_latency get_host_latency.cpp $(LIBS)
	g++ $(OPT) $(INC_PATH) $(LIB_PATH) -o $(BUILD_PATH)/get_ip_gateway get_ip_gateway.cpp $(LIBS)
	g++ $(OPT) $(INC_PATH) $(LIB_PATH) -o $(BUILD_PATH)/get_ram_usage get_ram_usage.cpp $(LIBS)
	g++ $(OPT) $(INC_PATH) $(LIB_PATH) -o $(BUILD_PATH)/http_post_server http_post_server.cpp $(LIBS)

run_cpu:
	LD_LIBRARY_PATH=./restbed/lib ./build/get_cpu_load http://$(IP):10000/cpu
	
run_ram:
	LD_LIBRARY_PATH=./restbed/lib ./build/get_ram_usage http://$(IP):10000/ram
	
run_latency:
	LD_LIBRARY_PATH=./restbed/lib ./build/get_host_latency http://$(IP):10000/latency google.com
	
run_gateway:
	LD_LIBRARY_PATH=./restbed/lib ./build/get_ip_gateway http://$(IP):10000/gateway

run_server:
	LD_LIBRARY_PATH=./restbed/lib ./build/http_post_server

env:
	mkdir -p $(BUILD_PATH)/

clean: 
	rm -rf $(BUILD_PATH)/