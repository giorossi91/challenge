OPT:=-Wall -Wextra -O0 -g2
INC_PATH:=
LIB_PATH:=
LIBS:=-lrestbed
BUILD_PATH:=build

all: env
	g++ $(OPT) $(INC_PATH) $(LIB_PATH) -o $(BUILD_PATH)/get_cpu_load     $(LIBS) get_cpu_load.cpp
	# g++ $(OPT) $(INC_PATH) $(LIB_PATH) -o $(BUILD_PATH)/get_host_latency $(LIBS) get_host_latency.cpp 
	# g++ $(OPT) $(INC_PATH) $(LIB_PATH) -o $(BUILD_PATH)/get_ip_gateway   $(LIBS) get_ip_gateway.cpp
	# g++ $(OPT) $(INC_PATH) $(LIB_PATH) -o $(BUILD_PATH)/get_ram_usage    $(LIBS) get_ram_usage.cpp

env:
	mkdir -p $(BUILD_PATH)/

clean: 
	rm -rf $(BUILD_PATH)/
