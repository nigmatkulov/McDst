/*
  Copyright (c) 2019-2020 Nikita (arei) Ermakov <coffe92@gmail.com>
  SPDX-License-Identifier: MIT
*/

// getopt.
#include <unistd.h>
#include <getopt.h>

// srand/rand
#include <stdlib.h>

#include <assert.h>

// C++ headers.
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <map>
#include <cstdint>

// ROOT headers.
#include "TObject.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TBranch.h"
#include "TMath.h"
#include "TClonesArray.h"
#include "Compression.h"
#include "TLorentzVector.h"

// McDst headers.
#include "McRun.h"
#include "McEvent.h"
#include "McParticle.h"
#include "McPIDConverter.h"
#include "McArrays.h"
#include "McDstCut.h"

// Pythia 8 headers.
#include "Pythia8/Pythia.h"
#include "Pythia8/HeavyIons.h"

/*
  Emit error message and exit. Args:
  * doexit - if 0 then do not exit, else exit from a program.
  * format - printf-like format string.
  * ... - arguments for format string.
  There is no need to add a new line character explicitly at the end of the
  format string.
*/
#define ERR(doexit, format, ...)                                        \
  { fprintf(stderr, "pythia8gen.cpp:%d / errno=%d / " format "\n", __LINE__, errno, ##__VA_ARGS__); \
    if (doexit) exit(EXIT_FAILURE); }                                   \

#define PROGNAME "pythia8gen"
#define VERSION "1.0"
#define OFILE_DEFAULT "out_pythia8.root"
#define XMLDOC_DEFAULT "../share/Pythia8/xmldoc" /* see Pythia8/Pythia.h */

// Options for getopt.
struct option longopts[] =
{
  { .name = "help", .has_arg = 0, .flag = 0, .val = 'h' },
  { .name = "pythia-option", .has_arg = 1, .flag = 0, .val = 's' },
  { .name = "ifile", .has_arg = 1, .flag = 0, .val = 'i' },
  { .name = "ofile", .has_arg = 1, .flag = 0, .val = 'o' },
  { .name = "rnd", .has_arg = 1, .flag = 0, .val = 'r' },
  { .name = "compression-level", .has_arg = 1, .flag = 0, .val = 0xFF01 },
  { .name = "compression-algo", .has_arg = 1, .flag = 0, .val = 0xFF02 },
  { .name = "all-particles", .has_arg = 0, .flag = 0, .val = 0xFF03 },
  { .name = "xmldoc-dir", .has_arg = 1, .flag = 0, .val = 0xFF04 },
  { .name = "pt-low", .has_arg = 1, .flag = 0, .val = 0xFF05 },
  { .name = "pt-high", .has_arg = 1, .flag = 0, .val = 0xFF06 },
  { .name = "eta-low", .has_arg = 1, .flag = 0, .val = 0xFF07 },
  { .name = "eta-high", .has_arg = 1, .flag = 0, .val = 0xFF08 },
  { .name = "exclude-pdg", .has_arg = 1, .flag = 0, .val = 0xFF09 },
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
    -h, --help                      help\n\
    -s, --pythia-option <OPTION>    pass an option to Pythia8\n\
    -i, --ifile <FILE NAME>         input Pythia8 configuration file\n\
    -o, --ofile <FILE NAME>         output file (default: " OFILE_DEFAULT ")\n\
    -r, --rnd <SEED>                override random seed\n\
    --compression-level <LEVEL>     Set compression level to <LEVEL>.\n\
                                    Valid levels are from 1 (low) 9 (high).\n\
    --compression-algo <ALGORITHM>  Set compression algorithm to <ALGORITHM>\n\
                                    Possible values are zlib, lz4 (only for ROOT > 6), lzma\n\
    --all-particles                 Save decayed/branched/fragmented/... particles too\n\
    --xmldoc-dir <DIR>              Path to the xmldoc directory (default: " XMLDOC_DEFAULT "\n\
    --pt-low <low pt cut>            Low edge of the p_{t} cut.\n\
    --pt-high <high pt cut>          High edge of the p_{t} cut.\n\
    --eta-low <low eta cut>         Low edge of the pseudorapidity cut.\n\
    --eta-high <high eta cut>       High edge of the pseudorapidity cut.\n\
    --exclude-pdg <pdg code>        Exclude <pdg code>. Could be defined multiple times\n\
                                    to exclude multiple particles.\n";

  std::cout << hstr;
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
  int onlyFinal = 1;
  const char optstring[] = "hs:i:o:n:"; // This string must be sync with a struct option array.
  int opt, nev;
  char *rnd = 0;
  char *ifile = 0;
  char *ofile = OFILE_DEFAULT; // FIXME: ISO C++ forbids converting a string constant to ‘char*’
  char *xmldoc = XMLDOC_DEFAULT; // FIXME: ISO C++ forbids converting a string constant to ‘char*’
  std::vector<std::string> pills;
  TFile *outFile = 0;
  TTree *tree = 0;
#if defined(ROOT_VERSION_CODE) && defined(ROOT_VERSION)
#  if ROOT_VERSION_CODE < ROOT_VERSION(6,0,0) // ROOT 5.
  int ofileCompLevel = 7;
#  else // ROOT 6.
  int ofileCompLevel = ROOT::RCompressionSetting::ELevel::kDefaultLZMA;
#  endif
#else
#  error "Could not find ROOT_VERSION_CODE or ROOT_VERSION macros."
#endif
  int ofileCompAlgo = ROOT::kLZMA; // Use LZMA by default.
  int treeAutoSave = -(4 << 20); /* Do auto save each 4 MB == 4^20 Bytes.
                                    Minus stands for bytes limit rather than number of entries. */
  TClonesArray *mcArrays[McArrays::NAllMcArrays];
  // Precalculated hashes.
  const uint32_t lzma = 2053988608; // hash4("lzma")
  const uint32_t zlib = 1818845696; // hash4("zlib")
  const uint32_t lz4 = 8008704; // hash4("lz4")
  // Preselection cut class.
  McDstCut cut;
  // Heavy ion collision class.
  Pythia8::Angantyr *hion = nullptr;

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
    case 's':
      pills.push_back(optarg);
      break;
    case 'i':
      ifile = optarg;
      break;
    case 'o':
      ofile = optarg;
      break;
    case 'r':
      rnd = optarg;
      break;
    case 0xFF01:
      ofileCompLevel = std::stoi(optarg);
      std::cout << "ofileCompLevel = " << ofileCompLevel << std::endl;
      break;
    case 0xFF02:
      switch (hash4(optarg))
      {
      case lzma:
        ofileCompLevel = ROOT::kLZMA;
        break;
      case zlib:
        ofileCompLevel = ROOT::kZLIB;
        break;
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,0,0) // ROOT 5.
      case lz4:
        ofileCompLevel = ROOT::kLZ4;
        break;
#endif
      default:
        std::cout << "Warning: there is no support for " << optarg << " compression algorithm"
                  << "\nWarning: fallback to the lzma!\n";
        ofileCompLevel = ROOT::kLZMA;
      }
      break;
    case 0xFF03:
      onlyFinal = 0;
      break;
    case 0xFF04:
      xmldoc = optarg;
      break;
    case 0xFF05:
      cut.setPtLow(std::stof(optarg));
      break;
    case 0xFF06:
      cut.setPtHigh(std::stof(optarg));
      break;
    case 0xFF07:
      cut.setEtaLow(std::stof(optarg));
      break;
    case 0xFF08:
      cut.setEtaHigh(std::stof(optarg));
      break;
    case 0xFF09:
      cut.excludePdg(std::stoi(optarg));
      break;
    default:
      ERR(1, "Wrong option: %c", (char)opt);
    }
  }

  // Initialize Pythia 8.
  Pythia8::Pythia pythia(xmldoc, false);
  /*
    One pill makes you larger
    And one pill makes you small
    And the ones that mother gives you
    Don't do anything at all
    Go ask Alice
    When she's ten feet tall

    Choose pills with a caution!
    But not forget what Dormouse said.
    Feed your head!
  */
  for (std::vector<std::string>::iterator pill = pills.begin(); pill != pills.end(); ++pill)
  {
    pythia.readString(pill->c_str());
  }
  pythia.readFile(ifile);
  pythia.readString("Random:setSeed = on");
  std::string sseed = "Random:seed = ";
  if (rnd)
  { // Set random seed manually.
    sseed += rnd;
  }
  else
  { // Get random seed from time.
    srand(time(NULL));
    int seed = int(((float)rand()/RAND_MAX)*900000000.); // 900000000 is a maximum possible seed in Pythia 8.
    sseed += std::to_string(seed);
  }
  pythia.readString(sseed);

  // Setting up Pythia 8.
  pythia.init();
  // If beams are not nuclei then do not set Agantyr
  if (pythia.beamA.id() >= (int)1e9 && pythia.beamB.id() >= (int)1e9)
  {
    hion = (Pythia8::Angantyr *)pythia.getHeavyIonsPtr();
    if (!hion)
        ERR(1, "cannot work with Agantyr model");
  }

  // Setting up McDst.
  outFile = new TFile(ofile, "RECREATE");
  if (!outFile)
    ERR(1, "cannot open output file");
  outFile->SetCompressionLevel(ofileCompLevel);
  outFile->SetCompressionLevel(ofileCompAlgo);
  McEvent::Class()->IgnoreTObjectStreamer();
  McParticle::Class()->IgnoreTObjectStreamer();
  McRun::Class()->IgnoreTObjectStreamer();
  tree = new TTree("McDst", "Pythia 8 tree");
  if (!tree)
  {
    outFile->Close();
    ERR(1, "cannot create McDst TTree");
  }
  tree->SetAutoSave(treeAutoSave);
  for (int i = 0; i < McArrays::NAllMcArrays; ++i)
  { // Create arrays.
    mcArrays[i] = new TClonesArray(McArrays::mcArrayTypes[i], McArrays::mcArraySizes[i]);
    // Set branch.
    tree->Branch(McArrays::mcArrayNames[i], &mcArrays[i]);
  }

  // Main loop.
  nev = pythia.mode("Main:numberOfEvents");
  for (int iev = 0; iev < nev; ++iev)
  {
    // Generate event.
    pythia.next();
    // Clear all arrays.
    for (int i = 0; i < McArrays::NAllMcArrays; mcArrays[i++]->Clear());
    // Get event.
    const Pythia8::Event &ev = pythia.event;
    // Set McEvent. FIXME: check nullptr.
    TClonesArray *mcEvCol = mcArrays[McArrays::Event];
    McEvent *mcEv = new ((*mcEvCol)[mcEvCol->GetEntries()]) McEvent();
    mcEv->setEventNr(iev);
    mcEv->setB(hion ? hion->hiinfo.b() : 0.);
    mcEv->setPhi(0.0); // 0 in Pythia 8.
    mcEv->setNes(0);
    mcEv->setComment(0);
    mcEv->setStepNr(0);
    mcEv->setStepT(0);

    // Set McTrack.
    int ntrk = ev.size();
    for (int itrk = 0; itrk < ntrk; ++itrk)
    {
      int status = ev[itrk].status();
      int decay = -1; /* -1 means not decayed, see McParticle.h for
                          more details. But there is no more
                          details... :( */
      int parent = 0;
      int parent_decay = 0;
      int mate = 0;
      int child[2] = {0};

      // Skip beam particles.
      if (abs(status) < 19)
        continue;

      // Optionally do skip not final particles.
      if (status < 0)
      {
        if (onlyFinal)
          continue;
        decay = 0;
      }

      /*
        From the Pythia 8 docs (this information should be here[1]):

        1. mother1 = mother2 = 0: for lines 0 - 2, where line 0
        represents the event as a whole, and 1 and 2 the two
        incoming beam particles;

        2. mother1 = mother2 > 0: the particle is a "carbon copy" of
        its mother, but with changed momentum as a "recoil" effect,
        e.g. in a shower;

        3. mother1 > 0, mother2 = 0: the "normal" mother case, where
        it is meaningful to speak of one single mother to several
        products, in a shower or decay;

        4. mother1 < mother2, both > 0, for abs(status) = 81 - 86:
        primary hadrons produced from the fragmentation of a string
        spanning the range from mother1 to mother2, so that all
        partons in this range should be considered mothers; and
        analogously for abs(status) = 101 - 106, the formation of
        R-hadrons;

        5. mother1 < mother2, both > 0, except case 4: particles
        with two truly different mothers, in particular the
        particles emerging from a hard 2 → n interaction.

        6. mother2 < mother1, both > 0: particles with two truly
        different mothers, notably for the special case that two
        nearby partons are joined together into a status 73 or 74
        new parton, in the g + q → q case the q is made first mother
        to simplify flavour tracing.

        We are consider only "normal" mother case (3).

        [1] http://home.thep.lu.se/~torbjorn/pythia82html/Welcome.html
      */
      int mother1 = ev[itrk].mother1();
      int mother2 = ev[itrk].mother2();
      if (mother1 > 0 && mother2 == 0)
        parent = mother1;
      else
        continue;

      /*
        This was taken from the same source as for mothers description:
        1. daughter1 = daughter2 = 0: there are no daughters (so far);

        2. daughter1 = daughter2 > 0: the particle has a "carbon copy"
        as its sole daughter, but with changed momentum as a "recoil"
        effect, e.g. in a shower;

        3. daughter1 > 0, daughter2 = 0: each of the incoming beams
        has only (at most) one daughter, namely the initiator parton
        of the hardest interaction; further, in a 2 → 1 hard
        interaction, like q qbar → Z^0, or in a clustering of two
        nearby partons, the initial partons only have this one
        daughter;

        4. daughter1 < daughter2, both > 0: the particle has a range
        of decay products from daughter1 to daughter2;

        5. daughter2 < daughter1, both > 0: the particle has two
        separately stored decay products (e.g. in backwards evolution
        of initial-state showers).

        We are consider only forth case where:
        mother -> daughter1 + ... + daughter2
        here, daughter2 - daughter1 must equals to 1. In that case
        only 2 daughter are produced.
      */
      int daughter1 = ev[itrk].daughter1();
      int daughter2 = ev[itrk].daughter2();
      if (daughter1 != 0 || daughter2 != 0)
      {
        if (daughter2 - daughter1 == 1 && daughter1 < daughter2 && daughter1 > 0)
        {
          child[0] = daughter1;
          child[1] = daughter2;
        }
        else
        {
          continue;
        }
      }
      int index = ev[itrk].index();
      int pdg = ev[itrk].id();
      float px = ev[itrk].px();
      float py = ev[itrk].py();
      float pz = ev[itrk].pz();
      float e = ev[itrk].e();
      TLorentzVector v(ev[itrk].px(), ev[itrk].py(), ev[itrk].pz(), ev[itrk].e());
      float x = ev[itrk].xProd()*1e-12; // In Pythia 8 these values in mm (mm/c).
      float y = ev[itrk].yProd()*1e-12; // Convert mm (and mm/c) to fm (fm/c).
      float z = ev[itrk].zProd()*1e-12; // It should be zero for primary particles - i.e. not from
      float t = ev[itrk].tProd()*1e-12; // decayed particle.

      // Check particle cut.
      if (!cut.isGoodParticle(v, pdg))
      {
        continue;
      }

      // Add new track. FIXME: check nullptr.
      TClonesArray *mcTrkCol = mcArrays[McArrays::Particle];
      new ((*mcTrkCol)[mcTrkCol->GetEntries()]) McParticle(index, pdg, status, parent,
                                                           parent_decay, mate-1, decay, child,
                                                           px, py, pz, e, x, y, z, t);
    }
    // Add an event to DST.
    tree->Fill();
  }

  outFile->Write();
  outFile->Close();
  return EXIT_SUCCESS;
}
