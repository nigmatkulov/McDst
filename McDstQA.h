/*
  Copyright (c) 2020 Eugenia Khyzhniak <eugenia.sh.el@gmail.com>, Nikita (arei) Ermakov <coffe92@gmail.com>
  SPDX-License-Identifier: MIT
*/

#ifndef MCDSTQA_H
#define MCDSTQA_H

// C/C++ headers.
#include <iostream>
#include <string>
#include <map>
#include <assert.h>

// ROOT headers.
#include "TObject.h"
#include "TFile.h"
#include "TTree.h"
#include "TMath.h"
#include "TClonesArray.h"
#include "TLorentzVector.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMatrixDSymEigen.h"
#include "TMatrixTLazy.h"

// McDst headers.
#include "McDstReader.h"
#include "McDstCut.h"
#include "McParticle.h"
#include "McEvent.h"

class McDstQA : public TObject
{
public:
  McDstQA(const char *input_file, const char *output_file);
  ~McDstQA();

  void setMcDstCut(McDstCut *ptr);

  /*
    Event QA histograms.  We are expose these histograms to make it
    possible to change limits/binning before `::run()`.
  */
  TH1F *hImpactPar;
  TH1F *hRefMult05, *hRefMult10;
  TH1F *hNTracks;
  TH1F *hSph05, *hSph10; // Transverse sphericity.

  /*
    Track QA histograms.
  */
  TH1F *hPx, *hPy, *hPz;
  TH1F *hPdg;
  TH2F *hMSqrVsP;

  void run(int nev = -1);
private:
  McDstReader *reader;
  TFile *ofile;
  McDstCut *cut;
  int nEvents;

  ClassDef(McDstQA, 1);
};

#endif // #ifndef MCDSTQA_H
