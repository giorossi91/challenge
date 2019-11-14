#include <sys/sysinfo.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>

#include <memory>
#include <future>
#include <cstdio>
#include <restbed>
#include <sstream>

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

int main(int argc, char** argv) {
	if(argc != 2) {
		return EXIT_FAILURE;
	}
	
	while(1) {
		double load = get_cpu_load();

		stringstream ss;
		ss << load;
		
		auto request = make_shared< Request >( Uri( argv[1] ) );
		request->set_header("Accept", "*/*" );
		request->set_header("Host", "localhost");
		request->set_header("Content-Type", "application/x-www-form-urlencoded");
		request->set_header("Content-Length", ss.str());
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
