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
  int eventNr() const              { return fEventNr; }
  /// Return imparct parameter (fm)
  double b() const                 { return (double)fB; }
  /// Return impact parameter (fm)
  double impact() const            { return (double)fB; }
  /// Return reaction plane angle
  double phi() const               { return (double)fPhi; }
  /// Return number of event steps
  int numberOfSteps() const        { return (int)fNes; }
  /// Return time step (stamp) number
  int stepNumber() const           { return (int)fStepNr; }
  /// Return time of the step (stamp)
  double stepT() const             { return (double)fStepT; }
  /// Return time of the step (stamp)
  double stepTime() const          { return stepT(); }
  /// Return number of participants
  int npart() const                { return (int)fNpart; }
  /// Return number of binary collisions
  int ncoll() const                { return (int)fNcoll; }
  /// Return comment
  void comment(TString& comment) const { comment = fComment; }

  //
  // Setters
  //

  /// Set all event parameters
  void setParameters(const int& eventNr, const double& b, const double& phi,
                     const int& nes, const int& stepNr, const double& stepT,
                     const char* comment = "");
  /// Set event number
  void setEventNr(const int& eventNr)  { fEventNr = (UInt_t)eventNr; }
  /// Set impact parameter
  void setB(const double& b)           { fB = (Float_t)b; }
  /// Set reaction plane angle
  void setPhi(const double& phi)       { fPhi = (Float_t)phi; }
  /// Set number of steps
  void setNumberOfSteps(const int& nes)
  { fNes = ( ( nes > std::numeric_limits<unsigned short>::max() ) ?
	     std::numeric_limits<unsigned short>::max() : (UShort_t)nes ); }
  /// Set number of steps
  void setNes(const int& nes)
  { fNes = ( ( nes > std::numeric_limits<unsigned short>::max() ) ?
	     std::numeric_limits<unsigned short>::max() : (UShort_t)nes ); }
  /// Set the current step number
  void setStepNr(const int& stepNr)
  { fStepNr = ( ( stepNr > std::numeric_limits<unsigned short>::max() ) ?
		std::numeric_limits<unsigned short>::max() : (UShort_t)stepNr ); }
  /// Set time of the current step
  void setStepT(const double& stepT)   { fStepT = (Float_t)stepT; }
  /// Set time of the current step
  void setStepTime(const double& time) { setStepT( time ); }
  /// Set comment
  void setComment(const char* comment)   { fComment = comment; }
  /// Set number of participants
  void setNpart(const int& npart)      { fNpart = (Short_t)npart; }
  /// Set number of binary collisions
  void setNcoll(const int& ncoll)      { fNcoll = (Short_t)ncoll; }

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

  ClassDef(McEvent, 2);
};

#endif // #define McEvent_h
