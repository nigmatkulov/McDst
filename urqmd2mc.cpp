/**
 * urqmd2mc reads UrQMD events from the ftn13 or ftn14 ascii files and
 * converts them to the UniGen format and saves on a root file.
 *
 * ftn14 contains snapshots at given times (event steps). The event steps
 * are converted to separate events.
 *
 * ftn13 contains the final snapshot and the freeze-out coordinates.
 * The freeze-out coordinates are used. The final snapshot coordinates
 * are discarded.
 */

/*
  WARNING: This macro SHOULD NOT be compiled with the `cons' at BNL
  clusters.  The `cons' will add -D__ROOT_ to the `g++' so in the case
  of -D__ROOT__ this source file will be expanded to zero.
*/
#ifndef __ROOT__

// C++ headers
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <map>

// ROOT headers
#include "TObject.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TBranch.h"
#include "TMath.h"
#include "TClonesArray.h"

// McDst headers
#include "McRun.h"
#include "McEvent.h"
#include "McParticle.h"
#include "McPIDConverter.h"
#include "McArrays.h"

using namespace std;

TFile *fi;
TTree *tr;
TClonesArray *arrays[McArrays::NAllMcArrays];

//_________________
void bomb(const char *myst) {
  std::cerr << "Error: " << myst << ", bombing" << std::endl;
  exit(-1);
}

//_________________
std::string newName(char* origName) {
  std::string fname(origName);
  std::string key1 = ".f13";
  std::string key2 = ".f14";
  std::size_t found1 = fname.rfind(key1);
  std::size_t found2 = fname.rfind(key2);
  if ( found1 != std::string::npos ) {
    fname.replace(found1, key1.length(), ".mcDst.root");
  }
  else if ( found2 != std::string::npos ) {
    fname.replace(found2, key2.length(), ".mcDst.root");
  }
  else {
    bomb("Wrong input data format (not f13 or f14)");
  }

  return fname;
}

//_________________
int trapco(int ityp, int ichg) {
  // translate UrQMD pid code to pdg code

  /* UrQMD PIDs are in fact composite - a particle is fully defined by the
     type specifier (ityp), the charge (ichg) and in case of baryons, the
     third component isospin (iso3; ignored by us at present). For
     simplicity, our conversion tables collapse these into a single number
     as follows:
     - propagate the sign of ityp (particle-antiparticle distinction for
     baryons, strangeness-antistrangeness distinction for mesons) to that
     of the stored value;
     - shift the ichg range from -2..2 (UrQMD does not support nuclear
     fragments other than protons and neutrons so all particles it
     produces fall in this range) to 0..4 to make sure it doesn't
     interfere with the above;
     - multiply shifted charge by +/-1000 and add it to type. The latter
     is guaranteed to be smaller than 1000 (baryon types are one- or
     two-digit, meson types three-digit) so that way no ambiguities
     occur. */
  int id;
  if (ityp >= 0) id = 1000 * (ichg + 2) + ityp;
  else
    id = -1000 * (ichg + 2) + ityp;

  /* This will return 0 for unknown input values. */
  return McPIDConverter::instance()->pdgCode(id, McPIDConverter::eUrQMD);
}

//_________________
int main(int argc, char *argv[]) {

  ifstream in;
  char *inpfile;
  char c;
  int nevents;
  string dust;

  // Print debug information during the conversion
  bool debug = false;
  // Switcher that excludes elastic collisions
  bool excludeElastic = true;

  // McRun initialization
  McRun *run = nullptr;

  bool isElastic;
  string version, comment;
  int filetype, eos, aproj, zproj, atarg, ztarg, nr;
  double beta, b, bmin, bmax, sigma, elab, plab, sqrts, time, dtime;

  if (argc != 3) {
    std::cout << "usage:   " << argv[0] << " inputfile nevents\n";
    std::cout << "example: " << argv[0] << " inputfile.f14 10 \n"
	      << "This will create inputfile.uDst.root\n";
    exit(0);
  }

  // Read input file from the command line
  inpfile = argv[1];
  // Read number of events to convert from the command line
  nevents = atoi(argv[2]);

  int nout=0;

  // Check that filename contains .f13 or .f14
  TString oFileName( newName( argv[1] ) );

  // Try to open file
  in.open(inpfile);
  if ( in.fail() ) {
    bomb("cannot open input file");
  }

  fi = TFile::Open(oFileName.Data(), "RECREATE", "UrQMD");
  fi->SetCompressionLevel(9);
  int bufsize = 65536 * 4;
  int split = 99;

  // Create and set out TTree
  McEvent::Class()->IgnoreTObjectStreamer();
  McParticle::Class()->IgnoreTObjectStreamer();
  McRun::Class()->IgnoreTObjectStreamer();

  tr = new TTree("McDst", "UrQMD tree", split);
  tr->SetAutoSave(1000000);
  for (unsigned int i = 0; i < McArrays::NAllMcArrays; i++) {
    // Create arrayss
    arrays[i] = new TClonesArray( McArrays::mcArrayTypes[i], McArrays::mcArraySizes[i] );
    // Set branch
    tr->Branch(McArrays::mcArrayNames[i], &arrays[i], bufsize / 4, split);
  }

  // How oftep to print info
  const int bunch = 100;

  int events_processed=0;

  // Start event loop
  for (int n=0; n<nevents; n++) {
    if ((n%bunch)==0) std::cout << "event "  << setw(5) << n << std::endl;
    string line;

    // Read event information
    in >> dust >> dust >> version >> dust >> dust;
    in >> dust >> filetype >> dust >> dust >> dust >> aproj >> zproj;
    in >> dust >> dust >> dust >> atarg >> ztarg;
    in >> dust >> dust >> dust >> beta >> dust >> dust;
    in >> dust >> b >> bmin >> bmax >> dust >> sigma;
    in >> dust >> eos >> dust >> elab >> dust >> sqrts >> dust >> plab;
    in >> dust >> nr >> dust >> dust >> dust;
    in >> dust >> dust >> time >> dust >> dtime;
    in.ignore(777,'\n'); // ignore the rest of the line

    if ( debug ) {
      std::cout << "version: " << version << " sqrts: " << sqrts << " dtime: " << dtime << std::endl;
    }
    comment.clear();
    // read 4 lines of options and 6 lines of params
    for (int i=0; i<10; i++) {
      getline(in,line);
      comment.append(line);
      comment.append("\n");
    }
    in.ignore(777,'\n');

    // Clear all arrays
    for (unsigned int i = 0; i < McArrays::NAllMcArrays; i++) {
      // Create arrayss
      arrays[i]->Clear();
    }

    // Increment number of processed events
    events_processed++;

    int step_nr=0;
    char pee;
    // Loop over time slices
    while (1) {
      int mult;
      double step_time;
      pee=in.peek();
      if (pee=='U') break;
      if (pee==EOF) break;
      in >> mult >> step_time;

      // Check if collision is elastic
      isElastic = false;
      if ( (aproj+atarg) == mult ) {
        isElastic = true;
      }

      if ( debug ) {
	      std::cout << "Number of particles in event: " << mult << std::endl;
      }

      in.ignore(777,'\n'); // ignore the rest of the line
      getline(in,line);

      // Loop over generated particles
      for (int i=0;  i<mult; i++) {
      	if ( debug ) {
      	  std::cout << "Working on particle i: " << i << std::endl;
      	}
      	double t, x, y, z, e, px, py, pz;
      	int ityp, iso3, ichg, status, parent, parent_decay, mate;
      	int decay, child[2];

        // Read particl information
      	in >> t >> x >> y >> z;
      	in >> e >> px >> py >> pz >> dust;
      	in >> ityp >> iso3 >> ichg >> mate >> dust >> dust;

        // Print particle information
      	if ( debug ) {
      	  std::cout << Form( " t: %6.3f \tx: %6.3f \ty: %6.3f \tz: %6.3f \tpx: %6.3f \tpy: %6.3f \tpz: %6.3f\n",
      			                 t, x, y, z, px, py, pz );
      	}

        // Read freeze-out information
      	if (filetype==13) {
      	  in >> t >> x >> y >> z;
      	  in >> e >> px >> py >> pz;

          // Print freeze-out information
      	  if ( debug ) {
      	    std::cout << Form( " t: %6.3f \tx: %6.3f \ty: %6.3f \tz: %6.3f \tpx: %6.3f \tpy: %6.3f \tpz: %6.3f\n",
      			                   t, x, y, z, px, py, pz );
      	  }
      	} // if (filetype==13)

      	if (in.fail()) bomb("while reading tracks");

        // Do not fill McParticles for elastic collisions (if skip)
        if (isElastic && excludeElastic) continue;

      	status = parent_decay = decay = child[0] = child[1] = 0;
        // Add new particle to the event
      	new ( ( *(arrays[McArrays::Particle]) )[arrays[McArrays::Particle]->GetEntries()])
      	  McParticle( i, trapco(ityp, ichg), status, parent,
            		      parent_decay, mate-1, decay, child,
            		      px, py, pz, e, x, y, z, t );

        // Print particle information stored in McParticle
      	if (debug) {
      	  int iPart = arrays[McArrays::Particle]->GetEntries();
      	  McParticle *particle = (McParticle*)arrays[McArrays::Particle]->At(iPart-1);
      	  if ( !particle ) {
      	    std::cout << "Particle does not exist!" << std::endl;
      	    break;
      	  }
      	  std::cout << Form( " t: %6.3f \tx: %6.3f \ty: %6.3f \tz: %6.3f \tpx: %6.3f \tpy: %6.3f \tpz: %6.3f\n",
                  			     particle->t(), particle->x(),
                  			     particle->y(), particle->z(),
                  			     particle->px(), particle->py(),
                  			     particle->pz() );
      	} // if (debug) {
      } // for (int i=0;  i<mult; i++)

      do in.get(c); while (c!='\n');

      // Create an instance of the McEvent and add it to the DST
      // only when the event is not elastic and those are not required
      // to be skipped
      if ( !(isElastic && excludeElastic) ) {
        // Add new McEvent
        McEvent *ev = new ( ( *(arrays[McArrays::Event]) )[arrays[McArrays::Event]->GetEntries()]) McEvent();

        ev->setEventNr(nr);
        ev->setB(b);
        ev->setPhi(0);
        ev->setNes((int) (time/dtime));
        ev->setComment(line.data());
        ev->setStepNr(step_nr++);
        ev->setStepT(step_time);

        // Fill DST with event and track information
        nout += tr->Fill();
      }
    } // while (1) { // Loop over time slices

    if (pee==EOF) break;
  } // for (int n=0; n<nevents; n++)
  in.close();
  std::cout << events_processed << " events processed\n";

  // create the run object

  std::string generator = "UrQMD";
  generator.append(version);
  double m = 0.938271998;
  double ecm = sqrts/2; // energy per nucleon in cm
  double pcm = sqrt(ecm*ecm-m*m); // momentum per nucleon in cm
  double gamma = 1.0/sqrt(1-beta*beta);
  double pproj = gamma*(+pcm-beta*ecm);
  double ptarg = gamma*(-pcm-beta*ecm);
  run = new McRun( generator.data(), comment.data(),
            		   aproj, zproj, pproj,
            		   atarg, ztarg, ptarg,
            		   bmin, bmax, -1, 0, 0, sigma, events_processed);
  run->Write();
  fi->Write();
  fi->Close();
  std::cout << "Total bytes were written: " << nout << std::endl;
  return 0;
}

#endif // #ifndef __ROOT__
