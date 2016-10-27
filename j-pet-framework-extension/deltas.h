#ifndef _________DELTAS_H_______
#       define _________DELTAS_H_______
#include <JPetHit/JPetHit.h>
#include "PetDict.h"
struct SynchroStrip{double A,B;};
inline std::istream&operator>>(std::istream&str,SynchroStrip&item){
  return str>>item.A>>item.B;
}
inline std::ostream&operator<<(std::ostream&str,const SynchroStrip&item){
  return str<<item.A<<"\t"<<item.B;
}

#endif