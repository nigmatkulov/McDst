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
  McParticle(const int& index, const int& pdg, const int& status,
      	     const int& parent, const int& parentDecay,
      	     const int& mate, const int& decay, int child[2],
      	     const double& px, const double& py, const double& pz,
             const double& e,
             const double& x, const double& y, const double& z,
             const double& t);
  /// Another constructor with parameters
  McParticle(const int& index, const int& pdg, const int& status,
	     const int& parent, const int& parentDecay,
	     const int& mate, const int& decay, int child[2],
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
  int index() const       { return (int)fIndex; }
  /// Return PDG code
  int pdg() const         { return fPdg; }
  /// Return particle status
  int status() const      { return (int)fStatus; }
  /// Return parent index
  int parent() const      { return (int)fParent; }
  /// Return parent decay index
  int parentDecay() const { return (int)fParentDecay; }
  /// Return index of the last collision partner
  int mate() const        { return (int)fMate; }
  /// Return decay index (-1 if not decayed)
  int decay() const       { return (int)fDecay; }
  /// Return index of the first child
  int firstChild() const  { return (int)fChild[0]; }
  /// Return index of the second child
  int lastChild() const   { return (int)fChild[1]; }
  /// Return px (GeV/c)
  double px() const       { return (double)fPx; }
  /// Return py (GeV/c)
  double py() const       { return (double)fPy; }
  /// Return pz (GeV/c)
  double pz() const       { return (double)fPz; }
  /// Return p (GeV/c)
  double ptot() const
  { return TMath::Sqrt( px()*px() + py()*py() + pz()*pz() ); }
  /// Return transverse momentum (pT)
  double pt() const
  { return TMath::Sqrt( px()*px() + py()*py() ); }
  double phi() const      { return TMath::ATan2( py(), px() ); }
  /// Return mass according to the PDG code (GeV/c^2)
  double pdgMass() const { 
    return ( TDatabasePDG::Instance()->GetParticle( fPdg ) ) ?
      TDatabasePDG::Instance()->GetParticle( fPdg )->Mass() : -999.; }
  /// Return mass according to the generator
  double mass() const     { return momentum().M(); }
  /// Return charge
  double charge() const { 
    return ( TDatabasePDG::Instance()->GetParticle( fPdg ) ) ?
      TDatabasePDG::Instance()->GetParticle( fPdg )->Charge() : 0; }
  /// Return energy of the particle (GeV)
  double energy() const   { return (double)fE; }
  /// Return energy (GeV)
  double e() const        { return energy(); }
  /// Calculate particle energy using PDG mass
  double pdgEnergy() const
  { return TMath::Sqrt( ptot()*ptot() + pdgMass()*pdgMass() ); }
  /// Return energy (GeV)
  double pdgE() const     { return pdgEnergy(); }
  /// Return pseudorapidity
  double eta() const      { return momentum().Eta(); }
  /// Return pseudorapidity
  double pseudoRapidity() const { return momentum().Eta(); }
  /// Return four-momentum (px,py,pz,E)
  TLorentzVector momentum() const
  { return TLorentzVector( fPx, fPy, fPz, fE ); }
  /// Set four-momentum to the mom vector
  void momentum(TLorentzVector& mom) const { mom.SetPxPyPzE( fPx, fPy, fPz, fE ); }
  /// Return x position (fm)
  double x() const        { return (double)fX; }
  /// Return y position (fm)
  double y() const        { return (double)fY; }
  /// Return z position (fm)
  double z() const        { return (double)fZ; }
  /// Return t position (fm)
  double t() const        { return (double)fT; }
  /// Return four-coordinate (x,y,z,t)
  TLorentzVector position() const
  { return TLorentzVector( fX, fY, fZ, fT ); }
  /// Set four-coordinate to the pos vector
  void position(TLorentzVector& pos) const
  { pos.SetXYZT( fX, fY, fZ, fT); }
  /// Return space-time rapidity
  double etaS() const
  { return 0.5 * TMath::Log( (fT + fZ) / (fT - fZ) ); }
  /// Return proper time (fm/c)
  double tau() const
  { return ( ( fT*fT > fZ*fZ) ) ? TMath::Sqrt( fT*fT - fZ * fZ ) : -0.5; }

  //
  // Setters
  //

  /// Set particle index
  void setIndex(const int& index)
  { fIndex = ( (index > std::numeric_limits<unsigned short>::max() ) ?
	             std::numeric_limits<unsigned short>::max() : (UShort_t)index ); }
  /// Set PdgId (pdg code)
  void setPdg(const int& pdg)       { fPdg = pdg; }
  /// Set status
  void setStatus(const int& status)
  { if ( status <= std::numeric_limits<char>::min() ) { fStatus = std::numeric_limits<char>::min(); }
    else if ( status >= std::numeric_limits<char>::max() ) { fStatus = std::numeric_limits<char>::max(); }
    else { fStatus = (Char_t)status; } }
  /// Set parent index
  void setParent(const int& parent)
  { fParent = ( ( parent > std::numeric_limits<unsigned short>::max() ) ?
		            std::numeric_limits<unsigned short>::max() : (UShort_t)parent ); }
  /// Set parent decay index
  void setParentDecay(const int& parentDecay)
  { fParentDecay = ( ( parentDecay > std::numeric_limits<unsigned short>::max() ) ?
		                 std::numeric_limits<unsigned short>::max() : (UShort_t)parentDecay ); }
  /// Set index of the last collision partner
  void setMate(const int& mate)
  { fMate = ( (mate > std::numeric_limits<unsigned short>::max() ) ?
	            std::numeric_limits<unsigned short>::max() : (UShort_t)mate ); }
  /// Set decay index (-1 if not decayed)
  void setDecay(const int& decay)
  { fDecay = ( ( TMath::Abs(decay) > std::numeric_limits<short>::max() ) ?
	             std::numeric_limits<short>::max() : (Short_t)decay ); }
  /// Set 2 childer indeces
  void setChild(int child[2])
  { setFirstChild( child[0] ); setLastChild( child[1] ); }
  /// Set index of the first child
  void setFirstChild(const int& child)
  { fChild[0] = ( (child > std::numeric_limits<unsigned short>::max() ) ?
		              std::numeric_limits<unsigned short>::max() : (UShort_t)child ); }
  /// Set index of the second child
  void setLastChild(const int& child)
  { fChild[1] = ( (child > std::numeric_limits<unsigned short>::max() ) ?
		              std::numeric_limits<unsigned short>::max() : (UShort_t)child ); }
  /// Set px (GeV/c)
  void setPx(const double& px)         { fPx = (Float_t)px; }
  /// Set py (GeV/c)
  void setPy(const double& py)         { fPy = (Float_t)py; }
  /// Set pz (GeV/c)
  void setPz(const double& pz)         { fPz = (Float_t)pz; }
  /// Set energy (GeV)
  void setE(const double& e)           { fE = (Float_t)e; }
  /// Set four-momentum (px,py,pz,E)
  void setMomentum(const double& px, const double& py,
                   const double& pz, const double& e)
  { fPx = (Float_t)px; fPy = (Float_t)py; fPz = (Float_t)pz; fE = (Float_t)e; }
  /// Set four-momentum (TLorentzVector)
  void setMomentum(const TLorentzVector& mom)
  { fPx=(Float_t)mom.Px(); fPy=(Float_t)mom.Py(); fPz=(Float_t)mom.Pz(); fE=(Float_t)mom.E(); }
  /// Set x coordinate (fm)
  void setX(const double& x)         { fX = (Float_t)x; }
  /// Set y coordinate (fm)
  void setY(const double& y)         { fY = (Float_t)y; }
  /// Set z coordinate (fm)
  void setZ(const double& z)         { fZ = (Float_t)z; }
  /// Set t coordinate (fm/c)
  void setT(const double& t)         { fT = (Float_t)t; }
  /// Set four-coordinate (x,y,z,t)
  void setPosition(const double& x, const double& y,
                   const double& z, const double& t)
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
