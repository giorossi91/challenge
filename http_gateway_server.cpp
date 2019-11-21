#include <memory>
#include <cstdlib>
#include <restbed>
#include <iostream>
#include <thread>
#include <map>
#include <mutex>
#include <vector>
#include <chrono>
#include <sstream>
#include <ctime>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

using namespace std;
using namespace restbed;
using std::chrono::system_clock;

struct MicroService {
    string   service_ip;
    uint16_t service_port;
    system_clock::time_point last_time_seen;
    string   resource_url;
};

mutex service_map_mutex;
map<string, map<string, MicroService>> service_map; // [ IP remoto ; { [url, info servizi] } ]

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

string get_ip(string rest_host) {
    vector<string> x = split_string(rest_host, ":");
    return x[2].substr(0, x[2].size() - 1);
}

void service_announce_method_handler( const shared_ptr< Session > session ) {
    const auto request = session->get_request( );

    int content_length = request->get_header( "Content-Length", 0 );
    string host_ip = get_ip(session->get_origin());

    session->fetch( content_length, [ host_ip ]( const shared_ptr< Session > session, const Bytes & body ) {
        fprintf( stdout, "[ Gateway ] IP=%s %.*s\n", host_ip.c_str(), ( int ) body.size( ), body.data( ) );

        // url;porta
        string data_received;
        data_received.assign((const char *)body.data(), ( int ) body.size( ));
        vector<string> data_array = split_string(data_received, ";");
        if(data_array.size() < 2) {
            session->close( BAD_REQUEST, "Doh! :-(", { { "Content-Length", "8" } } );
        } else {
            stringstream port_ss(data_array[1]); 

            uint16_t port = 0;
            port_ss >> port;

            MicroService new_service;
            new_service.service_ip = host_ip;
            new_service.service_port = port;
            new_service.resource_url = data_array[0];
            new_service.last_time_seen = system_clock::now();


            service_map_mutex.lock();
            map<string, MicroService> &service_list = service_map[host_ip];
            service_list[new_service.resource_url] = new_service;
            service_map_mutex.unlock();

            session->close( OK, "Bravo!!", { { "Content-Length", "7" } } );
        }
    } 
    );
}

void annouce_server_thread(void) {
    auto resource_announce = make_shared< Resource >( );
    resource_announce->set_path( "/announce" );
    resource_announce->set_method_handler( "POST", service_announce_method_handler );
	

    auto settings = make_shared< Settings >( );
    settings->set_port( 20000 );
    settings->set_default_header( "Connection", "close" );
	
    cout << "Running Announce Thread..." << std::endl;
    Service service;
    service.publish( resource_announce );
    service.start( settings );
}

void request_data_from_services(void) {
    while(1) {

        service_map_mutex.lock();
        auto map_local = service_map;
        service_map_mutex.unlock();

        for(auto &service_list_per_host: map_local) {
            cout << "[ Gateway ] Contacting " << service_list_per_host.first << endl;
            for(auto &service : service_list_per_host.second) {
                cout << "[ Gateway ] Service " << service.first << " at " << service.second.service_port << endl;
                stringstream port_ss;
                port_ss << service.second.service_port;

                auto request = make_shared< Request >( Uri( "http://" + service.second.service_ip + ":" + port_ss.str() + service.second.resource_url ) );
                request->set_header("Accept", "*/*" );
                request->set_header("Host", "gateway");

                auto future = Http::async( request, [ service ]( const shared_ptr< Request >, const shared_ptr< Response > response ) {
                    auto length = response->get_header( "Content-Length", 0 );
                    Http::fetch( length, response );

                    fprintf( stdout, "[ %s -- %s:%d ] %.*s\n",  
                        service.second.resource_url.c_str(), 
                        service.second.service_ip.c_str(), 
                        service.second.service_port, 
                        ( int ) response->get_body().size( ), 
                        response->get_body().data() 
                    );
                });

                future.wait( );
            }
        }
        sleep(1);
    }

}

int main( const int, const char** ) {
    thread annouce_manager(annouce_server_thread);
    thread request_manager(request_data_from_services);

    annouce_manager.join();
    request_manager.join();

    return EXIT_SUCCESS;
}
