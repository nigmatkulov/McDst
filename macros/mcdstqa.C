/*
  Copyright (c) 2020 Eugenia Khyzhniak <eugenia.sh.el@gmail.com>, Nikita (arei) Ermakov <coffe92@gmail.com>
  SPDX-License-Identifier: MIT

  This is a simple example of using McDstQA class.
*/

#include "../McDstQA.h"

void mcdstqa(const char *ifile = "../test.mcDst.root", const char *ofile = "../qa_mcdst.root")
{
#if defined(ROOT_VERSION_CODE) && defined(ROOT_VERSION)
#  if ROOT_VERSION_CODE < ROOT_VERSION(6,0,0) // ROOT 5.
  gSystem->Load("../libMcDst.so");
#  else // ROOT 6.
  R__LOAD_LIBRARY(../libMcDst)
#  endif
#else
#  error "Could not find ROOT_VERSION_CODE or ROOT_VERSION macros."
#endif

  McDstQA qa(ifile, ofile);
  McDstCut cut;
  cut.excludePdg(-211); // exclude #pi^{-}. Just for example.
  qa.setMcDstCut(&cut);

  qa.run();
}
