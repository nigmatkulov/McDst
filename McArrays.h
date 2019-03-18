/**
 *  \class McArrays
 *  \brief Holds array number of arrays, their names and sizes
 *
 *  The McArrays class holds array names, number of arrays and
 *  array default sizes
 *
 *  \author Grigory Nigmatkulov
 */

#ifndef McArrays_h
#define McArrays_h

//________________
class McArrays {
 public:
  /// Default constructor
  McArrays();
  /// Destructor
  ~McArrays();

  /// Number of used arrays
  enum { NAllMcArrays = 2 };

  /// Names of the TBranches in the TTree/TFile
  static const char* mcArrayNames[NAllMcArrays];

  /// Names of the classes, the TClonesArrays are arrays of this type
  static const char* mcArrayTypes[NAllMcArrays];

  /// Maximum sizes of the TClonesArrays
  static int mcArraySizes[NAllMcArrays];

  /// Setup type indices
  enum typeIndex{ Event=0, Particle };
};

#endif // McArrays_h
