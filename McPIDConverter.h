/**
 * \class McPIDConverter
 * \brief Converts particle PID code from generator to the PDG standard code
 *
 * The class allows a conversion of the particle PID from the
 * generator encoding to the PDG ones
 */

#ifndef McPIDConverter_h
#define McPIDConverter_h

// C++ headers
#include <map>

// ROOT headers
#include "TNamed.h"

//_________________
class McPIDConverter : public TNamed {

 public:

  /// Generator PID table indices (enumeration: PDG=-1, Pluto=0, UrQMD=1, Werner(VENUS, NEXUS, EPOS)=2)
  typedef enum {
    ePDG = -1,
    ePluto,
    eUrQMD,
    eWerner
  } EConvention;

  /// Default constructor
  McPIDConverter();
  /// Return particle PDG code
  Int_t pdgCode(const Int_t& pid, const EConvention& pidType);
  /// Return a pointer to the instance
  static McPIDConverter* instance();

 private:

  // We should use 'EConvention' as the map key type, however this confused rootcint.
  // For the time being use UInt_t instead, hopefully fix this later (FIXME).
  typedef std::map<UInt_t, std::map<Int_t, Int_t> > ConversionTableMap;

  /// Load table to decode
  void loadConversionTable(const EConvention& pidType);

  /// Conversion table map
  ConversionTableMap fConversionTables;
  /// Instance of the McPIDConverter which can be achieved at any time
  static McPIDConverter* fgInstance;

#ifdef __ROOT__
  ClassDef(McPIDConverter, 0);
#endif
  
};

#endif // #define McPIDConverter_h
