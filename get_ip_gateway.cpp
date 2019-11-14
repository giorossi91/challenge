#include <cstring>
#include <ios>
#include <sstream>
#include <iostream>

#include <memory>
#include <future>
#include <cstdio>
#include <restbed>
#include <sstream>

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

int main(int argc, char** argv) {
	if(argc != 2) {
		return EXIT_FAILURE;
	}
	
	while(1) {
		string ip = get_IP_gateway();
		
		cout << "[IP gateway] " << ip << endl;
		
		auto request = make_shared< Request >( Uri( argv[1] ) );
		request->set_header("Accept", "*/*" );
		request->set_header("Host", "localhost");
		request->set_header("Content-Type", "application/x-www-form-urlencoded");
		request->set_header("Content-Length", ss.str());
		request->set_method("POST");
		request->set_body(ip);

		auto future = Http::async( request, [ ]( const shared_ptr< Request >, const shared_ptr< Response > response ) {
			fprintf( stderr, "Printing async response\n" );
		});

		future.wait( );
		
		sleep(1);
	}
	
	return EXIT_SUCCESS;
}

