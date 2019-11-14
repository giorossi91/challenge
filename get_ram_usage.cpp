#include <sys/sysinfo.h>
#include <iostream>
#include <cstdlib>
#include <unistd.h>


#include <memory>
#include <future>
#include <cstdio>
#include <restbed>
#include <sstream>

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

int main(int argc, char** argv) {
	if(argc != 2) {
		return EXIT_FAILURE;
	}
	
	while(1) {
		double load = get_ram_usage();
		stringstream ss_len;


		stringstream ss;
		ss << load;
		
		ss_len << ss.str().length();
		
		cout << "[RAM load] " << ss.str() << endl;
		
		auto request = make_shared< Request >( Uri( argv[1] ) );
		request->set_header("Accept", "*/*" );
		request->set_header("Host", "localhost");
		request->set_header("Content-Type", "application/x-www-form-urlencoded");
		request->set_header("Content-Length", ss_len.str());
		request->set_method("POST");
		request->set_body(ss.str());

		auto future = Http::async( request, [ ]( const shared_ptr< Request >, const shared_ptr< Response > response ) {
			fprintf( stderr, "Printing async response\n" );
		});

		future.wait( );
		
		sleep(1);
	}
	
	return EXIT_SUCCESS;
}