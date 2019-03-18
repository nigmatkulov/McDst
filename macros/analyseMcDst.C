/**
 * \brief Example of how to read a file (list of files) using McDst classes
 *
 * analyseMcDst.C is an example of reading McDst format.
 * One can use either uDst file or a list of mcDst files (inFile.lis or
 * inFile.list) as an input, and preform physics analysis
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

// McDst headers
#include "../McDstReader.h"
#include "../McDst.h"
#include "../McEvent.h"
#include "../McParticle.h"
#include "../McRun.h"

// Load libraries (for ROOT_VERSTION_CODE >= 393215)
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,0,0)
R__LOAD_LIBRARY(../libMcDst)
#endif

// inFile - is a name of name.uDst.root file or a name
//          of a name.lis(t) files that contains a list of
//          name1.uDst.root files
//_________________
void analyseMcDst(const Char_t *inFile = "../test.mcDst.root",
		  const Char_t *oFileName = "oProcTest.root") {

  std::cout << "Hi! Lets do some physics, Master!" << std::endl;

  #if ROOT_VERSION_CODE < ROOT_VERSION(6,0,0)
    gSystem->Load("../libMcDst.so");
  #endif

  McDstReader* myReader = new McDstReader(inFile);
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

  TFile *oFile = new TFile(oFileName, "RECREATE");

  // Histogramming
  // Event
  TH2D *hImpactParVsNch = new TH2D("hImpactParVsNch",
				   "Impact parameter vs. Nch;Nch;Impact parameter (fm)",
				   300, -0.5, 599.5, 130, 0., 13.);
  TH1D *hNch = new TH1D("hNCh","Number of charged particles;Nch;Entries",
                        300, -0.5, 599.5);
  TH1D *hSqrtSnn = new TH1D( "hSqrtSnn","Collision energy;#sqrt{s_{NN}} (GeV);Entries",
                             100, 150., 250. );
  
  // Track
  TH1D *hPz = new TH1D("hPz","p_{z} of particle;p_{z} (GeV/c);Entries",
                            402, -201., 201.);
  TH2D *hPtVsEta = new TH2D("hPtVsEta",
			    "p_{T} vs. #eta of primary track;#eta;p_{T} (GeV/c)",
                            220, -1.1, 1.1, 80, 0.05, 2.05);
  TH1D *hPionMom = new TH1D("hPionMom","Momentum of #pi;p (GeV/c);Entries",
                            100, 0., 2.);

  Int_t eventCounter = 0;
  Int_t hundredIter = 0;

  // Run info
  hSqrtSnn->Fill( myReader->run()->nnSqrtS() );
  myReader->run()->print();

  // Loop over events
  for(Long64_t iEvent=0; iEvent<events2read; iEvent++) {

    eventCounter++;
    if( eventCounter >= 100000 ) {
      eventCounter = 0;
      hundredIter++;
      std::cout << "Working on event #[" << (hundredIter * 100000)
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
      McParticle *particle = dst->particle(iTrk);

      if (!particle) continue;
      //std::cout << "Track #[" << (iTrk+1) << "/" << nTracks << "]"  << std::endl;

      hPz->Fill( particle->pz() );

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

  std::cout << "I'm done with analysis. We'll have a Nobel Prize, Master!"
	    << std::endl;
}
