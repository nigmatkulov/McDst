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


//StarGenEvent headers

#include "../EVENT/StarGenAAEvent.h"
#include "../EVENT/StarGenEvent.h"
#include "../EVENT/StarGenParticle.h"
#include "../EVENT/StarGenStats.h"


using namespace std;

TClonesArray *arrays[McArrays::NAllMcArrays];

//_________________
void bomb(const char *myst) {
  std::cerr << "Error: " << myst << ", bombing" << std::endl;
  exit(-1);
}

//_________________
std::string newName(char* origName) {
  std::string fname(origName);
  std::string key1 = ".root";
  std::size_t found1 = fname.rfind(key1);
  if ( found1 != std::string::npos ) {
    fname.replace(found1, key1.length(), ".mcDst.root");
  }
  else {
    bomb("Wrong input data format (not .root)");
  }

  return fname;
}


//_________________
int main(int argc, char *argv[]) {

  //  ifstream in;
  char *inpfile;
  char c;
  int nevents;
  string dust;
  bool debug = false;

  McRun *run = nullptr;

  string version, comment;
  int filetype, eos, aproj, zproj, atarg, ztarg, nr;
  double beta, b, bmin, bmax, sigma, elab, plab, sqrts, time, dtime;

  if (argc != 3) {
    std::cout << "usage:   " << argv[0] << " inputfile nevents\n";
    std::cout << "example: " << argv[0] << " inputfile.root 10 \n"
	      << "This will create inputfile.mcDst.root\n";
    exit(0);
  }

  inpfile = argv[1];
  nevents = atoi(argv[2]);

  int nout=0;

  // Check that filename contains .f13 or .f14
  TString oFileName( newName( argv[1] ) );

  
  /*  in.open(inpfile);
      if ( in.fail() ) {
      bomb("cannot open input file");
      }
  */

  TFile *fi = TFile::Open(oFileName.Data(), "RECREATE", "HIJING");



  fi->SetCompressionLevel(9);
  int bufsize = 65536 * 4;
  int split = 99;

  // Create and set out TTree
  McEvent::Class()->IgnoreTObjectStreamer();
  McParticle::Class()->IgnoreTObjectStreamer();
  McRun::Class()->IgnoreTObjectStreamer();

  TTree *tr = new TTree("McDst", "Hijing tree", split);
  tr->SetAutoSave(1000000);
  for (unsigned int i = 0; i < McArrays::NAllMcArrays; i++) {
    // Create arrayss
    arrays[i] = new TClonesArray( McArrays::mcArrayTypes[i], McArrays::mcArraySizes[i] );
    // Set branch
    tr->Branch(McArrays::mcArrayNames[i], &arrays[i], bufsize / 4, split);
  }

  // Try to open file
  TFile *inFile = new TFile(inpfile,"READ");

  //Getting a tree
  TTree *inTree = nullptr;
  inFile->GetObject("genevents",inTree);
  StarGenEvent *evHij = nullptr;
  StarGenParticle *parHij = nullptr;
  StarGenAAEvent *evInfoHij = nullptr;
  StarGenStats *statHij = nullptr;
  inFile->GetObject("stats",statHij);
  sigma = 0;//statHij->sigmaGen;
  inTree->SetBranchAddress("primaryEvent", &evHij);
  inTree->SetBranchAddress("Hijing", &evInfoHij);



  
  // How oftep to print info
  const int bunch = 100;

  int events_processed=0;
  
  // Start event loop

  if (nevents > inTree->GetEntriesFast()) {
    std::cout<<"Too much nevents. Switch to"<<inTree->GetEntriesFast()<<std::endl;
    nevents = inTree->GetEntriesFast();
  }//if (nevents > inTree->GetEntriesFast())

  for (int n=0; n<nevents; n++) {

    for (unsigned int i = 0; i < McArrays::NAllMcArrays; i++) {
      // Create arrayss
      arrays[i]->Clear();
    }

    inTree->GetEntry(n);

    McEvent *ev = new ( ( *(arrays[McArrays::Event]) )[arrays[McArrays::Event]->GetEntries()]) McEvent();

    ev->setEventNr(evInfoHij->GetEventNumber());
    ev->setB(evInfoHij->impactParameter);
    ev->setPhi(evInfoHij->reactionPlane);
    ev->setNes(0);

    sqrts = evInfoHij->GetRootS();
    
    //    std::cout<<evHij->GetNumberOfParticles()<<" "<<evInfoHij->numberOfParticipantNeutrons[0]+evInfoHij->numberOfParticipantNeutrons[1]+evInfoHij->numberOfParticipantProtons[0]+evInfoHij->numberOfParticipantProtons[1]<<std::endl;


    if (evHij->GetNumberOfParticles() < 2) continue;


    events_processed++;
	
    for ( int iTr = 1; iTr < evHij->GetNumberOfParticles(); iTr++) { //BEWARE: Numeration at StarGenEvent is specific: it starts with 1, but counts from 0 >.<
      parHij = (*evHij)[iTr];

      double t, x, y, z, e, px, py, pz;
      int ityp, iso3, ichg, status, parent, parent_decay, mate;
      int decay, child[2];

      t = parHij->GetTof();
      x = parHij->GetVx();
      y = parHij->GetVy();
      z = parHij->GetVz();

      e = parHij->GetEnergy();


      px = parHij->GetPx();
      py = parHij->GetPy();
      pz = parHij->GetPz();

      ityp = parHij->GetId();
      status = parHij->GetStatus();

      
      if ( parHij->GetLastMother() == -1 ) {
	parent = parHij->GetFirstMother();
      } else {
	parent = parHij->GetLastMother();
      }//      if ( parHij->GetLastMother() == -1 ) {

      parent_decay = 0;
      mate = 0;
      decay = 0;
      child[0] = parHij->GetFirstDaughter();
      child[1] = parHij->GetLastDaughter();
      if (parHij->GetFirstDaughter() == -1 && parHij->GetLastDaughter() == -1) decay = -1;      

      
      new ( ( *(arrays[McArrays::Particle]) )[arrays[McArrays::Particle]->GetEntries()])
	McParticle( iTr-1, ityp, status, parent,
		    parent_decay, mate-1, decay, child,
		    px, py, pz, e, x, y, z, t );
      McParticle *particle = (McParticle*)arrays[McArrays::Particle]->At(iTr-1);      
      //      std::cout<<particle->pdg()<<std::endl;
      
    }//    for ( int iTr = 0; iTr < evHij->GetNumberOfParticles(); iTr++)
    nout += tr->Fill();    
  } // for (int n=0; n<nevents; n++)

  inFile->Close();
  fi->cd();
  std::cout << events_processed << " events processed\n";

  // create the run object

  std::string generator = "Hijing";
  generator.append(version);
  double m = 0.938271998;
  double ecm = sqrts/2; // energy per nucleon in cm
  double pcm = sqrt(ecm*ecm-m*m); // momentum per nucleon in cm
  double gamma = 0;//1.0/sqrt(1-beta*beta);
  double pproj = 0;//gamma*(+pcm-beta*ecm);
  double ptarg = 0;//gamma*(-pcm-beta*ecm);
  bmin = 0;
  bmax = 0;
  aproj = 0;
  zproj = 0;

  atarg = 0;
  ztarg = 0;
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
