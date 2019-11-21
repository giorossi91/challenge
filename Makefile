OPT:=-Wall -Wextra -O0 -g2
INC_PATH:=-Irestbed/inc
LIB_PATH:=-Lrestbed/lib
LIBS:=-lrestbed -lpthread
BUILD_PATH:=build
IP:=172.31.44.121
GTW:=http://$(IP):20000

all: env
	g++ -c $(OPT) $(INC_PATH) -o $(BUILD_PATH)/resource_server.o resource_server.cpp
	
	g++ -c $(OPT) $(INC_PATH) -o $(BUILD_PATH)/get_cpu_load.o get_cpu_load.cpp
	g++ $(OPT) $(INC_PATH) $(LIB_PATH) -o $(BUILD_PATH)/get_cpu_load $(BUILD_PATH)/get_cpu_load.o $(BUILD_PATH)/resource_server.o $(LIBS)
	
	g++ -c $(OPT) $(INC_PATH) -o $(BUILD_PATH)/get_host_latency.o get_host_latency.cpp
	g++ $(OPT) $(INC_PATH) $(LIB_PATH) -o $(BUILD_PATH)/get_host_latency $(BUILD_PATH)/get_host_latency.o $(BUILD_PATH)/resource_server.o $(LIBS)

	g++ -c $(OPT) $(INC_PATH) -o $(BUILD_PATH)/get_ip_gateway.o get_ip_gateway.cpp
	g++ $(OPT) $(INC_PATH) $(LIB_PATH) -o $(BUILD_PATH)/get_ip_gateway $(BUILD_PATH)/get_ip_gateway.o $(BUILD_PATH)/resource_server.o $(LIBS)

	g++ -c $(OPT) $(INC_PATH) -o $(BUILD_PATH)/get_ram_usage.o get_ram_usage.cpp
	g++ $(OPT) $(INC_PATH) $(LIB_PATH) -o $(BUILD_PATH)/get_ram_usage $(BUILD_PATH)/get_ram_usage.o $(BUILD_PATH)/resource_server.o $(LIBS)

	g++ $(OPT) $(INC_PATH) $(LIB_PATH) -o $(BUILD_PATH)/http_post_server http_post_server.cpp $(LIBS)
	
	g++ $(OPT) $(INC_PATH) $(LIB_PATH) -o $(BUILD_PATH)/http_gateway_server http_gateway_server.cpp $(LIBS)

run_cpu:
	LD_LIBRARY_PATH=./restbed/lib ./build/get_cpu_load $(GTW)
	
run_ram:
	LD_LIBRARY_PATH=./restbed/lib ./build/get_ram_usage $(GTW)
	
run_latency:
	LD_LIBRARY_PATH=./restbed/lib ./build/get_host_latency $(GTW) $(IP)
	
run_gateway:
	LD_LIBRARY_PATH=./restbed/lib ./build/get_ip_gateway $(GTW)

run_test_server:
	LD_LIBRARY_PATH=./restbed/lib ./build/http_post_server

run_gateway_server:
	LD_LIBRARY_PATH=./restbed/lib ./build/http_gateway_server

test_announce:
	curl -d "/cpu;10000" -X POST $(GTW)/announce
	curl -d "/latency;10000" -X POST $(GTW)/announce
	curl -d "/ram;10000" -X POST $(GTW)/announce
	curl -d "/gateway;10000" -X POST $(GTW)/announce


env:
	mkdir -p $(BUILD_PATH)/

clean: 
	rm -rf $(BUILD_PATH)/