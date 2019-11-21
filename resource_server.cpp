#include "resource_server.h"

using namespace std;
using namespace restbed;

get_data_func_T get_data_func;

ResourceManager::ResourceManager(std::string uri_gtw, std::string url, uint16_t port, get_data_func_T func) {
    _url = url;
    _port = port;
    _uri_gtw = uri_gtw;
    get_data_func = func;
}

ResourceManager::~ResourceManager() {
}

void ResourceManager::announce_service() const {
    string data = _url + ";" + ::to_string(_port);

    auto request = make_shared< Request >( Uri( _uri_gtw + "/announce" ) );
    request->set_header("Accept", "*/*" );
    request->set_header("Host", "ResourceManager" + _url);
    request->set_header("Content-Type", "application/x-www-form-urlencoded");
    request->set_header("Content-Length", ::to_string(data.length()));
    request->set_method("POST");
    request->set_body(data);

    auto future = Http::async( request, [ this ]( const shared_ptr< Request >, const shared_ptr< Response > response ) {
        fprintf( stdout, "[ ResourceManager - %s ] %.*s\n",  _url.c_str(), ( int ) response->get_body().size( ), response->get_body().data( ) );
    });

    future.wait( );
}


void ResourceManager::get_resource_handler( const shared_ptr< Session > session ) {
    const auto request = session->get_request( );
    
    string data_to_send = get_data_func();
    session->close( OK, data_to_send, { { "Content-Length", ::to_string( data_to_send.size( ) ) } } );
    cout << "[ Data sent ] " <<  data_to_send << " // len : " << data_to_send.size( ) << endl; 
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

void ResourceManager::start_resource_service() {
    _resource_thread = thread(&ResourceManager::start_service_thread, this);
}