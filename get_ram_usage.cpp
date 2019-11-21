#include <sys/sysinfo.h>
#include <iostream>
#include <cstdlib>
#include <unistd.h>


#include <memory>
#include <future>
#include <cstdio>
#include <restbed>
#include <sstream>

#include "resource_server.h"


using namespace restbed;
using namespace std;

double get_ram_usage(void) {
	struct sysinfo data;
	int            status = 0;

	status = sysinfo(&data);
	if(status == 0) {
		return (1.0 - ((double)(data.freeram) / (double)(data.totalram))) * 100.0;
	} else {
		return -1.0;
	}
}

std::string get_data() {
    double load = get_ram_usage();
    stringstream ss_len;

    stringstream ss;
    ss << load;

    return ss.str();
}

class RAMLoad : public ResourceManager {
public:
    RAMLoad(string uri, string url, uint16_t port) : ResourceManager(uri, url, port, get_data) {}
    ~RAMLoad() {}
};

int main(int argc, char** argv) {
	if(argc != 2) {
		return EXIT_FAILURE;
	}


    RAMLoad service(argv[1], "/ram", RAM_PORT);
    service.start_resource_service();
    service.announce_service();

    while(1) {
        pause();
    }
	
	return EXIT_SUCCESS;
}