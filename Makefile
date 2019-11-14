OPT:=-Wall -Wextra -O0 -g2
INC_PATH:=-I../restbed/source
LIB_PATH:=-L../restbed/build
LIBS:=-lrestbed
BUILD_PATH:=build

all: env
	g++ $(OPT) $(INC_PATH) $(LIB_PATH) -o $(BUILD_PATH)/get_cpu_load get_cpu_load.cpp $(LIBS)
	g++ $(OPT) $(INC_PATH) $(LIB_PATH) -o $(BUILD_PATH)/get_host_latency get_host_latency.cpp $(LIBS)
	g++ $(OPT) $(INC_PATH) $(LIB_PATH) -o $(BUILD_PATH)/get_ip_gateway get_ip_gateway.cpp $(LIBS)
	g++ $(OPT) $(INC_PATH) $(LIB_PATH) -o $(BUILD_PATH)/get_ram_usage get_ram_usage.cpp $(LIBS)

run_cpu:
	LD_LIBRARY_PATH=../restbed/build ./build/get_cpu_load http://localhost:10000/cpu
	
run_ram:
	LD_LIBRARY_PATH=../restbed/build ./build/get_ram_usage http://localhost:10000/ram
	
run_latency:
	LD_LIBRARY_PATH=../restbed/build ./build/get_host_latency http://localhost:10000/latency google.com
	
run_gateway:
	LD_LIBRARY_PATH=../restbed/build ./build/get_ip_gateway http://localhost:10000/gateway

env:
	mkdir -p $(BUILD_PATH)/

clean: 
	rm -rf $(BUILD_PATH)/