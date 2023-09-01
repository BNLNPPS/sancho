#include "Sancho.h"
#include <iostream>
int main() {

  Sancho san( "~/.rucio/etc/rucio.cfg", 
	      { 
		{"localhost",""},
                {"file",""},
		{":///","/"}
	      });

  for ( auto pfn : san.lfn2pfns( "beam_emcal-00021707-0000.prdf.panda1194", "user.jwebb2", "BNL_PROD3" ) ) {
    std::cout << pfn << std::endl;
  }
  return 0;
};
