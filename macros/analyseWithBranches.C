/**
 * \brief A simple example of reading TTree (McDst)
 *
 * A simple example that loads file, TTree (mcDst)
 * sets up branches to read, and prints few quantities
 */

// ROOT headers
#include "TTree.h"
#include "TFile.h"

// C++ headers
#include <iostream>

//_________________
void analyseWithBranches( const char* file = "../test.mcDst.root" ) {

  // Set input file
  TFile *f = new TFile( file );
  if ( !f ) { std::cout << "no file found" << std::endl; return; }
  // Set TTree
  TTree *T = (TTree*)f->Get("McDst");
  if ( !T ) { std::cout << "no McDst tree found" << std::endl; return; }

  // Turn on/off branches
  T->SetBranchStatus("*", 0);
  T->SetBranchStatus("Event", 1);
  T->SetBranchStatus("Event.*", 1);
  T->SetBranchStatus("Particle", 1);
  T->SetBranchStatus("Particle.*", 1);
  T->SetMakeClass(1);

  // Retrieve number of events
  Long64_t nentries = T->GetEntriesFast();
  std::cout << "entries: " << nentries << std::endl;

  Int_t           Event_;
  Float_t         Event_fB[1];

  Int_t           Particle_;
  Int_t           Particle_fPdg[10000];
  Float_t         Particle_fPz[10000];

  // Set up branches to read
  T->SetBranchAddress("Event", &Event_ );
  T->SetBranchAddress("Event.fB", Event_fB );

  T->SetBranchAddress("Particle", &Particle_ );
  T->SetBranchAddress("Particle.fPdg", Particle_fPdg );
  T->SetBranchAddress("Particle.fPz", Particle_fPz );

  Long64_t ientry = 0;

  std::cout << "getting entry " << ientry << std::endl;

  // Load values for the current event
  Long64_t nbytes = T->GetEntry( ientry );

  std::cout << "entry received" << std::endl;
  std::cout << "Impact parameter: " << Event_fB[0] << std::endl;
  std::cout << "num tracks: " << Particle_ << std::endl;

  std::cout << "Particle 0: "
	    << "\tpdg: " << Particle_fPdg[0] << ", "
	    << "pz: " << Particle_fPz[0]
	    << std::endl;

  std::cout << "Particle 1: "
	    << "\tpdg: " << Particle_fPdg[1] << ", "
	    << "pz: " << Particle_fPz[1]
	    << std::endl;

  // Print info for the turned on branches
  T->Show(0);
}
