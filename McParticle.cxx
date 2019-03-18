//
// The class describes particle properties such as momentum, PDG code, etc...
//

// C++ headers
#include <iostream>

// McDst headers
#include "McParticle.h"

#ifdef __ROOT__
ClassImp(McParticle);
#endif

//_________________
McParticle::McParticle() : TObject(),
  fIndex(0), fPdg(0), fStatus(0), fParent(0), fParentDecay(0),
  fMate(0), fDecay(0), fChild{}, fPx(0), fPy(0), fPz(0),
  fX(0), fY(0), fZ(0), fT(0) { // Default constructor
  /* empty */
}

//_________________
McParticle::McParticle( const Int_t& index, const Int_t& pdg,
			const Int_t& status,
			const Int_t& parent, const Int_t& parentDecay,
			const Int_t& mate, const Int_t& decay,
			Int_t child[2],
			const Double_t& px, const Double_t& py,
			const Double_t& pz, const Double_t& e,
			const Double_t& x, const Double_t& y,
			const Double_t& z, const Double_t& t) : TObject() {
  // Standard constructor
  fIndex = ( (index > std::numeric_limits<unsigned short>::max() ) ?
	     std::numeric_limits<unsigned short>::max() : (UShort_t)index );
  fPdg         = pdg;
  if ( status <= std::numeric_limits<char>::min() ) {
    fStatus = std::numeric_limits<char>::min();
  }
  else if ( status >= std::numeric_limits<char>::max() ) {
    fStatus = std::numeric_limits<char>::max();
  }
  else {
    fStatus = (Char_t)status;
  }
  fParent = ( ( parent > std::numeric_limits<unsigned short>::max() ) ?
	      std::numeric_limits<unsigned short>::max() : (UShort_t)parent );
  fParentDecay = ( ( parentDecay > std::numeric_limits<unsigned short>::max() ) ?
		   std::numeric_limits<unsigned short>::max() : (UShort_t)parentDecay );
  fMate = ( (mate > std::numeric_limits<unsigned short>::max() ) ?
	    std::numeric_limits<unsigned short>::max() : (UShort_t)mate );
  fDecay = ( ( TMath::Abs(decay) > std::numeric_limits<short>::max() ) ?
	     std::numeric_limits<short>::max() : (Short_t)decay );
  fChild[0] = ( (child[0] > std::numeric_limits<unsigned short>::max() ) ?
		std::numeric_limits<unsigned short>::max() : (UShort_t)child[0] );
  fChild[1] = ( (child[1] > std::numeric_limits<unsigned short>::max() ) ?
		std::numeric_limits<unsigned short>::max() : (UShort_t)child[1] );
  fPx = (Float_t)px;
  fPy = (Float_t)py;
  fPz = (Float_t)pz;
  fX  = (Float_t)x;
  fY  = (Float_t)y;
  fZ  = (Float_t)z;
  fT  = (Float_t)t;
}

//_________________
McParticle::McParticle( const Int_t& index, const Int_t& pdg,
                      const Int_t& status,
            		      const Int_t& parent, const Int_t& parentDecay,
            		      const Int_t& mate, const Int_t& decay,
                      Int_t child[2],
            		      const TLorentzVector& mom,
                      const TLorentzVector& pos) : TObject() {
  // Standard constructor
  fIndex = ( (index > std::numeric_limits<unsigned short>::max() ) ?
	     std::numeric_limits<unsigned short>::max() : (UShort_t)index );
  fPdg = pdg;
  if ( status <= std::numeric_limits<char>::min() ) {
    fStatus = std::numeric_limits<char>::min();
  }
  else if ( status >= std::numeric_limits<char>::max() ) {
    fStatus = std::numeric_limits<char>::max();
  }
  else {
    fStatus = (Char_t)status;
  }
  fParent = ( ( parent > std::numeric_limits<unsigned short>::max() ) ?
	      std::numeric_limits<unsigned short>::max() : (UShort_t)parent );
  fParentDecay = ( ( parentDecay > std::numeric_limits<unsigned short>::max() ) ?
		   std::numeric_limits<unsigned short>::max() : (UShort_t)parentDecay );
  fMate = ( (mate > std::numeric_limits<unsigned short>::max() ) ?
	    std::numeric_limits<unsigned short>::max() : (UShort_t)mate );
  fDecay = ( ( TMath::Abs(decay) > std::numeric_limits<short>::max() ) ?
	     std::numeric_limits<short>::max() : (Short_t)decay );
  fChild[0] = ( (child[0] > std::numeric_limits<unsigned short>::max() ) ?
		std::numeric_limits<unsigned short>::max() : (UShort_t)child[0] );
  fChild[1] = ( (child[1] > std::numeric_limits<unsigned short>::max() ) ?
		std::numeric_limits<unsigned short>::max() : (UShort_t)child[1] );
  fPx = (Float_t)mom.Px();
  fPy = (Float_t)mom.Py();
  fPz = (Float_t)mom.Pz();
  fX = (Float_t)pos.X();
  fY = (Float_t)pos.Y();
  fZ = (Float_t)pos.Z();
  fT = (Float_t)pos.T();
}

//_________________
McParticle::McParticle(const McParticle& right) {
  // Copy constructor
  fIndex = right.fIndex;
  fPdg = right.fPdg;
  fStatus = right.fStatus;
  fParent = right.fParent;
  fParentDecay = right.fParentDecay;
  fMate = right.fMate;
  fDecay = right.fDecay;
  fChild[0] = right.fChild[0];
  fChild[1] = right.fChild[1];
  fPx = right.fPx;
  fPy = right.fPy;
  fPz = right.fPz;
  fX = right.fX;
  fY = right.fY;
  fZ = right.fZ;
  fT = right.fT;
}

//_________________
const McParticle& McParticle::operator=(const McParticle& right) {
  // Assignment operator
  if( this != &right ) {
    fIndex = right.fIndex;
    fPdg = right.fPdg;
    fStatus = right.fStatus;
    fParent = right.fParent;
    fParentDecay = right.fParentDecay;
    fMate = right.fMate;
    fDecay = right.fDecay;
    fChild[0] = right.fChild[0];
    fChild[1] = right.fChild[1];
    fPx = right.fPx;
    fPy = right.fPy;
    fPz = right.fPz;
    fX = right.fX;
    fY = right.fY;
    fZ = right.fZ;
    fT = right.fT;
  }
  return (*this);
}

//_________________
McParticle::McParticle(const TParticle &right) {
  // Copy constructor from the TParticle
  fIndex = 0;
  fPdg = right.GetPdgCode();
  fStatus = right.GetStatusCode();
  fParent = right.GetFirstMother();
  fParentDecay = 0;
  fMate = 0;
  fDecay = 0;
  fChild[0] = right.GetFirstDaughter();
  fChild[1] = right.GetLastDaughter();
  fPx = right.Px();
  fPy = right.Py();
  fPz = right.Pz();
  //fE = right.Energy();
  fX = right.Vx();
  fY = right.Vy();
  fZ = right.Vz();
  fT = right.T();
}

//_________________
const McParticle& McParticle::operator=(const TParticle &right) {
  // Assignment operator from the TParticle
  fIndex = 0;
  fPdg = right.GetPdgCode();
  fStatus = right.GetStatusCode();
  fParent = right.GetFirstMother();
  fParentDecay = 0;
  fMate = 0;
  fDecay = 0;
  fChild[0] = right.GetFirstDaughter();
  fChild[1] = right.GetLastDaughter();
  fPx = right.Px();
  fPy = right.Py();
  fPz = right.Pz();
  //fE = right.Energy();
  fX = right.Vx();
  fY = right.Vy();
  fZ = right.Vz();
  fT = right.T();

  return (*this);
}

//_________________
McParticle::~McParticle() { // Destructor
  /* empty */
}

//_________________
const Bool_t McParticle::operator == (const McParticle& right) const {
  // If equal operator
  return (
	  fIndex       == right.fIndex &&
	  fPdg         == right.fPdg &&
	  fStatus      == right.fStatus &&
	  fParent      == right.fParent &&
	  fParentDecay == right.fParentDecay &&
	  fMate        == right.fMate &&
	  fDecay       == right.fDecay &&
	  fChild[0]    == right.fChild[0] &&
	  fChild[1]    == right.fChild[1] &&
	  ((TMath::Abs((fPx-right.fPx)/fPx)<0.0001) ||
	   (TMath::Abs(fPx)<1e-16&&TMath::Abs(right.fPx)<1e-16)) &&
	  ((TMath::Abs((fPy-right.fPy)/fPy)<0.0001) ||
	   (TMath::Abs(fPy)<1e-16&&TMath::Abs(right.fPy)<1e-16)) &&
	  ((TMath::Abs((fPz-right.fPz)/fPz)<0.0001) ||
	   (TMath::Abs(fPz)<1e-16&&TMath::Abs(right.fPz)<1e-16)) &&
	  ((TMath::Abs((fX-right.fX)/fX)<0.0001) ||
	   (TMath::Abs(fX)<1e-16&&TMath::Abs(right.fX)<1e-16)) &&
	  ((TMath::Abs((fY-right.fY)/fY)<0.0001) ||
	   (TMath::Abs(fY)<1e-16&&TMath::Abs(right.fY)<1e-16)) &&
	  ((TMath::Abs((fZ-right.fZ)/fZ)<0.0001) ||
	   (TMath::Abs(fZ)<1e-16&&TMath::Abs(right.fZ)<1e-16)) &&
	  ((TMath::Abs((fT-right.fT)/fT)<0.0001) ||
	   (TMath::Abs(fT)<1e-16&&TMath::Abs(right.fT)<1e-16))
	  );
}

//_________________
void McParticle::print(Option_t* option) {
  // Print the data members to the standard output
  std::cout << "------------------------------------------------" << std::endl
	    << "-I-                 Particle                 -I-" << std::endl
	    << "Index                       : " << fIndex << std::endl
	    << "PDG code                    : " << fPdg << std::endl
	    << "Status code                 : " << fStatus << std::endl
	    << "Parent index                : " << fParent << std::endl
	    << "Parent decay index          : " << fParentDecay << std::endl
	    << "Last collision partner      : " << fMate << std::endl
	    << "Decay index                 : " << fDecay << std::endl
	    << "First child index           : " << fChild[0] << std::endl
	    << "Last child index            : " << fChild[1] << std::endl
	    << "Momentum (px, py, pz) (GeV) : (" << fPx << ", " << fPy << ", " << fPz << ")" << std::endl
            // << "Energy (GeV)                : " << fE << std::endl
	    << "Position (x, y, z) (fm)     : (" << fX << ", " << fY << ", " << fZ << ")" << std::endl
	    << "Creation time (fm)          : " << fT << std::endl
	    << "------------------------------------------------" << std::endl;
}

//_________________
void McParticle::Print(Option_t* option) {
  // Print the data members to the standard output
  std::cout << "------------------------------------------------" << std::endl
	    << "-I-                 Particle                 -I-" << std::endl
	    << "Index                       : " << fIndex << std::endl
	    << "PDG code                    : " << fPdg << std::endl
	    << "Status code                 : " << fStatus << std::endl
	    << "Parent index                : " << fParent << std::endl
	    << "Parent decay index          : " << fParentDecay << std::endl
	    << "Last collision partner      : " << fMate << std::endl
	    << "Decay index                 : " << fDecay << std::endl
	    << "First child index           : " << fChild[0] << std::endl
	    << "Last child index            : " << fChild[1] << std::endl
	    << "Momentum (px, py, pz) (GeV) : (" << fPx << ", " << fPy << ", " << fPz << ")" << std::endl
            // << "Energy (GeV)                : " << fE << std::endl
	    << "Position (x, y, z) (fm)     : (" << fX << ", " << fY << ", " << fZ << ")" << std::endl
	    << "Creation time (fm)          : " << fT << std::endl
	    << "------------------------------------------------" << std::endl;
}
