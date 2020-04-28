/*
  Copyright (c) 2020 Nikita (arei) Ermakov <coffe92@gmail.com>
  SPDX-License-Identifier: MIT
*/

#include "McDstCut.h"

McDstCut::McDstCut()
{
  // We are using INFINITY IEEE 754.
  etaCut[0] = -INFINITY;
  ptCut[0] = -INFINITY;
  etaCut[1] = INFINITY;
  ptCut[1] = INFINITY;
}

McDstCut::McDstCut(const McDstCut &copy)
{
  etaCut[0] = copy.etaCut[0];
  etaCut[1] = copy.etaCut[1];
  ptCut[0] = copy.ptCut[0];
  ptCut[1] = copy.ptCut[1];
  pdgExclude = copy.pdgExclude;
}

void McDstCut::excludePdg(int pdg)
{
  pdgExclude.push_back(pdg);
}

void McDstCut::setEta(float lo, float hi)
{
  etaCut[0] = lo;
  etaCut[1] = hi;
}

void McDstCut::setEtaLow(float val)
{
  etaCut[0] = val;
}

void McDstCut::setEtaHigh(float val)
{
  etaCut[1] = val;
}

void McDstCut::setPt(float lo, float hi)
{
  ptCut[0] = lo;
  ptCut[1] = hi;
}

void McDstCut::setPtLow(float val)
{
  ptCut[0] = val;
}

void McDstCut::setPtHigh(float val)
{
  ptCut[1] = val;
}

bool McDstCut::isGoodParticle(const TLorentzVector &v, int pdg)
{
  if (v.Eta() > etaCut[0] && v.Eta() < etaCut[1] &&
      v.Pt() > ptCut[0] && v.Pt() < ptCut[1] &&
      checkExcludePdg(pdg) == false)
  {
    return true;
  }
  return false;
}

bool McDstCut::checkExcludePdg(int pdg)
{
  for (std::vector<int>::iterator i = pdgExclude.begin(); i != pdgExclude.end(); ++i)
  {
    if (*i == pdg)
      return true;
  }
  return false;
}
