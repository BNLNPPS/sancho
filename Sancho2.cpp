// We use the example from the main readme file

#include <iostream>

#include <boost/lexical_cast.hpp>
#include <boost/fusion/adapted.hpp>

#define BOOST_LOG_DYN_LINK 1
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup.hpp>

#include <restc-cpp/restc-cpp.h>
#include <restc-cpp/RequestBuilder.h>

#include <string>

using namespace std;
using namespace restc_cpp;
namespace logging = boost::log;

const std::string address="https://sprucio01.sdcc.bnl.gov:443";

// C++ structure that match the JSON entries received
// from http://jsonplaceholder.typicode.com/posts/{id}
//struct Result {
//  int userId = 0;
//  int id = 0;
//  string title;
//  string body;
//};

struct Result {
  string version;
};

// Since C++ does not (yet) offer reflection, we need to tell the library how
// to map json members to a type. We are doing this by declaring the
// structs/classes with BOOST_FUSION_ADAPT_STRUCT from the boost libraries.
// This allows us to convert the C++ classes to and from JSON.

BOOST_FUSION_ADAPT_STRUCT(
    Result,
    (string, version)
    //    (int, userId)
    //    (int, id)
    //    (string, title)
    //    (string, body)
)

std::string lfn="user.jwebb2:calor_emcal-00021707-0182.prdf.panda1151";                                                                
std::string rse="BNL_PROD3";  


// Obtain an authentication token as a string and return it...
std::string get_token( std::string uname="sphnxpro", std::string pwd="xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx" ) {
  auto rest_client = RestClient::Create();
  std::string result_;
  rest_client->ProcessWithPromise([&](Context& ctx) {
      std::cout << "Sending authentication request" << std::endl;
      auto reply = RequestBuilder(ctx)
	. Get( address + "/auth/userpass" )
	. AddHeader("X-Forwarded-For", "127.0.0.1")
	. AddHeader("X-Rucio-Account", "sphnxpro")
	. AddHeader("X-Rucio-AppID", "sancho")
	. AddHeader("X-Rucio-Auth-Token", "userpass")
	. AddHeader("X-Rucio-Username", uname)
	. AddHeader("X-Rucio-Password", pwd)
	. Execute();
      // TODO... handle error return codes
      result_ = reply->GetHeader( "X-Rucio-Auth-Token" ).get();
      return;
    });  
  return result_;
}

void get_lfn2pfn( std::string lfn, std::string rse, std::string token ) {
  auto rest_client = RestClient::Create();
  rest_client->ProcessWithPromise([&](Context& ctx) {
      std::cout << "lfn2pfn" << std::endl;
      auto reply = RequestBuilder(ctx)
	. Get( address + "/rses/BNL_PROD2/lfns2pfns?" )
	. Argument( "lfn", lfn )
	//	. AddHeader("X-Rucio-Auth-Token", token )
	. Execute();
      std::cout << "Reponse code = " << reply->GetResponseCode() << std::endl;    
      std::cout << reply->GetBodyAsString() << std::endl;
      return;
    });  
  return;
}

void get_rse( std::string rse, std::string token ) {
  auto rest_client = RestClient::Create();
  rest_client->ProcessWithPromise([&](Context& ctx) {
      auto reply = RequestBuilder(ctx)
	. Get( address + "/rses/"+rse )
	. AddHeader("X-Rucio-Auth-Token", token )
	. Execute();
      std::cout << "Reponse code = " << reply->GetResponseCode() << std::endl;    
      std::cout << reply->GetBodyAsString() << std::endl;
      return;
    });  
  return;
}

void get_rse_protocols( std::string rse, std::string token ) {
  auto rest_client = RestClient::Create();
  rest_client->ProcessWithPromise([&](Context& ctx) {
      auto reply = RequestBuilder(ctx)
	. Get( address + "/rses/"+rse+"/protocols" )
	. AddHeader("X-Rucio-Auth-Token", token )
	. Execute();
      std::cout << "Reponse code = " << reply->GetResponseCode() << std::endl;    
      std::cout << reply->GetBodyAsString() << std::endl;
      return;
    });  
  return;
}

void get_rse_( std::string rse, std::string info, std::string token ) {
  auto rest_client = RestClient::Create();
  rest_client->ProcessWithPromise([&](Context& ctx) {
      std::cout << address+"/rses/"+rse+"/"+info << std::endl;
      auto reply = RequestBuilder(ctx)
	. Get( address + "/rses/"+rse+"/"+info )
	. AddHeader("X-Rucio-Auth-Token", token )
	. Execute();
      std::cout << "Reponse code = " << reply->GetResponseCode() << std::endl;    
      std::cout << reply->GetBodyAsString() << std::endl;
      return;
    });  
  return;
}



int main() {

  // Obtain a token
  auto token=get_token();
  std::cout << "X-Rucio-Auth-Token: " << token << std::endl;

  //  get_rse ( "BNL_PROD2", token );
  //  get_rse_( "BNL_PROD2", "protocols", token );
  //  get_rse_( "BNL_PROD2", "accounts", token );
  //  get_rse_( "BNL_PROD2", "accounts/usage", token );
  get_rse_( "BNL_PROD3", "lfns2pfns?lfn=user.jwebb2:user.jwebb2.calor-calib-0evts_jwebb210379_001_CalorChain.log.1198.001297.log.tgz", token );


  return 0;
  

};

