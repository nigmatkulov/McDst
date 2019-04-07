//
// The class keeps the information about generated event
//

// C++ headers
#include <iostream>

// ROOT headers
#include "TObject.h"
#include "TString.h"
#include "TClonesArray.h"

// McDst headers
#include "McEvent.h"

#ifdef __ROOT__
ClassImp(McEvent);
#endif

//_________________
McEvent::McEvent() : TObject(), fEventNr(0), fB(0), fPhi(0),
		     fNes(0), fStepNr(0), fStepT(0), fComment("") {
  // Default constructor
  /* empty */
}

//_________________
McEvent::McEvent(const McEvent& right) : TObject() {
  // Copy constructor
  fEventNr  = right.fEventNr;
  fB        = right.fB;
  fPhi      = right.fPhi;
  fNes      = right.fNes;
  fStepNr   = right.fStepNr;
  fStepT    = right.fStepT;
  fComment  = right.fComment;
}

//_________________
McEvent::~McEvent() {
  // Default destructor
  /* empty */
}

//_________________
void McEvent::print() const {
  // Print data members to the standard output
  std::cout << "---------------------------------------------" << std::endl
            << "-I-                 Event                 -I-" << std::endl
            << "Event number               : " << fEventNr << std::endl
            << "Impact parameter (fm)      : " << fB << std::endl
            << "Reaction plane angle (rad) : " << fPhi << std::endl
            << "Number of time steps       : " << fNes << std::endl
            << "Time step number           : " << fStepNr << std::endl
            << "Time of the time step (fm) : " << fStepT << std::endl
            << "Comment                    :\n" << fComment << std::endl;
  std::cout << "---------------------------------------------" << std::endl;
}

//_________________
void McEvent::Print( Option_t* option __attribute__((unused)) ) const {
  // Print data members to the standard output
  std::cout << "---------------------------------------------" << std::endl
            << "-I-                 Event                 -I-" << std::endl
            << "Event number               : " << fEventNr << std::endl
            << "Impact parameter (fm)      : " << fB << std::endl
            << "Reaction plane angle (rad) : " << fPhi << std::endl
            << "Number of time steps       : " << fNes << std::endl
            << "Time step number           : " << fStepNr << std::endl
            << "Time of the time step (fm) : " << fStepT << std::endl
            << "Comment                    :\n" << fComment << std::endl;
  std::cout << "---------------------------------------------" << std::endl;
}

//_________________
void McEvent::setParameters(const Int_t& eventNr, const Double_t& b,
												    const Double_t& phi, const Int_t& nes,
												    const Int_t& stepNr, const Double_t& stepT,
												    const char* comment) {
  // Set the event parameters
  fEventNr = (UInt_t)eventNr;
  fB = (Float_t)b;
  fPhi = (Float_t)phi;
  fNes = ( ( nes > std::numeric_limits<unsigned short>::max() ) ?
	   std::numeric_limits<unsigned short>::max() : (UShort_t)nes );
  fStepNr = ( ( stepNr > std::numeric_limits<unsigned short>::max() ) ?
	      std::numeric_limits<unsigned short>::max() : (UShort_t)stepNr ) ;
  fStepT = (Float_t)stepT;
  fComment = comment;
}
