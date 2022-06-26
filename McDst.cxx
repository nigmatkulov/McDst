//
// The class holds DST structure and access methods
//

// C++ headers
#include <iostream>

// McDst headers
#include "McEvent.h"
#include "McParticle.h"
#include "McDst.h"       // MUST be the last one

#ifdef __ROOT__
ClassImp(McDst)
#endif

TClonesArray** McDst::mcArrays = nullptr;

//_________________
McDst::~McDst() {
  /* empty */
}

//________________
void McDst::unset() {
  // Unset pointers
  mcArrays = nullptr;
}

//________________
void McDst::set(TClonesArray** theMcArrays) {
  // Set pointers
  mcArrays = theMcArrays;
}

//________________
void McDst::print() {
  // Print all information
  std::cout << "\n==================== Full event information ====================\n";
  printEventInfo();
  printParticles();
  std::cout << "\n================================================================\n";
}

//________________
void McDst::printEventInfo() {
  // Print event information
  event()->print();
}

//________________
void McDst::printParticles() {
  // Print all particles
  if( numberOfParticles() == 0 ) {
    std::cout << "No particles found!" << std::endl;
    return;
  }

  std::cout << "\n Particle list contains: " << numberOfParticles() << " entries\n\n";
  // Particle loop
  for(UInt_t iPart=0; iPart<numberOfParticles(); iPart++) {
    std::cout << "+++ particles #[" << iPart << "/" << numberOfParticles() << "]\n";
    particle(iPart)->print();
  }

  std::cout << std::endl;
}
