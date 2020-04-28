/**
 * \class McDst
 * \brief Monte Carlo DST format holder
 *
 * The class McDst holds the Monte Carlo data structure and access
 * methods to the branches
 */

#ifndef McDst_h
#define McDst_h

// ROOT headers
#include "TClonesArray.h"

// McDst headers
#include "McArrays.h"

// Forward declarations
class McEvent;
class McParticle;

//________________
class McDst {

 public:
  /// Default constructor
  McDst()  { /* emtpy */ }
  /// Destructor
  virtual ~McDst();

  /// Set pointers to the TClonesArrays
  static void set(TClonesArray** array);
  /// Reset pointers to the TClonesArrays to 0
  static void unset();
  /// Return pointer to the i-th TClonesArray
  static TClonesArray* mcArray(Int_t type) { return mcArrays[type]; }
  /// Return pointer to the McEvent information
  static McEvent* event() { return (McEvent*)mcArrays[McArrays::Event]->UncheckedAt(0); }
  /// Return pointer to the i-th particle
  static McParticle* particle(Int_t i) { return (McParticle*)mcArrays[McArrays::Particle]->UncheckedAt(i); }
  /// Return number of particles in the current events
  static UInt_t numberOfParticles() { return mcArrays[McArrays::Particle]->GetEntriesFast(); }

  /// Print information
  static void print();
  /// Print event information
  static void printEventInfo();
  /// Print particle information
  static void printParticles();

 private:
  /// Array of TClonesArrays
  static TClonesArray** mcArrays;

#ifdef __ROOT__
  ClassDef(McDst, 0)
#endif

};

#endif // #define McDst_h
