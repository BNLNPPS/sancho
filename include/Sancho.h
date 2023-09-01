#ifndef __Sancho_h__
#define __Sancho_h__

#include <string>
#include <vector>
#include <utility>

class Sancho {
  
public:
  Sancho();
  Sancho( const char* config, std::vector<std::pair<std::string,std::string>> replacements={} );
 ~Sancho(){ /* nada */ }

  /// Given a logical filename, return a vector of all physical file locations on the
  /// specified resource element for the specified scope.
  std::vector<std::string> lfn2pfns( const std::string lfn, const std::string scope="group.sphenix", const std::string rse="BNL_PROD3" );

  
private:

  /// Read in initialization files and ping the rucio server  
  bool init();
  /// Ping the rucio server
  bool ping();
  /// Check authentication status and update as needed
  bool auth();

  std::string rucio_config;
  std::string rucio_host;
  std::string rucio_auth_host;
  std::string rucio_auth_type;
  std::string rucio_account;
  std::string rucio_username;
  std::string rucio_password;  
  std::string rucio_token;
  std::string rucio_token_expiration;
  std::string rucio_version;

  int http_version;
  int verbose;

  std::vector<std::pair<std::string,std::string>> replacements;

  
};
#endif
