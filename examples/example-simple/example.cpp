#include "http-cpp0x.hpp"

#include <iostream>
#include <iterator>
#include <map>

using namespace cpp0x::http;
using std::cout;
using std::endl;

class my_request_handler_t
{
 public:
	void operator()( request_t const & request )
	{
		// Upon every request, print it to the screen and echo it back to the sender

		cout << "\nGot a request!" << endl;
		if( request._uri._raw.size() )
		{
			cout << "URI: \"" << request._uri._raw << "\"" << endl;
			cout << "  path = \"" << request._uri._path << "\"" << endl;
		}
		cout << "Content:" << endl;
		auto& buf = request.get_buffer();
		cout << "\t";
		for( auto it = buf.cbegin(); it != buf.cend(); ++it )
		{
			auto& c = *it;
			cout << *it;
			if( c == '\n' )
			{
				cout << "\t";
			}
		}

		char const raw_header[] = "HTTP/1.0 200 OK\r\n\r\n";
		buffer_t header_ok;
		header_ok.insert( header_ok.begin(), &raw_header[0], &raw_header[sizeof(raw_header) - 1]);
		request.send_to_client( header_ok );

		request.send_to_client( buf );
	}
};

int main( int const argc, char const * const * const argv )
{
	server_config_t cfg;
	cfg._port = 47890;

	my_request_handler_t request_handler;
	server_t<decltype( request_handler )> server( request_handler );
	if( server.init( cfg ) )
	{
		cout << "Error configuring server!" << endl;
		return 1;
	}

	cout << "Listen socket ready on port " << cfg._port << endl;

	while( true )
	{
		// This tells the HTTP server to use some CPU time, since it has no thread 
		// of it's own. This takes care of:
		//   1. Accepting incoming connections
		//   2. Reading requests and calling the request_handler
		//   
		server.work();
	}

	return 0;
}

