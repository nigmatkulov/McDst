/**
 * \class McParticle
 * \brief Class that describes particle
 *
 * The class descibes particle properties such as momentum, PDG codes, etc...
 */

#ifndef McParticle_h
#define McParticle_h

// C++ headers
#include <limits>

// ROOT headers
#include "TObject.h"
#include "TLorentzVector.h"
#include "TMath.h"
#include "TDatabasePDG.h"
#include "TParticlePDG.h"
#include "TParticle.h"

//_________________
class McParticle : public TObject {

 public:
  /// Default constructor
  McParticle();
  /// Constructor that takes parameters
  McParticle(const Int_t& index, const Int_t& pdg, const Int_t& status,
      	     const Int_t& parent, const Int_t& parentDecay,
      	     const Int_t& mate, const Int_t& decay, Int_t child[2],
      	     const Double_t& px, const Double_t& py, const Double_t& pz,
             const Double_t& e,
             const Double_t& x, const Double_t& y, const Double_t& z,
             const Double_t& t);
  /// Another constructor with parameters
  McParticle(const Int_t& index, const Int_t& pdg, const Int_t& status,
	     const Int_t& parent, const Int_t& parentDecay,
	     const Int_t& mate, const Int_t& decay, Int_t child[2],
	     const TLorentzVector& mom, const TLorentzVector& pos);
  /// Copy constructor that takes McParticle
  McParticle(const McParticle& right);
  /// Copy constructor that takes TParticle
  McParticle(const TParticle& right);
  /// Destructor
  virtual ~McParticle();
  /// Assignement operator
  const McParticle& operator=(const McParticle& right);
  /// Assignement operator
  const McParticle& operator=(const TParticle& right);
  /// Comparison operator
  Bool_t operator==(const McParticle& right) const;
  /// Print particle parameters
  virtual void print() const;
  /// Print particle parameters
  virtual void Print(Option_t* option = "") const;

  //
  // Getters
  //

  /// Return particle index
  Int_t index() const       { return (Int_t)fIndex; }
  /// Return PDG code
  Int_t pdg() const         { return fPdg; }
  /// Return particle status
  Int_t status() const      { return (Int_t)fStatus; }
  /// Return parent index
  Int_t parent() const      { return (Int_t)fParent; }
  /// Return parent decay index
  Int_t parentDecay() const { return (Int_t)fParentDecay; }
  /// Return index of the last collision partner
  Int_t mate() const        { return (Int_t)fMate; }
  /// Return decay index (-1 if not decayed)
  Int_t decay() const       { return (Int_t)fDecay; }
  /// Return index of the first child
  Int_t firstChild() const  { return (Int_t)fChild[0]; }
  /// Return index of the second child
  Int_t lastChild() const   { return (Int_t)fChild[1]; }
  /// Return px (GeV/c)
  Double_t px() const       { return (Double_t)fPx; }
  /// Return py (GeV/c)
  Double_t py() const       { return (Double_t)fPy; }
  /// Return pz (GeV/c)
  Double_t pz() const       { return (Double_t)fPz; }
  /// Return p (GeV/c)
  Double_t ptot() const
  { return TMath::Sqrt( px()*px() + py()*py() + pz()*pz() ); }
  /// Return transverse momentum (pT)
  Double_t pt() const
  { return TMath::Sqrt( px()*px() + py()*py() ); }
  Double_t phi() const      { return TMath::ATan2( py(), px() ); }
  /// Return mass according to the PDG code (GeV/c^2)
  Double_t pdgMass() const { 
    return ( TDatabasePDG::Instance()->GetParticle( fPdg ) ) ?
      TDatabasePDG::Instance()->GetParticle( fPdg )->Mass() : -999.; }
  /// Return mass according to the generator
  Double_t mass() const     { return momentum().M(); }
  /// Return charge
  Double_t charge() const { 
    return ( TDatabasePDG::Instance()->GetParticle( fPdg ) ) ?
      TDatabasePDG::Instance()->GetParticle( fPdg )->Charge() : 0; }
  /// Return energy of the particle (GeV)
  Double_t energy() const   { return (Double_t)fE; }
  /// Return energy (GeV)
  Double_t e() const        { return energy(); }
  /// Calculate particle energy using PDG mass
  Double_t pdgEnergy() const
  { return TMath::Sqrt( ptot()*ptot() + pdgMass()*pdgMass() ); }
  /// Return energy (GeV)
  Double_t pdgE() const     { return pdgEnergy(); }
  /// Return pseudorapidity
  Double_t eta() const      { return momentum().Eta(); }
  /// Return pseudorapidity
  Double_t pseudoRapidity() const { return momentum().Eta(); }
  /// Return four-momentum (px,py,pz,E)
  TLorentzVector momentum() const
  { return TLorentzVector( fPx, fPy, fPz, fE ); }
  /// Set four-momentum to the mom vector
  void momentum(TLorentzVector& mom) const { mom.SetPxPyPzE( fPx, fPy, fPz, fE ); }
  /// Return x position (fm)
  Double_t x() const        { return (Double_t)fX; }
  /// Return y position (fm)
  Double_t y() const        { return (Double_t)fY; }
  /// Return z position (fm)
  Double_t z() const        { return (Double_t)fZ; }
  /// Return t position (fm)
  Double_t t() const        { return (Double_t)fT; }
  /// Return four-coordinate (x,y,z,t)
  TLorentzVector position() const
  { return TLorentzVector( fX, fY, fZ, fT ); }
  /// Set four-coordinate to the pos vector
  void position(TLorentzVector& pos) const
  { pos.SetXYZT( fX, fY, fZ, fT); }

  //
  // Setters
  //

  /// Set particle index
  void setIndex(const Int_t& index)
  { fIndex = ( (index > std::numeric_limits<unsigned short>::max() ) ?
	             std::numeric_limits<unsigned short>::max() : (UShort_t)index ); }
  /// Set PdgId (pdg code)
  void setPdg(const Int_t& pdg)       { fPdg = pdg; }
  /// Set status
  void setStatus(const Int_t& status)
  { if ( status <= std::numeric_limits<char>::min() ) { fStatus = std::numeric_limits<char>::min(); }
    else if ( status >= std::numeric_limits<char>::max() ) { fStatus = std::numeric_limits<char>::max(); }
    else { fStatus = (Char_t)status; } }
  /// Set parent index
  void setParent(const Int_t& parent)
  { fParent = ( ( parent > std::numeric_limits<unsigned short>::max() ) ?
		            std::numeric_limits<unsigned short>::max() : (UShort_t)parent ); }
  /// Set parent decay index
  void setParentDecay(const Int_t& parentDecay)
  { fParentDecay = ( ( parentDecay > std::numeric_limits<unsigned short>::max() ) ?
		                 std::numeric_limits<unsigned short>::max() : (UShort_t)parentDecay ); }
  /// Set index of the last collision partner
  void setMate(const Int_t& mate)
  { fMate = ( (mate > std::numeric_limits<unsigned short>::max() ) ?
	            std::numeric_limits<unsigned short>::max() : (UShort_t)mate ); }
  /// Set decay index (-1 if not decayed)
  void setDecay(const Int_t& decay)
  { fDecay = ( ( TMath::Abs(decay) > std::numeric_limits<short>::max() ) ?
	             std::numeric_limits<short>::max() : (Short_t)decay ); }
  /// Set 2 childer indeces
  void setChild(Int_t child[2])
  { setFirstChild( child[0] ); setLastChild( child[1] ); }
  /// Set index of the first child
  void setFirstChild(const Int_t& child)
  { fChild[0] = ( (child > std::numeric_limits<unsigned short>::max() ) ?
		              std::numeric_limits<unsigned short>::max() : (UShort_t)child ); }
  /// Set index of the second child
  void setLastChild(const Int_t& child)
  { fChild[1] = ( (child > std::numeric_limits<unsigned short>::max() ) ?
		              std::numeric_limits<unsigned short>::max() : (UShort_t)child ); }
  /// Set px (GeV/c)
  void setPx(const Double_t& px)         { fPx = (Float_t)px; }
  /// Set py (GeV/c)
  void setPy(const Double_t& py)         { fPy = (Float_t)py; }
  /// Set pz (GeV/c)
  void setPz(const Double_t& pz)         { fPz = (Float_t)pz; }
  /// Set energy (GeV)
  void setE(const Double_t& e)           { fE = (Float_t)e; }
  /// Set four-momentum (px,py,pz,E)
  void setMomentum(const Double_t& px, const Double_t& py,
                   const Double_t& pz, const Double_t& e)
  { fPx = (Float_t)px; fPy = (Float_t)py; fPz = (Float_t)pz; fE = (Float_t)e; }
  /// Set four-momentum (TLorentzVector)
  void setMomentum(const TLorentzVector& mom)
  { fPx=(Float_t)mom.Px(); fPy=(Float_t)mom.Py(); fPz=(Float_t)mom.Pz(); fE=(Float_t)mom.E(); }
  /// Set x coordinate (fm)
  void setX(const Double_t& x)         { fX = (Float_t)x; }
  /// Set y coordinate (fm)
  void setY(const Double_t& y)         { fY = (Float_t)y; }
  /// Set z coordinate (fm)
  void setZ(const Double_t& z)         { fZ = (Float_t)z; }
  /// Set t coordinate (fm/c)
  void setT(const Double_t& t)         { fT = (Float_t)t; }
  /// Set four-coordinate (x,y,z,t)
  void setPosition(const Double_t& x, const Double_t& y,
                   const Double_t& z, const Double_t& t)
  { fX = (Float_t)x; fY = (Float_t)y; fZ = (Float_t)z; fT = (Float_t)t; }
  /// Set four-coordinate (TLorentzVector)
  void setPosition(const TLorentzVector& pos)
  { fX=(Float_t)pos.X(); fY=(Float_t)pos.Y(); fZ=(Float_t)pos.Z(); fT=(Float_t)pos.T(); }

 private:
  /// Index of this particle
  UShort_t fIndex;
  /// PDG code
  Int_t fPdg;
  /// Status
  Char_t fStatus;
  /// Index of the parent
  UShort_t fParent;
  /// Parent decay index
  UShort_t fParentDecay;
  /// Index of the last collision partner
  UShort_t fMate;
  /// Decay index (-1 if not decayed)
  Short_t fDecay;
  /// Index of the first and the last child
  UShort_t fChild[2];
  /// px (GeV/c)
  Float_t fPx;
  /// py (GeV/c)
  Float_t fPy;
  /// pz (GeV/c)
  Float_t fPz;
  /// x (fm)
  Float_t fX;
  /// y (fm)
  Float_t fY;
  /// z (fm)
  Float_t fZ;
  /// t (fm/c)
  Float_t fT;
  /// Energy (GeV)
  Float_t fE;

  ClassDef(McParticle, 2);
};

#endif // #define McParticle_h
