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
#include "TLorentzVector.h"
#include "TString.h"

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
  /// Return event number
  Int_t GetEventNr() const           { return eventNr(); }
  /// Return imparct parameter (fm)
  Double_t b() const                 { return (Double_t)fB; }
  /// Return imparct parameter (fm)
  Double_t GetB() const              { return b(); }
  /// Return impact parameter (fm)
  Double_t impact() const            { return (Double_t)fB; }
  /// Return impact parameter (fm)
  Double_t GetImpact() const         { return impact(); }
  /// Return reaction plane angle
  Double_t phi() const               { return (Double_t)fPhi; }
  /// Return reaction plane angle
  Double_t GetPhi() const            { return phi(); }
  /// Return number of event steps
  Int_t numberOfSteps() const        { return (Int_t)fNes; }
  /// Return number of event steps
  Int_t GetNes() const               { return numberOfSteps(); }
  /// Return time step (stamp) number
  Int_t stepNumber() const           { return (Int_t)fStepNr; }
  /// Return time step (stamp) number
  Int_t GetStepNr() const            { return stepNumber(); }
  /// Return time of the step (stamp)
  Double_t stepT() const             { return (Double_t)fStepT; }
  /// Return time of the step (stamp)
  Double_t GetStepT() const          { return stepT(); }
  /// Return time of the step (stamp)
  Double_t stepTime() const          { return stepT(); }
  /// Return number of participants
  Int_t npart() const                { return (Int_t)fNpart; }
  /// Return number of participants
  Int_t GetNpart() const             { return npart(); }
  /// Return number of binary collisions
  Int_t ncoll() const                { return (Int_t)fNcoll; }
  /// Return number of binary collisions
  Int_t GetNcoll() const             { return ncoll(); }
  /// Return comment
  void comment(TString& comment) const { comment = fComment; }
  /// Return comment
  void GetComment(TString& comment) const { comment = fComment; }

  //
  // Setters
  //

  /// Set all event parameters
  void setParameters(const Int_t& eventNr, const Double_t& b, const Double_t& phi,
                     const Int_t& nes, const Int_t& stepNr, const Double_t& stepT,
                     const char* comment = "");
  /// Set all event parameters
  void SetParameters(const Int_t& eventNr, const Double_t& b, const Double_t& phi,
                     const Int_t& nes, const Int_t& stepNr, const Double_t& stepT,
                     const char* comment = "")
  { setParameters( eventNr, b, phi, nes, stepNr, stepT, comment ); }
  /// Set event number
  void setEventNr(const Int_t& eventNr)  { fEventNr = (UInt_t)eventNr; }
  /// Set event number
  void SetEventNr(const Int_t& eventNr)  { setEventNr(eventNr); }
  /// Set impact parameter
  void setB(const Double_t& b)           { fB = (Float_t)b; }
  /// Set impact parameter
  void SetB(const Double_t& b)           { setB( b ); }
  /// Set impact parameter
  void setImpact(const Double_t& impact) { setB( impact ); }
  /// Set reaction plane angle
  void setPhi(const Double_t& phi)       { fPhi = (Float_t)phi; }
  /// Set reaction plane angle
  void SetPhi(const Double_t& phi)       { setPhi( phi ); }
  /// Set number of steps
  void setNumberOfSteps(const Int_t& nes)
  { fNes = ( ( nes > std::numeric_limits<unsigned short>::max() ) ?
	           std::numeric_limits<unsigned short>::max() : (UShort_t)nes ); }
  /// Set number of steps
  void setNes(const Int_t& nes)
  { fNes = ( ( nes > std::numeric_limits<unsigned short>::max() ) ?
	     std::numeric_limits<unsigned short>::max() : (UShort_t)nes ); }
  /// Set number of steps
  void SetNes(const Int_t& nes)          { setNes(nes); }
  /// Set the current step number
  void setStepNr(const Int_t& stepNr)
  { fStepNr = ( ( stepNr > std::numeric_limits<unsigned short>::max() ) ?
		std::numeric_limits<unsigned short>::max() : (UShort_t)stepNr ); }
  /// Set the current step number
  void SetStepNr(const Int_t& stepNr)    { setStepNr(stepNr); }
  /// Set time of the current step
  void setStepT(const Double_t& stepT)   { fStepT = (Float_t)stepT; }
  /// Set time of the current step
  void setStepTime(const Double_t& time) { setStepT( time ); }
  /// Set comment
  void setComment(const char* comment)   { fComment = comment; }
  /// Set comment
  void SetComment(const char* comment)   { setComment( comment); }
  /// Set number of participants
  void setNpart(const Int_t& npart)      { fNpart = (Short_t)npart; }
  /// Set number of participants
  void SetNpart(const Int_t& npart)      { setNpart(npart); }
  /// Set number of binary collisions
  void setNcoll(const Int_t& ncoll)      { fNcoll = (Short_t)ncoll; }
  /// Set number of binary collisions
  void SetNcoll(const Int_t& ncoll)      { setNcoll(ncoll); }

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
