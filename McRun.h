/**
 * \class McRun
 * \brief The class descibes collision parameters
 *
 * The McRun class describes collision parameters such
 * as projectile and target (A,Z), momentum, etc...
 */

#ifndef McRun_h
#define McRun_h

// C++ headers
#include <limits>

// ROOT headers
#include "TNamed.h"
#include "TString.h"

//_________________
class McRun : public TNamed {

 public:
  /// Default constructor
  McRun();
  /// Parametrized constructor
  McRun(const char* generator, const char* comment, const Int_t& aProj,
	const Int_t& zProj, const Double_t& pProj, const Int_t& aTarg,
	const Int_t& zTarg, const Double_t& pTarg, const Double_t& bMin,
	const Double_t& bMax, const Int_t& bWeight, const Double_t& phiMin,
	const Double_t& phiMax, const Double_t& sigma, const Int_t& nEvents);
  /// Default destructor
  virtual ~McRun();
  /// Print run info
  void print() const;
  /// Print run info
  void Print(Option_t* option = "") const;

  /// Proton mass (GeV/c^2)
  static Double_t mProtMass;
  /// Neutron mass (GeV/c^2)
  static Double_t mNeutMass;
  /// Charged pion mass (GeV/c^2)
  static Double_t mPionMass;

  //
  // Getters
  //

  /// Return generator name
  void generator(TString& gen)       { gen = fGenerator; }
  /// Return comment
  void comment(TString& cmnt)        { cmnt = fComment; }
  /// Return decayer name
  void decayer(TString& dec)         { dec = fDecayer; }
  /// Return number of nucleons in the projectile
  Int_t aProj() const                { return (Int_t)fAProj; }
  /// Return number of protons in the projectile
  Int_t zProj() const                { return (Int_t)fZProj; }
  /// Return momentum of the projectile
  Double_t pProj() const             { return (Double_t)fPProj; }
  /// Return number of nucleons in the target
  Int_t aTarg() const                { return (Int_t)fATarg; }
  /// Return number of protons in the target
  Int_t zTarg() const                { return (Int_t)fZTarg; }
  /// Return momentum of the target
  Double_t pTarg() const             { return (Double_t)fPTarg; }
  /// Return minimal impact parameter requested
  Double_t bMin() const              { return (Double_t)fBMin; }
  /// Return maximal impact parameter requested
  Double_t bMax() const              { return (Double_t)fBMax; }
  /// Impact parameter weighting:
  /// \param 0 for geometrical weights (bdb)
  /// \param 1 for flat distribution
  Int_t bWeight() const              { return (fBWeight) ? 1 : 0 ; }
  /// Return maximal phi angle requested
  Double_t phiMax() const            { return (Double_t)fPhiMax; }
  /// Return minimal phi angle requested
  Double_t phiMin() const            { return (Double_t)fPhiMin; }
  /// Return cross section
  Double_t xSection() const          { return (Double_t)fXSection; }
  /// Return requested number of events to generate
  UInt_t nEvents() const             { return fNEvents; }
  /// Return center-of-mass energy
  Double_t sqrtS() const;
  /// Return center-of-mass energy per nucleon
  Double_t nnSqrtS() const;
  /// Return energy of the projectile
  Double_t projectileEnergy() const;
  /// Return energy of the target
  Double_t targetEnergy() const;
  /// Return center-of-mass velocity
  Double_t betaCM() const;
  /// Return center-of-mass lorentz factor
  Double_t gammaCM() const;

  //
  // Setters
  //

  /// Set amount of event that was requested
  void setNEvents(const Int_t& nEvents)
  { if (nEvents<0) {fNEvents=0;}
    else { fNEvents = ( (nEvents > std::numeric_limits<int>::max() ) ?
			                  std::numeric_limits<unsigned int>::max() : (UInt_t)nEvents ); } }
  /// Set momentum of the projectile
  void setPProj(const Double_t& pProj)    { fPProj = (Float_t)pProj; }
  /// Set momentum of the target
  void setPTarg(const Double_t& pTarg)    { fPTarg = (Float_t)pTarg; }
  /// Set decayer type
  void setDecayer(const TString& decayer) { fDecayer = decayer; }

 private:
  /// Generator description
  TString    fGenerator;
  /// Run comment
  TString    fComment;
  /// Decayer description
  TString    fDecayer;
  /// Projectile mass number
  Short_t    fAProj;
  /// Projectile charge
  Short_t    fZProj;
  /// Projectile momentum per nucleon (GeV)
  Float_t    fPProj;
  /// Target mass number
  Short_t    fATarg;
  /// Target charge
  Short_t    fZTarg;
  /// Target momentum per nucleon (GeV)
  Float_t    fPTarg;
  /// Minimum impact parameter
  Float_t    fBMin;
  /// Maximum impact parameter
  Float_t    fBMax;
  /// Impact parameter weighting:
  /// \param 0 for geometrical weights (bdb)
  /// \param 1 for flat distribution
  Bool_t      fBWeight;
  /// Event plane minimum angle (rad)
  Float_t    fPhiMin;
  /// Event plane maximum angle (rad)
  Float_t    fPhiMax;
  /// Cross-section (mb)
  Float_t    fXSection;
  /// Requested number of events
  UInt_t     fNEvents;

#ifdef __ROOT__
  ClassDef(McRun, 1);
#endif
};

#endif // #define McRun_h
