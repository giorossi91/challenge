#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <unistd.h>


#include <memory>
#include <future>
#include <restbed>
#include <sstream>

using namespace std;
using namespace restbed;


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

int main(int argc, char** argv) {
	if(argc != 3) {
		return EXIT_FAILURE;
	}
	
	while(1) {
		string latency = get_latency(argv[2]);
		stringstream ss_len;
		ss_len << latency.length();


		cout << "[Latency from " << argv[2] << "] " << latency << " ms" << endl;

		auto request = make_shared< Request >( Uri( argv[1] ) );
		request->set_header("Accept", "*/*" );
		request->set_header("Host", "localhost");
		request->set_header("Content-Type", "application/x-www-form-urlencoded");
		request->set_header("Content-Length", ss_len.str());
		request->set_method("POST");
		request->set_body(latency);

		auto future = Http::async( request, [ ]( const shared_ptr< Request >, const shared_ptr< Response > response ) {
            (void) response;
			fprintf( stderr, "Printing async response\n" );
		});

		future.wait( );
		
		sleep(1);
	}
	
	return EXIT_SUCCESS;
}
