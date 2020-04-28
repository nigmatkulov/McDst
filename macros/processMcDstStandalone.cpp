/**
 * \brief Example of how to read a file (list of files) using McDst class in a standalone mode
 *
 * processMcDstStandalone.cpp is an example of reading McDst format in a standalone mode
 * on your laptop of local computer farm.
 * Prerequisites:
 * - McDst directory
 * - CERN ROOT package
 * - g++ >= 4.8
 * - Makefile
 *
 * First, the program must be compiled with the Makefile, with simple command in the bash shell:
 * make
 *
 * Then the executable file processMcDstStandalone will be created. The current version of the program
 * expects 3 arguments: ./processMcDstStandalone inputFile outputFile
 * The first one is the program name, the second one is the name of the inputfile that 
 * maybe either the mcDst file itself, in a format dummyname.mcDst.root or a list of
 * such files called dummyname.list or dummyname.lis. The outputFile assumes the some_output_name.root.
 *
 */

// C++ headers
#include <iostream>

// ROOT headers
#include "TROOT.h"
#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include "TSystem.h"
#include "TH1.h"
#include "TH2.h"
#include "TMath.h"
#include "TDatabasePDG.h"

// McDst headers
#include "McDstReader.h"
#include "McDst.h"
#include "McEvent.h"
#include "McParticle.h"
#include "McRun.h"

//_________________
int main(int argc, char* argv[]) {

// #if ROOT_VERSION_CODE >= ROOT_VERSION(6,0,0)
//   R__LOAD_LIBRARY(libEG)
//   R__LOAD_LIBRARY(libMcDst)
// #else
//   gSystem->Load("libEG");
//   gSystem->Load("libMcDst.so");
// #endif

  std::cout << "Hi! Lets do some physics, Master!" << std::endl;

  const char* fileName;
  const char* oFileName;
  
  switch (argc) {
  case 3:
    fileName = argv[1];
    oFileName = argv[2];
    break;
  default:
    std::cout << "Usage: processMcDstStandalone inputFileName outputFileName.root" << std::endl;
    return -1;
  }
  std::cout << " inputFileName : " << fileName << std::endl;
  std::cout << " outputFileName: " << oFileName << std::endl;

  McDstReader* myReader = new McDstReader(fileName);
  myReader->Init();  

  // This is a way if you want to spead up IO
  std::cout << "Explicit read status for some branches" << std::endl;
  myReader->setStatus("*",0);
  myReader->setStatus("Event",1);
  myReader->setStatus("Particle",1);
  std::cout << "Status has been set" << std::endl;

  std::cout << "Now I know what to read, Master!" << std::endl;

  if( !myReader->chain() ) {
    std::cout << "No chain has been found." << std::endl;
  }
  Long64_t eventsInTree = myReader->tree()->GetEntries();
  std::cout << "eventsInTree: "  << eventsInTree << std::endl;
  Long64_t events2read = myReader->chain()->GetEntries();

  std::cout << "Number of events to read: " << events2read << std::endl;

  TFile *oFile = new TFile(oFileName, "recreate");

  /////////////////////
  //  Histogramming  //
  /////////////////////

  // Event
  TH2D *hImpactParVsNch = new TH2D("hImpactParVsNch",
				   "Impact parameter vs. Nch;Nch;Impact parameter (fm)",
				   300, -0.5, 599.5, 130, 0., 13.);
  TH1D *hNch = new TH1D("hNCh","Number of charged particles;Nch;Entries",
                        300, -0.5, 599.5);

  // Track
  TH1D *hParticleCharge = new TH1D("hParticleCharge","Charge of the particle;Q;Entries",
				   19, -9.5, 9.5);
  TH1D *hPz = new TH1D("hPz","p_{z} of particle;p_{z} (GeV/c);Entries",
                            402, -201., 201.);
  TH2D *hPtVsEta = new TH2D("hPtVsEta",
			    "p_{T} vs. #eta of primary track;#eta;p_{T} (GeV/c)",
                            220, -1.1, 1.1, 80, 0.05, 2.05);
  TH1D *hPionMom = new TH1D("hPionMom","Momentum of #pi;p (GeV/c);Entries",
                            100, 0., 2.);

  /////////////////////
  //     Analysis    //
  /////////////////////

  Int_t eventCounter = 0;
  Int_t hundredIter = 0;

  // Loop over events
  for(Long64_t iEvent=0; iEvent<events2read; iEvent++) {

    eventCounter++;
    if( eventCounter >= 100 ) {
      eventCounter = 0;
      hundredIter++;
      std::cout << "Working on event #[" << (hundredIter * 100)
		<< "/" << events2read << "]" << std::endl;
    }

    Bool_t readEvent = myReader->loadEntry(iEvent);
    if( !readEvent ) {
      std::cout << "Something went wrong, Master! Nothing to analyze..."
		<< std::endl;
      break;
    }

    // Retrieve femtoDst
    McDst *dst = myReader->mcDst();

    // Retrieve event information
    McEvent *event = dst->event();
    if( !event ) {
      std::cout << "Something went wrong, Master! Event is hiding from me..."
		<< std::endl;
      break;
    }

    // Track analysis
    Int_t nTracks = dst->numberOfParticles();
    Int_t NumOfCharged = 0;

    // Track loop
    for(Int_t iTrk=0; iTrk<nTracks; iTrk++) {

      // Retrieve i-th femto track
      McParticle *particle = (McParticle*)dst->particle(iTrk);

      if (!particle) continue;

      // std::cout << "Track #[" << (iTrk+1) << "/" << nTracks << "]"  
      // 		<< std::endl;

      // std::cout << "pdgId: " << particle->pdg() 
      // 		<< " status: " << particle->status()
      // 		<< " px/py/pz/E/m: " 
      // 		<< particle->px() << " / " 
      // 		<< particle->py() << " / "
      // 		<< particle->pz() << " / " 
      // 		<< particle->e() << " / "
      // 		<< particle->mass() << std::endl;

      
      hPz->Fill( particle->pz() );
      hParticleCharge->Fill( particle->charge() );

      if ( particle->charge() ) {
        NumOfCharged++;

        hPtVsEta->Fill( particle->eta(), particle->pt() );

        if ( particle->pdg() == 211 ) {
          hPionMom->Fill( particle->ptot() );
        }
      } // if ( particle->charge() )
    } //for(Int_t iTrk=0; iTrk<nTracks; iTrk++)

    hImpactParVsNch->Fill( NumOfCharged, event->impact() );
    hNch->Fill( NumOfCharged );

  } //for(Long64_t iEvent=0; iEvent<events2read; iEvent++)

  oFile->Write();
  oFile->Close();
  myReader->Finish();

  std::cout << "I'm done with analysis. We'll have a Nobel Prize, Master!" << std::endl;

  return 0;
}
