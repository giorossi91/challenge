#include <memory>
#include <cstdlib>
#include <restbed>
#include <iostream>

using namespace std;
using namespace restbed;

void post_method_handler( const shared_ptr< Session > session )
{
    const auto request = session->get_request( );

    int content_length = request->get_header( "Content-Length", 0 );

    session->fetch( content_length, [ ]( const shared_ptr< Session > session, const Bytes & body )
    {
        fprintf( stdout, "%.*s\n", ( int ) body.size( ), body.data( ) );
        session->close( OK, "Bravo!!", { { "Content-Length", "13" } } );
    } );
}

int main( const int, const char** )
{
    auto resource_cpu = make_shared< Resource >( );
    resource_cpu->set_path( "/cpu" );
    resource_cpu->set_method_handler( "POST", post_method_handler );
	
	auto resource_ram = make_shared< Resource >( );
    resource_ram->set_path( "/ram" );
    resource_ram->set_method_handler( "POST", post_method_handler );
	
	auto resource_gateway = make_shared< Resource >( );
    resource_gateway->set_path( "/gateway" );
    resource_gateway->set_method_handler( "POST", post_method_handler );
	
	auto resource_latency = make_shared< Resource >( );
    resource_latency->set_path( "/latency" );
    resource_latency->set_method_handler( "POST", post_method_handler );
	
	
	

    auto settings = make_shared< Settings >( );
    settings->set_port( 10000 );
    settings->set_default_header( "Connection", "close" );
	
    cout << "Running..." << std::endl;
    Service service;
    service.publish( resource_cpu );
	service.publish( resource_ram );
	service.publish( resource_gateway );
	service.publish( resource_latency );
    service.start( settings );

    return EXIT_SUCCESS;
}
