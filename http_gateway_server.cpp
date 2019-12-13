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

#define GTW_IP "172.31.44.121"
#define PROMETHEUS_EXPORT_PORT 9091
#define GIVEUP_THRESHOLD 5.0 //s

struct MicroService {
    string   service_ip;
    uint16_t service_port;
    system_clock::time_point last_time_seen;
    string   resource_url;

    string last_data;
};

string get_stdout_from_command(string cmd);
bool is_number(const std::string &s);

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
        fprintf( stdout, "[ Gateway ] Service Registered IP=%s %.*s\n", host_ip.c_str(), ( int ) body.size( ), body.data( ) );

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

            session->close( OK, "Service UP", { { "Content-Length", "10" } } );
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
            //cout << "[ Gateway ] Contacting " << service_list_per_host.first << endl;
            for(auto &service : service_list_per_host.second) {
                //cout << "[ Gateway ] Service " << service.first << " at " << service.second.service_port << endl;
                auto last_time = service.second.last_time_seen;
                auto now = system_clock::now();
                string metricname = service.second.resource_url.substr(1) + "_metric";
                string ip = service.second.service_ip;
                string key = service.second.resource_url.substr(1);
                string value = service.second.last_data;
                
                std::chrono::duration<double> elapsed_time = now - last_time;
                if(elapsed_time.count() > GIVEUP_THRESHOLD) { //dead!
                    service_map_mutex.lock();
                    service_map[service_list_per_host.first].erase(service.first); //cancello servizio non più esistente


                    service_map_mutex.unlock();
                    
                    string cmd_curl;
                    if(is_number(value)) {
                      cmd_curl = "curl -X DELETE http://" GTW_IP ":" + 
                       ::to_string(PROMETHEUS_EXPORT_PORT) +
                       "/metrics/job/" + metricname + 
                       "/instance/" + 
                       ip + 
                       "/team/saturno";
                    } else {
                    
                      cmd_curl = "curl -X DELETE http://" GTW_IP ":" + 
                       ::to_string(PROMETHEUS_EXPORT_PORT) +
                       "/metrics/job/" + metricname + 
                       "/instance/" + 
                       ip + 
                       "/team/saturno/"+ key + "/" + value;
                    }
                    
                    

                    get_stdout_from_command(cmd_curl);
                    
                    cout << "[Gateway] Service Removed: " << service.first << endl;
                    continue;
                }


                stringstream port_ss;
                port_ss << service.second.service_port;

                auto request = make_shared< Request >( Uri( "http://" + service.second.service_ip + ":" + port_ss.str() + service.second.resource_url ) );
                request->set_header("Accept", "*/*" );
                request->set_header("Host", "gateway");

                auto future = Http::async( request, [ service ]( const shared_ptr< Request >, const shared_ptr< Response > response ) {
                    auto length = response->get_header( "Content-Length", 0 );
                    Http::fetch( length, response );

                    if(response->get_status_code() == OK) {
                        service_map_mutex.lock();
                        auto &services = service_map[service.second.service_ip];
                        services[service.second.resource_url].last_time_seen = system_clock::now();
                        service_map_mutex.unlock();
                        
                        fprintf( stdout, "[ %s -- %s:%d ] %.*s\n",  
                            service.second.resource_url.c_str(), 
                            service.second.service_ip.c_str(), 
                            service.second.service_port, 
                            ( int ) response->get_body().size( ), 
                            response->get_body().data() 
                        );

                        string data_received;
                        data_received.assign((const char *)response->get_body().data(), ( int ) response->get_body().size( ));

                        service_map_mutex.lock();
                        service_map[service.second.service_ip].at(service.second.resource_url).last_data = data_received;
                        service_map_mutex.unlock();

                    } else {
                        cout << "[Gateway] Errore " << service.first << endl;
                    }

                });


                future.wait( );
            }
        }
        sleep(1);
    }

}

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

bool is_number(const std::string &s) {
    unsigned int dot_number = 0;
    bool is_num = true;
    unsigned int i = 0;
    while(is_num && i < s.length()) {
        if(!isdigit(s[i])) {
            if(s[i] == '.') {
                dot_number ++;
            } else {
                is_num = false;
            }
        }

        if(dot_number > 1) {
            is_num = false;
        }
        i++;
    }

    return is_num;
}


void prometheus_exporter(void) {
    while(1) {
        service_map_mutex.lock();
        auto map_local = service_map;
        service_map_mutex.unlock();

        for(auto &service_list_per_host: map_local) {
            //cout << "[ Gateway Exporter ] Exporting " << service_list_per_host.first << endl;
            for(auto &service : service_list_per_host.second) {

                string metricname = service.second.resource_url.substr(1) + "_metric";
                string ip = service.second.service_ip;
                string key = service.second.resource_url.substr(1);
                string value = service.second.last_data;

                string url, data;
                if(is_number(value)) {
                    url = "http://" GTW_IP ":" + ::to_string(PROMETHEUS_EXPORT_PORT) + "/metrics/job/" + metricname + "/instance/" + ip + "/team/saturno"; 
                    data = key + " " + value;
                } else {
                    url = "http://" GTW_IP ":" + ::to_string(PROMETHEUS_EXPORT_PORT) + "/metrics/job/" + metricname + "/instance/" + ip + "/team/saturno/" + key + "/" + value ; 
                    data = key + " -1.0";
                }

                cout << "[ Gateway Exporter ] Export URL=" << url << " DATA=" << data << endl;
                get_stdout_from_command(
                    "echo \""+ data + "\" | curl --data-binary @- " + url
                );
            }
        }


        sleep(1);
    }

}

int main( const int, const char** ) {
    thread annouce_manager(annouce_server_thread);
    thread request_manager(request_data_from_services);
    thread thread_exporter_manager(prometheus_exporter);

    annouce_manager.join();
    request_manager.join();
    thread_exporter_manager.join();

    return EXIT_SUCCESS;
}
