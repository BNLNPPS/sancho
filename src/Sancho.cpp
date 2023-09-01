#include <Sancho.h>

#include "example/common/root_certificates.hpp"

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>

namespace beast = boost::beast; // from <boost/beast.hpp>
namespace http = beast::http;   // from <boost/beast/http.hpp>
namespace net = boost::asio;    // from <boost/asio.hpp>
namespace ssl = net::ssl;       // from <boost/asio/ssl.hpp>
using tcp = net::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/local_time_adjustor.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>

#include <boost/tokenizer.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/replace.hpp>

#include <boost/regex.hpp>

//#include <cstdlib>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <string>

//______________________________________________________________________________________

std::string indent(int level) {
  std::string s; 
  for (int i=0; i<level; i++) s += "  ";
  return s; 
} 

void printTree( boost::property_tree::ptree &pt, int level) {
  if (pt.empty()) {
    std::cerr << "\""<< pt.data()<< "\"";
  }

  else {
    if (level) std::cerr << std::endl; 

    std::cerr << indent(level) << "{" << std::endl;     

    for (boost::property_tree::ptree::iterator pos = pt.begin(); pos != pt.end();) {
      std::cerr << indent(level+1) << "\"" << pos->first << "\": "; 

      printTree(pos->second, level + 1); 
      ++pos; 
      if (pos != pt.end()) {
	std::cerr << ","; 
      }
      std::cerr << std::endl;
    } 

    std::cerr << indent(level) << " }";     
  }

  return; 
}


//______________________________________________________________________________________
http::response<http::string_body> GET( std::string target, std::vector<std::pair<std::string,std::string>> headers, std::string host, std::string port, int version );

//______________________________________________________________________________________
Sancho::Sancho() : 
  rucio_config( ".rucio/etc/rucio.cfg" ),
  rucio_host(),
  rucio_auth_host(),
  rucio_auth_type(),
  rucio_account(),
  rucio_username(),
  rucio_password(), 
  rucio_token(),
  rucio_token_expiration(),
  rucio_version(),
  http_version(11),
  verbose(1),
  replacements()
{ 
  init();
}
//______________________________________________________________________________________
Sancho::Sancho( const char* config, std::vector<std::pair<std::string,std::string>> replacements_ ) : rucio_config( config ),
  rucio_host(),
  rucio_auth_host(),
  rucio_auth_type(),
  rucio_account(),
  rucio_username(),
  rucio_password(), 
  rucio_token(),
  rucio_token_expiration(),
  rucio_version(),
  http_version(11),
  verbose(1),
  replacements( replacements_ )
{
  init();
}
//______________________________________________________________________________________
bool Sancho::init(){
  bool result = false;

  boost::property_tree::ptree config;
  boost::property_tree::ini_parser::read_ini(rucio_config, config);

  if ( verbose ) {
    std::cout << "==============================================" << std::endl;
    std::cout << "Parsed rucio ini file." << std::endl;
    std::cout << "==============================================" << std::endl;
    //boost::property_tree::write_ini( std::cout, config );
  }
 
  // Setup rucio host and account information
  rucio_host      = config.get<std::string>("client.rucio_host");
  boost::replace_all( rucio_host, "https://", "" );
  boost::replace_all( rucio_host, "http://",  "" );
  std::cout << "client.rucio_host = " << rucio_host << std::endl;
  rucio_auth_host = config.get<std::string>("client.auth_host");
  boost::replace_all( rucio_auth_host, "https://", "" );
  boost::replace_all( rucio_auth_host, "http://",  "" );
  std::cout << "client.auth_host = " << rucio_auth_host << std::endl;
  rucio_auth_type = config.get<std::string>("client.auth_type");
  std::cout << "client.auth_type = " << rucio_auth_type << std::endl;
  rucio_account   = config.get<std::string>("client.account");
  std::cout << "client.account = " << rucio_account << std::endl;
  rucio_username  = config.get<std::string>("client.username");
  std::cout << "client.username = " << rucio_username << std::endl;
  rucio_password  = config.get<std::string>("client.password");
  std::cout << " client.password = xxxx-xxxx-xxxx" << std::endl;

  // Start with an expired token
  rucio_token_expiration = "Mon, 13 May 2013 10:23:03 UTC";
  rucio_token            = "X";

  // Ping the server
  ping();

  // Authenticate to the server
  auth();

  return result;
}

//______________________________________________________________________________________
bool Sancho::ping(){
  bool result = false;

  boost::char_separator<char> sep(":");
  boost::tokenizer< boost::char_separator<char> > tokens(rucio_host, sep);

  std::vector<std::string> hostport;
  for ( auto t : tokens ) {
    hostport.push_back(t);
  }

  std::string host=hostport[0];  
  std::string port=hostport[1];

  auto response = GET( "/ping", { /* no headers */ }, host, port, http_version );


  return result;
}

//______________________________________________________________________________________
bool Sancho::auth(){
  
  bool result = false;

  boost::char_separator<char> sep(":");
  boost::tokenizer< boost::char_separator<char> > tokens(rucio_auth_host, sep);

  std::vector<std::string> hostport;
  for ( auto t : tokens ) {
    hostport.push_back(t);
  }

  std::string host=hostport[0];  
  std::string port=hostport[1];
  
  auto response = GET( "/auth/userpass", {
      {"X-Forwarded-For",    "127.0.0.1"},
      {"X-Rucio-Account",    rucio_account },
      {"X-Rucio-AppID",      "sancho"},
      {"X-Rucio-Auth-Token", rucio_auth_type },
      {"X-Rucio-Username",   rucio_username  },
      {"X-Rucio-Password",   rucio_password  }
    }, host, port, http_version );

  rucio_token            = response["X-Rucio-Auth-Token"].to_string();
  rucio_token_expiration = response["X-Rucio-Auth-Token"].to_string();
  
  return result;
}
//______________________________________________________________________________________
std::vector<std::string> Sancho::lfn2pfns( const std::string lfn, const std::string scope, const std::string rse ) {

  std::vector<std::string> result;

  boost::char_separator<char> sep(":");
  boost::tokenizer< boost::char_separator<char> > tokens(rucio_auth_host, sep);

  std::vector<std::string> hostport;
  for ( auto t : tokens ) {
    hostport.push_back(t);
  }

  std::string host=hostport[0];  
  std::string port=hostport[1];

  std::string target = "/rses/";
  target += rse;
  target += "/lfns2pfns?lfn=";
  target += scope;
  target += ":";
  target += lfn;

  auto response = GET( target, {
      {"X-Forwarded-For",    "127.0.0.1"},
      {"X-Rucio-Account",    rucio_account },
      {"X-Rucio-AppID",      "sancho"},
      {"X-Rucio-Auth-Token", rucio_token }
    }, host, port, http_version );

  std::stringstream body ;
  body << response.body();

  boost::property_tree::ptree pt;
  boost::property_tree::read_json(body, pt);  

  for (boost::property_tree::ptree::iterator pos = pt.begin(); pos != pt.end(); ++pos ) {

    std::string pfn = (pos->second).data();
    std::string replaced;

    // Apply regular expressions in order that they were declared in ctor
    for ( auto rep : replacements ) {
      boost::regex re(rep.first); // regular expression will be replaced with...
      replaced = boost::regex_replace(pfn, re, rep.second);
      pfn      = replaced; // update the pfn
    }

    result.push_back( pfn );
  }
  
  return result;

};



//______________________________________________________________________________________
// s/dynamic_body/string_body
http::response<http::string_body> GET( std::string target_, std::vector<std::pair<std::string,std::string>> headers, std::string host_, std::string port_, int http_version ){
  // n.b. adapted from boost beast example


  // Declare a container to hold the response
  http::response<http::string_body> res;   

  try
    {

      auto const host = host_.c_str();
      auto const port = port_.c_str(); 
      auto const target = target_.c_str();
      int version = http_version;
      
      std::string target_ = target;
      
      // The io_context is required for all I/O
      net::io_context ioc;
      
      // The SSL context is required, and holds certificates
      ssl::context ctx(ssl::context::tlsv12_client);
      
      // This holds the root certificate used for verification
      load_root_certificates(ctx);
      
      // Verify the remote server's certificate
      ctx.set_verify_mode(ssl::verify_peer);
      
      // These objects perform our I/O
      tcp::resolver resolver(ioc);
      beast::ssl_stream<beast::tcp_stream> stream(ioc, ctx);
      
      // Set SNI Hostname (many hosts need this to handshake successfully)
      if(! SSL_set_tlsext_host_name(stream.native_handle(), host))
        {
	  beast::error_code ec{static_cast<int>(::ERR_get_error()), net::error::get_ssl_category()};
	  throw beast::system_error{ec};
        }
      
      // Look up the domain name
      auto const results = resolver.resolve(host, port);
      
      // Make the connection on the IP address we get from a lookup
      beast::get_lowest_layer(stream).connect(results);
      
      // Perform the SSL handshake
      stream.handshake(ssl::stream_base::client);
      
      // Set up an HTTP GET request message
      http::request<http::string_body> req{http::verb::get, target, version};
      req.set(http::field::host, host);
      req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

      // Set the headers for this request
      for ( auto h : headers ) req.set ( h.first, h.second );
      
      // Send the HTTP request to the remote host
      http::write(stream, req);
      
      // This buffer is used for reading and must be persisted
      beast::flat_buffer buffer;
      
      // Declare a container to hold the response
      //$$$ http::response<http::string_body> res;
      
      // Receive the HTTP response
      http::read(stream, buffer, res);
            
      // Gracefully close the stream
      beast::error_code ec;
      stream.shutdown(ec);
      if(ec == net::error::eof)
        {
	  // Rationale:
	  // http://stackoverflow.com/questions/25587403/boost-asio-ssl-async-shutdown-always-finishes-with-an-error
	  ec = {};
        }
      if(ec)
	throw beast::system_error{ec};
      
      // If we get here then the connection is closed gracefully
    }
  catch(std::exception const& e)
    {
      std::cerr << "Error: " << e.what() << std::endl;
      return res;
    }

  
  return res;

}
