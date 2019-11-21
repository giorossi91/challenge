#include "resource_server.h"

using namespace std;
using namespace restbed;

ResourceManager::ResourceManager(std::string uri_gtw, std::string url, uint16_t port) {
    _url = url;
    _port = port;
    _uri_gtw = uri_gtw;
}

ResourceManager::~ResourceManager() {
}

int16_t ResourceManager::announce_service() const {
    stringstream len_ss;
    stringstream port_ss;
    port_ss << _port;

    string data = _url + ";" + port_ss.str();
    len_ss << data.size();

    auto request = make_shared< Request >( Uri( _uri_gtw + "/announce" ) );
    request->set_header("Accept", "*/*" );
    request->set_header("Host", "ResourceManager" + _url);
    request->set_header("Content-Type", "application/x-www-form-urlencoded");
    request->set_header("Content-Length", len_ss.str());
    request->set_method("POST");
    request->set_body(data);

    auto future = Http::async( request, [  ]( const shared_ptr< Request >, const shared_ptr< Response > response ) {
        fprintf( stdout, "[ ResourceManager - %s ] %.*s\n",  _url.c_str(), ( int ) response->get_body().size( ), response->get_body().data( ) );
    });

    future.wait( );
}


void get_resource_handler( const shared_ptr< Session > session ) {
    const auto request = session->get_request( );
    stringstream len_ss;

    session->fetch( content_length, [ ]( const shared_ptr< Session > session, const Bytes & body ) {

        string data_to_send = get_data() ; //pure virtual

        len_ss << data_to_send.size();
        session->close( OK, data_to_send, { { "Content-Length", len_ss.str() } } );
    } 
    );
}


void ResourceManager::start_service_thread(void) {
    auto resource_get = make_shared< Resource >( );
    resource_get->set_path( _url );
    resource_get->set_method_handler( "GET", get_resource_handler );
	

    auto settings = make_shared< Settings >( );
    settings->set_port( _port );
    settings->set_default_header( "Connection", "close" );
	
    cout << "Running Resource Service ( " << _url << " - " << _port << " )" << std::endl;
    Service service;
    service.publish( resource_get );
    service.start( settings );
}

int16_t ResourceManager::start_resource_service() const {
    _resource_thread = thread(&ResourceManager::start_service_thread);
}