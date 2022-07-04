/*
  vim:et:sw=2:

  Copyright (c) 2020 Nikita (sh1r4s3) Ermakov <sh1r4s3@mail.si-head.nl>
  SPDX-License-Identifier: MIT

  This code is based on pythia8gen.cpp
*/

#define MAX_LINE_LENGTH 0xff

// getopt
#include <unistd.h>
#include <getopt.h>

// srand/rand
#include <stdlib.h>

#include <assert.h>

// C++ headers
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <map>
#include <cstdint>

// ROOT headers
#include <TObject.h>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TBranch.h>
#include <TMath.h>
#include <TClonesArray.h>
#include <Compression.h>
#include <TLorentzVector.h>

// McDst headers
#include "McRun.h"
#include "McEvent.h"
#include "McParticle.h"
#include "McPIDConverter.h"
#include "McArrays.h"
#include "McDstCut.h"

// There is only one namespace is used. So make it default.
using namespace std;

/*
  Emit error message and exit. Args:
  * doexit - if 0 then do not exit, else exit from a program.
  * format - printf-like format string.
  * ... - arguments for format string.
  There is no need to add a new line character explicitly at the end of the
  format string.
*/
#define ERR(doexit, format, ...)                                        \
  { fprintf(stderr, "oscar2013.cpp:%d / errno=%d / " format "\n", __LINE__, errno, ##__VA_ARGS__); \
    if (doexit) exit(EXIT_FAILURE); }                                   \

#define PROGNAME "oscar2013"
#define VERSION "1.0"
#define OFILE_DEFAULT "out_oscar2013.root"

// Options for getopt.
struct option longopts[] =
{
  { .name = "help", .has_arg = 0, .flag = 0, .val = 'h' },
  { .name = "ifname", .has_arg = 1, .flag = 0, .val = 'i' },
  { .name = "ofname", .has_arg = 1, .flag = 0, .val = 'o' },
  { .name = "events", .has_arg = 1, .flag = 0, .val = 'e' },
  { .name = "compression-level", .has_arg = 1, .flag = 0, .val = 0xFF01 },
  { .name = "compression-algo", .has_arg = 1, .flag = 0, .val = 0xFF02 },
  { 0, 0, 0, 0 }
};

// Output help string.
void
help_me()
{
  const char *hstr =                                                    \
    PROGNAME " v" VERSION " Pythia 8 McDst generator\n"                 \
    "Usage: " PROGNAME " [Options]\n"                                   \
    "Options:\n\
    -h, --help                        help\n\
    -i, --ifname <filename>           input oscar 2013 file\n\
    -e, --events <number of events>   number of events to read\n\
    -o, --ofname <filename>           output file (default: " OFILE_DEFAULT ")\n\
    --compression-level <level>       set compression level to <level>.\n\
                                      valid levels are from 1 (low) 9 (high).\n\
    --compression-algo <aglorithm>    set compression algorithm to <algorithm>\n\
                                      possible values are zlib, lz4 (only for ROOT > 6), lzma\n";

  cout << hstr;
  exit(EXIT_SUCCESS);
}

/*
  Simple hash function to compare 4 bytes char arrays.  If char array
  are more than 4 bytes long it could results in hash collisions.
*/
uint32_t
hash4(char *s)
{
  uint32_t hash = 0;

  while (*s++)
    hash = (hash << 8) | (uint32_t)*s;
  return hash;
}

int
main(int argc, char *argv[])
{
  const char optstring[] = "hi:o:e:"; // This string must be sync with a struct option array.
  char line[MAX_LINE_LENGTH];
  int opt, nev;
  char *ifname = 0;
  char *ofname = OFILE_DEFAULT; // FIXME: ISO C++ forbids converting a string constant to ‘char*’
  TFile *ofile = 0;
  FILE *ifile;
  TTree *tree = 0;
#if defined(ROOT_VERSION_CODE) && defined(ROOT_VERSION)
#  if ROOT_VERSION_CODE < ROOT_VERSION(6,0,0) // ROOT 5.
  int ofnameCompLevel = 7;
#  else // ROOT 6.
  int ofnameCompLevel = ROOT::RCompressionSetting::ELevel::kDefaultLZMA;
#  endif
#else
#  error "Could not find ROOT_VERSION_CODE or ROOT_VERSION macros."
#endif
  int ofnameCompAlgo = ROOT::kLZMA; // Use LZMA by default.
  int treeAutoSave = -(4 << 20); /* Do auto save each 4 MB == 4^20 Bytes.
                                    Minus stands for bytes limit rather than number of entries. */
  TClonesArray *mcArrays[McArrays::NAllMcArrays];
  // Precalculated hashes.
  const uint32_t lzma = 2053988608; // hash4("lzma")
  const uint32_t zlib = 1818845696; // hash4("zlib")
  const uint32_t lz4 = 8008704; // hash4("lz4")
  // Preselection cut class.
  McDstCut cut;

  // Parse command line arguments.
  if (argc <= 1)
    ERR(1, "For the help message try: %s --help", PROGNAME);

  while ((opt = getopt_long(argc, argv, optstring, longopts, 0)) != -1)
  {
    switch (opt)
    {
    case 'h':
      help_me();
      break;
    case 'i':
      ifname = optarg;
      break;
    case 'o':
      ofname = optarg;
      break;
    case 'e':
      nev = std::stoi(optarg);
      break;
    case 0xFF01:
      ofnameCompLevel = std::stoi(optarg);
      std::cout << "ofnameCompLevel = " << ofnameCompLevel << std::endl;
      break;
    case 0xFF02:
      switch (hash4(optarg))
      {
      case lzma:
        ofnameCompLevel = ROOT::kLZMA;
        break;
      case zlib:
        ofnameCompLevel = ROOT::kZLIB;
        break;
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,0,0) // ROOT 5.
      case lz4:
        ofnameCompLevel = ROOT::kLZ4;
        break;
#endif
      default:
        std::cout << "Warning: there is no support for " << optarg << " compression algorithm"
                  << "\nWarning: fallback to the lzma!\n";
        ofnameCompLevel = ROOT::kLZMA;
      }
      break;
    default:
      ERR(1, "Wrong option: %c", (char)opt);
    }
  }

  // Output file
  ofile = new TFile(ofname, "RECREATE");
  if (!ofile)
    ERR(1, "cannot open output file");
  ofile->SetCompressionLevel(ofnameCompLevel);
  ofile->SetCompressionLevel(ofnameCompAlgo);

  // Input file
  ifile = fopen(ifname, "r");
  if (!ifile)
  {
    ofile->Close();
    ERR(1, "cannot open input file");
  }

  // Setting up McDst.
  McEvent::Class()->IgnoreTObjectStreamer();
  McParticle::Class()->IgnoreTObjectStreamer();
  McRun::Class()->IgnoreTObjectStreamer();
  tree = new TTree("McDst", "Oscar 2013 tree");
  if (!tree)
  {
    ofile->Close();
    fclose(ifile);
    ERR(1, "cannot create McDst TTree");
  }
  tree->SetAutoSave(treeAutoSave);
  for (int i = 0; i < McArrays::NAllMcArrays; ++i)
  { // Create arrays.
    mcArrays[i] = new TClonesArray(McArrays::mcArrayTypes[i], McArrays::mcArraySizes[i]);
    // Set branch.
    tree->Branch(McArrays::mcArrayNames[i], &mcArrays[i]);
  }

  // Read first three lines.
  for (int i = 0; i < 3; ++i)
  {
    fgets(line, MAX_LINE_LENGTH, ifile);
  }

  // Main loop.
  for (int iev = 0; iev < nev; ++iev)
  {
    // Get number of tracks.
    int tmp, ntrk;
    float impact;
    fscanf(ifile, "# event %d out %d\n", &tmp, &ntrk);

    // Clear all arrays.
    for (int i = 0; i < McArrays::NAllMcArrays; mcArrays[i++]->Clear());
    // Set McEvent. FIXME: check nullptr.
    // Fill an event after the loop over tracks.
    TClonesArray *mcEvCol = mcArrays[McArrays::Event];
    McEvent *mcEv = new ((*mcEvCol)[mcEvCol->GetEntries()]) McEvent();

    // Set McTrack.
    for (int itrk = 0; itrk < ntrk; ++itrk)
    {
      /*
       * This format string was taken from the
       * https://www.github.com/smas-transport/smash f19d6f3b
       * src/oscaroutput.cc:718
       */
      float t;    // -+  
      float x;    //  | 4-vector (space-time) of the particle
      float y;    //  |
      float z;    // -+
      float mass;
      float e;  // -+
      float px; //  | 4-vector (momentum space) of the particle
      float py; //  |
      float pz; // -+
      char spdg[MAX_LINE_LENGTH];
      int pdg;
      int id; // particle id
      int charge;
      int col_per_part; // number of collisions the particle has undergone
      float form_time; // formation time of the particle
      float xscale; // cross section scaling factor
      int id_process;
      int h_processtype; // type of the last process the particle has undergone
      float time_last_col;
      char sp1[MAX_LINE_LENGTH]; // parent 1
      char sp2[MAX_LINE_LENGTH]; // parent 2
      int p1, p2;

      fscanf(ifile,
             "%g %g %g %g %g "
             "%g %g %g %g "
             "%s %i %i %i "
             "%g %g %i "
             "%i %g %s %s\n",
             &t, &x, &y, &z, &mass,
             &e, &px, &py, &pz,
             spdg, &id, &charge, &col_per_part,
             &form_time, &xscale, &id_process,
             &h_processtype, &time_last_col, sp1, sp2);

      pdg = std::stoi(spdg);
      p1 = std::stoi(sp1);
      p2 = std::stoi(sp2);

      TLorentzVector momentum(px, py, pz, e);
      TLorentzVector coord(x, y, z, t);
      // r0 = r - v*(t - t0), where
      // r0 - kinetic freeze-out 3-coord
      // v  - speed
      // r  - current 3-corrd
      // t  - time at r
      // t0 - time at kinetic freeze-out (time_last_col)
      TVector3 freezeout = coord.Vect() - momentum.Vect()*(1/momentum.E())*(coord.T() - time_last_col);

      // Check particle cut.
      if (!cut.isGoodParticle(momentum, pdg))
      {
        continue;
      }

      // Add new track. FIXME: check nullptr.
      TClonesArray *mcTrkCol = mcArrays[McArrays::Particle];
      // Workaround for useless T& constructor parameters
      int status = 0;
      int none = -1;
      int child[2] = {-1};
      p1 = p2 ? -1 : p1;
      new ((*mcTrkCol)[mcTrkCol->GetEntries()]) McParticle(itrk,
                                                           pdg,
                                                           status,
                                                           p1, // parent id
                                                           none, // decayed parent id
                                                           none, // mate
                                                           none, // decay id
                                                           child, // child
                                                           px, py, pz, e,
                                                           freezeout.X(),
                                                           freezeout.Y(),
                                                           freezeout.Z(),
                                                           time_last_col);
    }

    // Fill an event.
    fscanf(ifile, "# event %d end 0 impact %f empty %s\n", &tmp, &impact, line);
    mcEv->setEventNr(iev);
    mcEv->setB(impact);
    mcEv->setPhi(0.0);
    mcEv->setNes(0);
    mcEv->setComment(0);
    mcEv->setStepNr(0);
    mcEv->setStepT(0);

    // Add an event to DST.
    tree->Fill();
  }

  ofile->Write();
  ofile->Close();
  return EXIT_SUCCESS;
}
