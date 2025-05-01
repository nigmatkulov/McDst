/**
 * \class McEvent
 * \brief Holds information about event
 *
 * The class keeps the information about generated event
 */

#ifndef McEvent_h
#define McEvent_h

// C++ headers
#include <limits>

// ROOT headers
#include "TObject.h"
#include "TString.h"
#include "TLorentzVector.h"

// Forward declarations
class TString;
class TClonesArray;

//_________________
class McEvent : public TObject {

 public:
  /// Default constructor
  McEvent();
  /// Copy constructor
  McEvent(const McEvent& right);
  /// Default destructor
  virtual ~McEvent();
  /// Print event information
  virtual void print() const;
  /// Print event information
  virtual void Print(Option_t* option = "") const;

  //
  // Getters
  //

  /// Return event number
  Int_t eventNr() const              { return fEventNr; }
  /// Return imparct parameter (fm)
  Double_t b() const                 { return (Double_t)fB; }
  /// Return impact parameter (fm)
  Double_t impact() const            { return (Double_t)fB; }
  /// Return reaction plane angle
  Double_t phi() const               { return (Double_t)fPhi; }
  /// Return number of event steps
  Int_t numberOfSteps() const        { return (Int_t)fNes; }
  /// Return time step (stamp) number
  Int_t stepNumber() const           { return (Int_t)fStepNr; }
  /// Return time of the step (stamp)
  Double_t stepT() const             { return (Double_t)fStepT; }
  /// Return time of the step (stamp)
  Double_t stepTime() const          { return stepT(); }
  /// Return number of participants
  Int_t npart() const                { return (Int_t)fNpart; }
  /// Return number of binary collisions
  Int_t ncoll() const                { return (Int_t)fNcoll; }
  /// Return comment
  void comment(TString& comment) const { comment = fComment; }

  //
  // Setters
  //

  /// Set all event parameters
  void setParameters(const Int_t& eventNr, const Double_t& b, const Double_t& phi,
                     const Int_t& nes, const Int_t& stepNr, const Double_t& stepT,
                     const char* comment = "");
  /// Set event number
  void setEventNr(const Int_t& eventNr)  { fEventNr = (UInt_t)eventNr; }
  /// Set impact parameter
  void setB(const Double_t& b)           { fB = (Float_t)b; }
  /// Set reaction plane angle
  void setPhi(const Double_t& phi)       { fPhi = (Float_t)phi; }
  /// Set number of steps
  void setNumberOfSteps(const Int_t& nes)
  { fNes = ( ( nes > std::numeric_limits<unsigned short>::max() ) ?
	     std::numeric_limits<unsigned short>::max() : (UShort_t)nes ); }
  /// Set number of steps
  void setNes(const Int_t& nes)
  { fNes = ( ( nes > std::numeric_limits<unsigned short>::max() ) ?
	     std::numeric_limits<unsigned short>::max() : (UShort_t)nes ); }
  /// Set the current step number
  void setStepNr(const Int_t& stepNr)
  { fStepNr = ( ( stepNr > std::numeric_limits<unsigned short>::max() ) ?
		std::numeric_limits<unsigned short>::max() : (UShort_t)stepNr ); }
  /// Set time of the current step
  void setStepT(const Double_t& stepT)   { fStepT = (Float_t)stepT; }
  /// Set time of the current step
  void setStepTime(const Double_t& time) { setStepT( time ); }
  /// Set comment
  void setComment(const char* comment)   { fComment = comment; }
  /// Set number of participants
  void setNpart(const Int_t& npart)      { fNpart = (Short_t)npart; }
  /// Set number of binary collisions
  void setNcoll(const Int_t& ncoll)      { fNcoll = (Short_t)ncoll; }

 private:
  /// Event number
  UInt_t fEventNr;
  /// Impact parameter (fm)
  Float_t fB;
  /// Reaction plane angle
  Float_t fPhi;
  /// Number of event steps
  UShort_t fNes;
  // Event step number
  UShort_t fStepNr;
  /// Event step time
  Float_t fStepT;
  /// Generator-specific information
  TString fComment;
  /// Number of participants (-1: not set)
  Short_t fNpart;
  /// Number of binary collisions (-1: not set)
  Short_t fNcoll;

#ifdef __ROOT__
  ClassDef(McEvent, 2);
#endif
};

#endif // #define McEvent_h
