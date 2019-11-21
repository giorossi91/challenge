#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <unistd.h>

#include <thread>
#include <mutex>
#include <memory>
#include <future>
#include <restbed>
#include <sstream>

#include "resource_server.h"

using namespace std;
using namespace restbed;

char *hostname;

string get_stdout_from_command(string cmd) {
	string data;
	FILE * stream;
	const int max_buffer = 256;
	char buffer[max_buffer];
	cmd.append(" 2>&1");

	stream = popen(cmd.c_str(), "r");
	if (stream) {
		while (!feof(stream)) {
			if (fgets(buffer, max_buffer, stream) != NULL) {
				data.append(buffer);
			}
		}
		pclose(stream);
	}
	return data;
}

vector<string> split_string(string str, string delim) {
	vector<string> list;
	string token;
	char *data = new char[str.size() + 1];
	str.copy(data, str.size());  
  
  char *saveptr = NULL; 
  char *ptoken = strtok_r (data, delim.c_str(), &saveptr);
  
  while(ptoken != NULL) {
   	token.assign(ptoken);
	  list.push_back(token);
    ptoken = strtok_r (NULL, delim.c_str(), &saveptr);
  }
  
	delete[] data;
	return list;
}

string extract_rtt(string output) {
	string ret = "0";
	vector<string> lines = split_string(output, "\n");
	if(!lines.empty()) {
		vector<string> data = split_string(lines.back(), "/");
		if(data.size() > 4) {
			ret = data[4];
		}
	}
    
	return ret;
}

string get_latency (char *hostname) {
	string out = "0";
	out = get_stdout_from_command("ping -q -c 3 " + string(hostname));	
	out = extract_rtt(out);

	return out;
}

mutex latency_mtx;
string latency_out = "0";
void thr_latency(void) {
    while(1) {
        latency_mtx.lock();
        latency_out = get_latency(hostname);
        cout << "[PING]" << latency_out << endl;

        latency_mtx.unlock();
        sleep(3);
    }
}

std::string get_data() {
    latency_mtx.lock();
    string out = latency_out ;
    cout << "[OUT]" << out << endl;

    latency_mtx.unlock();
    return out;
}

class LatencyLoad : public ResourceManager {
public:
    LatencyLoad(string uri, string url, uint16_t port) : ResourceManager(uri, url, port, get_data) {}
    ~LatencyLoad() {}
};

int main(int argc, char** argv) {
	if(argc != 3) {
		return EXIT_FAILURE;
    }

    hostname = argv[2];

    thread latency_thread = thread(thr_latency);

    LatencyLoad service(argv[1], "/latency", LAT_PORT);
    service.start_resource_service();
    service.announce_service();

    while(1) {
        pause();
    }
	
	return EXIT_SUCCESS;
}