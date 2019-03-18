//
// The class holds array names and their sizes
//

#include "McArrays.h"

//________________
McArrays::McArrays() {
  /* emtpy */
}

//________________
McArrays::~McArrays() {
  /* emtpy */
}

// Array names
//________________
const char* McArrays::mcArrayNames[NAllMcArrays] = { "Event", "Particle" };

// Array types
//________________
const char* McArrays::mcArrayTypes[NAllMcArrays] = { "McEvent", "McParticle" };

// Array sizes
// These are intial sizes. Automatically resized if too small.
// Choosing too large initial values gives a performance penalty when reading
// only selected UDst branches
//_________________
int McArrays::mcArraySizes[NAllMcArrays] = { 1, 1000 };
