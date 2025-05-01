/*
  Copyright (c) 2020 Nikita (arei) Ermakov <coffe92@gmail.com>
  SPDX-License-Identifier: MIT
*/

#ifndef McDstCut_h
#define McDstCut_h

// C++ headers
#include <vector>
#include <math.h>

// ROOT headers
#include <TObject.h>
#include <TLorentzVector.h>

//_________________
class McDstCut {
 public:
  /// Constructor
  McDstCut();
  /// Copy constructor
  McDstCut(const McDstCut &copy);

  /// Exclude particle from analysis
  void excludePdg(int pdg);
  /*
    lo -- low edge of the range.
    hi -- high edge of the range.
  */
  void setEta(float lo, float hi);
  void setEtaLow(float val);
  void setEtaHigh(float val);
  void setPt(float lo, float hi);
  void setPtLow(float val);
  void setPtHigh(float val);

  /*
    Check parameters of the particle.
  */
  bool isGoodParticle(const TLorentzVector &v, int pdg);

 private:
  std::vector<int> pdgExclude;
  /*
    [0] -- low edge of the range.
    [1] -- high edge of the range.
  */
  float etaCut[2];
  float ptCut[2];

  bool checkExcludePdg(int pdg);
};

#endif // #ifndef McDstCut_h
