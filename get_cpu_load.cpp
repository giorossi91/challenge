#include <sys/sysinfo.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>

#include <memory>
#include <future>
#include <cstdio>
#include <restbed>
#include <sstream>

#include "resource_server.h"

using namespace restbed;
using namespace std;

double get_cpu_load(void) {
	static double f_load = 1.f / (1 << SI_LOAD_SHIFT);

	struct sysinfo data;
	int            status = 0;

	status = sysinfo(&data);
	if(status == 0) {
		return data.loads[0] * f_load /** 100.0/get_nprocs()*/;
	} else {
		return -1.0;
	}
}

class CPULoad : ResourceManager {
public:
    CPULoad(string uri, string url, uint16_t port) : ResourceManager(uri, url, port) {}
    ~CPULoad() {}

    std::string get_data() {
		double load = get_cpu_load();
		stringstream ss_len;

		stringstream ss;
		ss << load;

        return ss.str();
    }
};

int main(int argc, char** argv) {
	if(argc != 2) {
		return EXIT_FAILURE;
	}


    CPULoad service(argv[1], "/cpu", 10000);
    service.start_resource_service();

    service.announce_service();

    while(1);
	
	return EXIT_SUCCESS;
}