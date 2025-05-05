/**
 * Code is developed to build charged pion, kaons, proton and antiproton
 * spectra from mcDst files. It is designed to work in a standalone mode
 * to compile the code one should use prepared Makefile with the command:
 * ```
 * make
 * '''
 * Do not forget to adjust path to ROOT and McDst packages, libraries
 * and set environment variables (including McDst).
 *
 * To run the code simply run next commands from the terminal:
 * ```
 * spectraFromMcDst inputFile outputFile.root
 * '''
 * For the input file one can use either fname.mcDst.root file or
 * a list of mcDst files with the .list or .lis extention.
 */

// C++ headers
#include <iostream>
#include <cmath>

// ROOT headers
#include "TROOT.h"
#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include "TSystem.h"
#include "TString.h"
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

// Array of momentum spectra bin edges
const Int_t nBinsPtSpectra = 59;
Double_t ptBinEdges[nBinsPtSpectra + 1] = {
    0.0, 0.05, 0.1, 0.15, 0.2, 0.25, 0.3, 0.35, 0.4, 0.45, 0.5, 0.55,
    0.6, 0.65, 0.7, 0.75, 0.8, 0.85, 0.9, 0.95, 1.0, 1.05, 1.1, 1.15,
    1.2, 1.25, 1.3, 1.35, 1.4, 1.45, 1.5, 1.55, 1.6, 1.65, 1.7, 1.75,
    1.8, 1.85, 1.9, 1.95, 2.0, 2.1, 2.2, 2.3, 2.4, 2.6, 2.8, 3.0, 3.5,
    4.0, 4.5, 5.0, 5.5, 6.0, 6.5, 7.0, 7.5, 8.0, 8.5, 9.0};

//________________
Int_t findPtBin(Double_t pt) {
    Int_t bin = 0;
    for (Int_t iBin = 0; iBin < nBinsPtSpectra; iBin++) {
        if (ptBinEdges[iBin] <= pt && pt < ptBinEdges[iBin + 1]) {
            bin = iBin;
            break;
        }
    }
    return bin;
}

//________________
Double_t ptBinWidth(Int_t iBin) {
    return ptBinEdges[iBin + 1] - ptBinEdges[iBin];
}

//________________
int main(int argc, char *argv[]) {

    std::cout << "Hi! Lets do some physics, Master!" << std::endl;

    const char *fileName;
    const char *oFileName;

    // Next line is important for the spectra analysis because it
    // will be used to select |rapidity|<rapidityCut
    double rapidityCut = 0.1;

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

    McDstReader *myReader = new McDstReader(fileName);
    myReader->Init();

    // This is a way if you want to spead up IO
    std::cout << "Explicit read status for some branches" << std::endl;
    myReader->setStatus("*", 0);
    myReader->setStatus("Event", 1);
    myReader->setStatus("Particle", 1);
    std::cout << "Status has been set" << std::endl;

    std::cout << "Now I know what to read, Master!" << std::endl;

    if (!myReader->chain()) {
        std::cout << "No chain has been found." << std::endl;
    }
    Long64_t eventsInTree = myReader->tree()->GetEntries();
    std::cout << "eventsInTree: " << eventsInTree << std::endl;
    Long64_t events2read = myReader->chain()->GetEntries();

    std::cout << "Number of events to read: " << events2read << std::endl;

    /////////////////////
    //  Histogramming  //
    /////////////////////

    //
    // Event
    //

    TH1D *hImpactParameter = new TH1D("hImpactParameter", "Impact parameter;b (bm);Entries", 75, 0., 15.);
    hImpactParameter->Sumw2();
    TH1D *hRefMult = new TH1D("hRefMult", "Reference multiplicity (|#eta|<1, p_{T}>0.3 GeV/c); Reference multiplicity;Entries",
                              300, -0.5, 599.5);
    hRefMult->Sumw2();
    TH2D *hImpactParVsRefMult = new TH2D("hImpactParVsRefMult",
                                         "Impact parameter vs. refMult (|#eta|<1, p_{T}>0.3 GeV/c);Reference multiplicity;Impact parameter (fm)",
                                         500, -0.5, 499.5, 75, 0., 15.);
    hImpactParVsRefMult->Sumw2();

    // PDG codes and names
    std::vector<int> pdgCodes = {
        211, -211, 111, 321, -321, 311, 310, 130,
        3122, -3122, 3312, -3312, 3334, 333, 2112, -2112,
        2212, -2212, 443, 411, -411, 421, -421,
        431, -431, 3112, 3222, 3212, 22, 11, -11
    };

    std::vector<TString> particleNames = {
        "#pi^{+}", "#pi^{-}", "#pi^{0}", "K^{+}", "K^{-}", "K^{0}", "K^{0}_{S}", "K^{0}_{L}",
        "#Lambda", "#bar{#Lambda}", "#Xi^{-}", "#bar{#Xi}^{+}", "#Omega^{-}", "#phi(1020)",
        "n", "#bar{n}", "p", "#bar{p}", "J/#psi",
        "D^{+}", "D^{-}", "D^{0}", "#bar{D}^{0}", "D_{s}^{+}", "D_{s}^{-}",
        "#Sigma^{-}", "#Sigma^{+}", "#Sigma^{0}", "#gamma", "e^{-}", "e^{+}"
    };

    const int nEtaBins = 20;
    const int nPtBins = 50;
    const int nEnergyDisbalanceBins = 60;
    double etaMin = -5.0, etaMax = 5.0;
    double ptMin = 0.0, ptMax = 10.0;
    double energyDisbalanceMin = -0.3, energyDisbalanceMax = 0.3;

    const Int_t nBinsPtSpectra = 59;
    Double_t ptBinEdges[nBinsPtSpectra + 1] = {
        0.0, 0.05, 0.1, 0.15, 0.2, 0.25, 0.3, 0.35, 0.4, 0.45, 0.5, 0.55,
        0.6, 0.65, 0.7, 0.75, 0.8, 0.85, 0.9, 0.95, 1.0, 1.05, 1.1, 1.15,
        1.2, 1.25, 1.3, 1.35, 1.4, 1.45, 1.5, 1.55, 1.6, 1.65, 1.7, 1.75,
        1.8, 1.85, 1.9, 1.95, 2.0, 2.1, 2.2, 2.3, 2.4, 2.6, 2.8, 3.0, 3.5,
        4.0, 4.5, 5.0, 5.5, 6.0, 6.5, 7.0, 7.5, 8.0, 8.5, 9.0
    };

    std::vector<TH2D*> hAcceptance;
    std::vector<TH2D*> hEnergyDisbalance;
    std::vector<TH1D*> hSpectra;

    TH1D* hAbundance = new TH1D("hAbundance", "Particle Abundance;Particle;Entries", pdgCodes.size(), 0, pdgCodes.size());
    // hAbundance->Sumw2();
    for (size_t i = 0; i < pdgCodes.size(); ++i) {
        hAbundance->GetXaxis()->SetBinLabel(i + 1, particleNames[i]);

        TString hAccName = Form("hAcc_%zu", i);
        TString hAccTitle = Form("Acceptance: %s;#eta;p_{T} (GeV/c)", particleNames[i].Data());
        hAcceptance.push_back(new TH2D(hAccName, hAccTitle, nEtaBins, etaMin, etaMax, nPtBins, ptMin, ptMax));
        hAcceptance.back()->Sumw2();

        TString hEnergyName = Form("hEnergyDisbalance_%zu", i);
        TString hEnergyTitle = Form("Energy Disbalance: %s;#eta;E_{model}-E_{calc}", particleNames[i].Data());
        hEnergyDisbalance.push_back(new TH2D(hEnergyName, hEnergyTitle, nEtaBins, etaMin, etaMax, nEnergyDisbalanceBins, energyDisbalanceMin, energyDisbalanceMax));
        hEnergyDisbalance.back()->Sumw2();

        TString hSpecName = Form("hSpectra_%zu", i);
        TString hSpecTitle = Form("p_{T} spectra: %s;p_{T} (GeV/c);1/2#pi * #frac{d^2N}{p_{T} dy dp_{T}}", particleNames[i].Data());
        hSpectra.push_back(new TH1D(hSpecName, hSpecTitle, nBinsPtSpectra, ptBinEdges));
        hSpectra.back()->Sumw2();
    } // for (size_t i = 0; i < pdgCodes.size(); ++i)


    /////////////////////
    //     Analysis    //
    /////////////////////

    Int_t eventEntrieser = 0;
    Int_t clock = 10000;
    Int_t clockIter = 0;

    // Loop over events
    for (Long64_t iEvent = 0; iEvent < events2read; iEvent++) {

        eventEntrieser++;
        if (eventEntrieser >= clock) {
            eventEntrieser = 0;
            clockIter++;
            std::cout << "Working on event #[" << (clockIter * clock)
                      << "/" << events2read << "]" << std::endl;
        }

        Bool_t readEvent = myReader->loadEntry(iEvent);
        if (!readEvent) {
            std::cout << "Something went wrong, Master! Nothing to analyze..."
                      << std::endl;
            break;
        }

        // Retrieve femtoDst
        McDst *dst = myReader->mcDst();

        // Retrieve event information
        McEvent *event = dst->event();
        if (!event) {
            std::cout << "Something went wrong, Master! Event is hiding from me..."
                      << std::endl;
            break;
        }

        Double_t b = event->b();
        hImpactParameter->Fill(b);

        //
        // Particle analysis
        //

        // Retrieve number of particles in the event
        Int_t nParticles = dst->numberOfParticles();
        // Number of charged particles with |eta|<0.5 and pT>0.15 GeV/c
        Int_t refMult{0};

        // Track loop
        for (Int_t iTrk = 0; iTrk < nParticles; iTrk++) {

            // Retrieve i-th femto track
            McParticle *particle = (McParticle *)dst->particle(iTrk);

            if (!particle)
                continue;

            // std::cout << "Track #[" << (iTrk+1) << "/" << nParticles << "]"
            // 		<< std::endl;

            /*
            std::cout << "pdgId: " << particle->pdg()
                          << " status: " << particle->status()
                        << " px/py/pz/E/m/pt/y: "
                        << particle->px() << " / "
                        << particle->py() << " / "
                        << particle->pz() << " / "
                        << particle->e() << " / "
                        << particle->mass() << " / "
                      << particle->pt() << " / "
                      << particle->momentum().Rapidity() << " / "
                      << std::endl;
                  */

            int pdgCode = particle->pdg();
            double pt = particle->pt();
            double eta = particle->eta();
            int charge = particle->charge();
            double rapidity = particle->momentum().Rapidity();
            double rapidityIntervalWidth = 2 * rapidityCut; 

            // Calculate reference multiplicity
            if (charge != 0 && pt > 0.3 && TMath::Abs(eta) < 1.0) {
                refMult++;
            } // if ( charge != 0 && pt>0.15 && TMath::Abs(eta) < 0.5 )

            // Fill distributions for all particles
            auto it = std::find(pdgCodes.begin(), pdgCodes.end(), pdgCode);
            if (it != pdgCodes.end()) {
                size_t i = std::distance(pdgCodes.begin(), it);
            
                // Fill acceptance histogram
                hAcceptance[i]->Fill(eta, pt);
                hEnergyDisbalance[i]->Fill(eta, particle->e() - particle->pdgEnergy());
            
                // Fill spectrum if |rapidity| < 0.1
                if (std::abs(rapidity) < rapidityCut) {
                    int bin = hSpectra[i]->FindBin(pt);
                    double binWidth = hSpectra[i]->GetBinWidth(bin);
                    double weight = 1.0 / (2. * TMath::Pi() * pt * rapidityIntervalWidth * binWidth);
            
                    hSpectra[i]->Fill(pt, weight);
                }
            
                // Fill abundance histogram
                hAbundance->Fill(i);
            } // if (it != pdgCodes.end())


        } // for(Int_t iTrk=0; iTrk<nParticles; iTrk++)

        hRefMult->Fill(refMult);
        hImpactParVsRefMult->Fill(refMult, b);

    } // for(Long64_t iEvent=0; iEvent<events2read; iEvent++)


    // Write histograms to the output file
    TFile *oFile = new TFile(oFileName, "recreate");
    for (auto* h : hAcceptance) h->Write();
    for (auto* h : hEnergyDisbalance) h->Write();
    for (auto* h : hSpectra) h->Write();
    hAbundance->Write();
    oFile->Close();

    myReader->Finish();

    std::cout << "Acceptance and spectra processing is finished" << std::endl;

    return 0;
}
