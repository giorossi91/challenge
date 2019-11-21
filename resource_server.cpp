#include "resource_server.h"

using namespace std;
using namespace restbed;
using std::chrono::system_clock;

get_data_func_T get_data_func;

bool need_announce = true;
thread monitor;
mutex time_mtx;
system_clock::time_point last_time_seen;

ResourceManager::ResourceManager(std::string uri_gtw, std::string url, uint16_t port, get_data_func_T func) {
    _url = url;
    _port = port;
    _uri_gtw = uri_gtw;
    get_data_func = func;
    last_time_seen = system_clock::now();
}

ResourceManager::~ResourceManager() {
}

void announce_thread(ResourceManager *obj) {
    while(need_announce) {
        cout << "[Resource Manager] Trying to announce..." << endl;
        string data = obj->_url + ";" + ::to_string(obj->_port);

        auto request = make_shared< Request >( Uri( obj->_uri_gtw + "/announce" ) );
        request->set_header("Accept", "*/*" );
        request->set_header("Host", "ResourceManager" + obj->_url);
        request->set_header("Content-Type", "application/x-www-form-urlencoded");
        request->set_header("Content-Length", ::to_string(data.length()));
        request->set_method("POST");
        request->set_body(data);

        auto future = Http::async( request, [ obj ]( const shared_ptr< Request >, const shared_ptr< Response > response ) {
            auto length = response->get_header( "Content-Length", 0 );
            Http::fetch( length, response );
            
            fprintf( stdout, "[ ResourceManager - %s ] %.*s\n",  obj->_url.c_str(), ( int ) response->get_body().size( ), response->get_body().data( ) );
            if(response->get_status_code() == OK) {
                need_announce = false;
                cout << "[Resource Manager] Ready!" << endl;
            }
        });

        future.wait( );

        sleep(3);
    }
}

void ResourceManager::announce_service() {
    thread announce_thr(announce_thread, this);
    announce_thr.join();
}


void ResourceManager::get_resource_handler( const shared_ptr< Session > session ) {
    const auto request = session->get_request( );
    
    string data_to_send = get_data_func();
    session->close( OK, data_to_send, { { "Content-Length", ::to_string( data_to_send.size( ) ) } } );
    cout << "[ Data sent ] " <<  data_to_send << " // len : " << data_to_send.size( ) << endl;

    time_mtx.lock();
    last_time_seen = system_clock::now();
    time_mtx.unlock();
}


void ResourceManager::start_service_thread(ResourceManager *obj) {
    auto resource_get = make_shared< Resource >( );
    resource_get->set_path( obj->_url );
    resource_get->set_method_handler( "GET", ResourceManager::get_resource_handler );
	

    auto settings = make_shared< Settings >( );
    settings->set_port( obj->_port );
    settings->set_default_header( "Connection", "close" );
	
    cout << "Running Resource Service ( " << obj->_url << " - " << obj->_port << " )" << std::endl;
    Service service;
    service.publish( resource_get );
    service.start( settings );
}

void monitor_thread(ResourceManager *obj) {
    while(1) {
        time_mtx.lock();
        auto last_time = last_time_seen;
        time_mtx.unlock();
        
        auto now = system_clock::now();
        std::chrono::duration<double> elapsed_time = now - last_time;
        if(elapsed_time.count() > RESOURCE_RETRY_THRESHOLD) {
            cout << "[Resource Manager] Re-announce..." << endl;
            need_announce = true;
            obj->announce_service();
        }
        sleep(1);
    }
}

void ResourceManager::start_resource_service() {
    _resource_thread = thread(&ResourceManager::start_service_thread, this);
    monitor = thread(monitor_thread, this);
}