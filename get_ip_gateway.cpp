#include <cstring>
#include <ios>
#include <sstream>
#include <iostream>
#include <unistd.h>

#include <memory>
#include <future>
#include <cstdio>
#include <restbed>
#include <sstream>

#include "resource_server.h"

using namespace restbed;
using namespace std;

string from_Hex_to_IP(std::string ipHex) {
	if(ipHex.length() != 8) {
		return "0.0.0.0";
	}

	uint32_t n_ip = 0U;

	std::stringstream sin, sout;
	sin << std::hex << ipHex;
	sin >> n_ip;

	uint16_t n_1 = 0, n_2 = 0, n_3 = 0, n_4 = 0;

	n_1 = (n_ip & 0x000000FF);
	n_2 = (n_ip & 0x0000FF00) >> 8;
	n_3 = (n_ip & 0x00FF0000) >> 16;
	n_4 = (n_ip & 0xFF000000) >> 24;

	sout << n_1 << "." << n_2 << "." << n_3 << "." << n_4;
	return sout.str();
}


string get_IP_gateway(void) {
	string ip = "0.0.0.0";

	FILE *f = NULL;
	char line[100] = { 0 };
	char *p = NULL;
	char *c = NULL;

	f = fopen("/proc/net/route" , "r");
	if(f == NULL) {
		return "0.0.0.0";
	}

	while(fgets(line , 100 , f)) {
		p = strtok(line , " \t");
		c = strtok(NULL , " \t");

		if(p != NULL && c != NULL) {
			if(strcmp(c , "00000000") == 0) {
				c = strtok(NULL, " \t");
				if(c != NULL) {
				  ip = from_Hex_to_IP(c);
				}
				break;
			}
		}
	}
	return ip;
}

std::string get_data() {
    return get_IP_gateway();
}

class GatewayIP : public ResourceManager {
public:
    GatewayIP(string uri, string url, uint16_t port) : ResourceManager(uri, url, port, get_data) {}
    ~GatewayIP() {}
};

int main(int argc, char** argv) {
	if(argc != 2) {
		return EXIT_FAILURE;
	}

    GatewayIP service(argv[1], "/gateway", GTW_PORT);
    service.start_resource_service();
    service.announce_service();

    while(1) {
        pause();
    }
	
	return EXIT_SUCCESS;
}