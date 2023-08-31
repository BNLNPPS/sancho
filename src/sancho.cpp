#include "Sancho.h"
#include <iostream>

int main() {
  Sancho s;

  std::string lfn="beam_emcal-00021707-0000.prdf.panda1194";

  // Both scope and lfn is required
  for ( auto p : s.lfn2pfns( lfn, "user.jwebb2" ) ) {
    std::cout << lfn << " -> " << p << std::endl;
  }

  return 0;
};
