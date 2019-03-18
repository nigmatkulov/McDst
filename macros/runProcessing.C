/**
 * \brief Helper macros for analyzing filename.uDst.root files using analyseUniGen.C
 *
 *  This macros takes inFileName argument with a femtoDst.root file
 *  or with a list of files (name.lis or name.list). It loads pre-compiled
 *  libUniGen.so (from unigen directory), compiles and executes a text
 *  analyseUniGen.C macro with passing inFileName to it, and
 *  cleans up the directory from the compilation products at the end.
 *
 *  Some details:
 *    inFileName - is a name of name.uDst.root file or a name
 *                 of a name.lis(t) files that contains a list of
 *                 name1.uDst.root files.
 *    NOTE: inFileName should contain either /absolutePath/inFileName
 *          or /relative2currentDir/inFileName
 *  It is assumed that analyseUniGen.C is placed in the same
 *  directory where the runProcessing.C is stored.
 *
 * \author Grigory Nigmatkulov
 * \date March 6, 2019
 */

// ROOT headers
#include "TROOT.h"
#include "TSystem.h"
#include "TString.h"

//_________________
void runProcessing(const Char_t *inFileName = "../data/filename.uDst.root",
                   const Char_t *oFileName = "oTest.root") {
  // Next line is not needed if you are not running in a standalone mode
  gSystem->Load("../libUniGen.so");
  TString str;
  str = ".x analyseUniGen.C+(\"";
  str += inFileName;
  str += "\",\"";
  str += oFileName;
  str += "\")";
  gROOT->ProcessLine( str.Data() );
  // Next line should be commented if you run several jobs at the same time
  gROOT->ProcessLine(".!rm -f analyseUniGen_C* ");
}
