// McDst headers
#include "McPIDConverter.h"

// ROOT headers
#include "TROOT.h"
#include "TSystem.h"

// C++ headers
#include <fstream>

McPIDConverter* McPIDConverter::fgInstance = nullptr;

//_________________
McPIDConverter::McPIDConverter() : TNamed("PIDConverter", "Any-to-PDG particle-ID converter") {
  if (fgInstance) {
    Warning("McPIDConverter", "object already instantiated");
  }
  else {
    fgInstance = this;
    gROOT->GetListOfSpecials()->Add(this);
  }
}

//_________________
Int_t McPIDConverter::pdgCode(const Int_t& pid, const EConvention& pidType) {
  // If convension is PDG then do nothing and return the code
  if (pidType == ePDG) {
    return pid;
  }

  // In all other cases one needs to read
  ConversionTableMap::const_iterator mIt = fConversionTables.find(pidType);

  /* Either pidType has been set to an unknown value or the conversion table
     for specified convention hasn't been loaded yet. Assume the latter and
     try to do so now; if we still cannot find the table afterwards then
     something is indeed wrong. */
  if ( mIt == fConversionTables.end() ) {
    loadConversionTable(pidType);
    mIt = fConversionTables.find(pidType);
    if ( mIt == fConversionTables.end() ) {
      Error("GetPDGCode",
            "PID conversion table not available for convention=%u",
            pidType);
      return 0;
    }
  }
  
  const std::map<Int_t, Int_t>& conversionTable = mIt->second;
  std::map<Int_t, Int_t>::const_iterator pidIt = conversionTable.find(pid);
  if ( pidIt == conversionTable.end() ) {
    Warning("GetPDGCode", "PDG code not found for convention=%u, pid=%d",
            pidType, pid);
    return 0;
  }

  return pidIt->second;
}

//_________________
McPIDConverter* McPIDConverter::instance() {
  return (fgInstance) ? fgInstance : new McPIDConverter();
}

//_________________
void McPIDConverter::loadConversionTable(const EConvention& pidType) {
  std::string dataFileName = "/input/";
  switch (pidType) {
  case (ePluto):
    dataFileName += "pluto_pdg.dat";
    break;
  case (eUrQMD):
    dataFileName += "urqmd_pdg.dat";
    break;
  case (eWerner):
    // VENUS, NEXUS, EPOS
    dataFileName += "werner_pdg.dat";
    break;
  default:
    Error("LoadConversionTable",
	  "I do not know where to find conversion table for convention=%u",
	  pidType);
    return;
  }

  std::string unigenBase = gSystem->Getenv("MCDST");
  unigenBase += dataFileName;
  const char* fullFileName = unigenBase.c_str();
  std::ifstream fin;
  fin.open(fullFileName);
  if (! fin.good()) {
    Error("LoadConversionTable",
          "Failed to open conversion-table file %s", fullFileName);
    return;
  }

  std::map<Int_t, Int_t>& conversionTable = fConversionTables[pidType];
  Int_t localPid;
  Int_t pdgPid;
  while (1) {
    // FIXME: we might want to make this more robust against malformed input
    if (fin.eof())
      break;
    fin >> localPid >> pdgPid;
    conversionTable[localPid] = pdgPid;
  }
  fin.close();

  return;
}
