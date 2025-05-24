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
#include <vector>

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
#include "TParticlePDG.h"

// McDst headers
#include "McDstReader.h"
#include "McDst.h"
#include "McEvent.h"
#include "McParticle.h"
#include "McRun.h"
#include "McUtils.h"

//________________
int main(int argc, char *argv[]) {

    std::cout << "Start spectraFromMcDst" << std::endl;

    const char *fileName;
    const char *oFileName;

    int ABeam = 124;
    int ZBeam = 54;
    int ATarget = 184;
    int ZTarget = 74;
    double beamEkin = 3.0;


    double betaCM = McUtils::beta_from_Ekin(beamEkin, ABeam, ATarget);

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
        3122, -3122, 
        3212, 3112, 3222, 
        3322, -3322, 3312, 
        -3312, 3334, 
        333, 2112, -2112,
        2212, -2212, 443, 411, -411, 421, -421,
        431, -431, 22, 11, -11
    };

    std::vector<TString> particleNames = {
        "#pi^{+}", "#pi^{-}", "#pi^{0}", "K^{+}", "K^{-}", "K^{0}", "K^{0}_{S}", "K^{0}_{L}",
        "#Lambda", "#bar{#Lambda}", 
        "#Sigma^{0}", "#Sigma^{-}", "#Sigma^{+}", 
        "#Xi^{0}", "#bar{#Xi}^{0}", "#Xi^{-}", "#bar{#Xi}^{+}", 
        "#Omega^{-}", "#phi(1020)",
        "n", "#bar{n}", "p", "#bar{p}", "J/#psi",
        "D^{+}", "D^{-}", "D^{0}", "#bar{D}^{0}", "D_{s}^{+}", "D_{s}^{-}",
        "#gamma", "e^{-}", "e^{+}"
    };

    const int nEtaBins = 140;
    const int nPtBins = 100;
    const int nEnergyImbalanceBins = 60;
    const int nEnergyBins = 100;
    const int nXBins = 50;
    const int nYBins = 50;
    const int nZBins = 85;
    const int nTBins = 42;
    const int nTauBins = 42;
    const int nSpaceTimeRapidityBins = 140;
    double etaMin = -7.0, etaMax = 7.0;
    double ptMin = 0.0, ptMax = 10.0;
    double energyMin = 0., energyMax = 10.;
    double xmin = -50., xmax = 50.;
    double ymin = -50., ymax = 50.;
    double zmin = -210., zmax = 210.;
    double tmin = -1., tmax = 209.;
    double tauMin = -1., tauMax = 209.;
    double spaceTimeRapidityMin = -7.0, spaceTimeRapidityMax = 7.0;
    double energyImbalanceMin = -0.3, energyImbalanceMax = 0.3;

    const Int_t nBinsPtSpectra = 59;
    Double_t ptBinEdges[nBinsPtSpectra + 1] = {
        0.0, 0.05, 0.1, 0.15, 0.2, 0.25, 0.3, 0.35, 0.4, 0.45, 0.5, 0.55,
        0.6, 0.65, 0.7, 0.75, 0.8, 0.85, 0.9, 0.95, 1.0, 1.05, 1.1, 1.15,
        1.2, 1.25, 1.3, 1.35, 1.4, 1.45, 1.5, 1.55, 1.6, 1.65, 1.7, 1.75,
        1.8, 1.85, 1.9, 1.95, 2.0, 2.1, 2.2, 2.3, 2.4, 2.6, 2.8, 3.0, 3.5,
        4.0, 4.5, 5.0, 5.5, 6.0, 6.5, 7.0, 7.5, 8.0, 8.5, 9.0
    };

    std::vector<TH2D*> hAcceptanceCMS;
    std::vector<TH2D*> hEnergyImbalanceCMS;
    std::vector<TH2D*> hMassImbalanceCMS;
    std::vector<TH1D*> hPtSpectra;
    std::vector<TH1D*> hEnergyCMS;
    std::vector<TH2D*> hFreezeOutXYCMS;
    std::vector<TH2D*> hFreezeOutZXCMS;
    std::vector<TH2D*> hFreezeOutZTCMS;
    std::vector<TH2D*> hProperTimeVsSpaceTimeRapidityCMS;
    std::vector<TH1D*> hFreezeOutTimeCMS;
    std::vector<TH1D*> hProperTimeCMS;
    std::vector<TH1D*> hEtaCMS;

    std::vector<TH2D*> hAcceptanceLab;
    std::vector<TH2D*> hEnergyImbalanceLab;
    std::vector<TH2D*> hMassImbalanceLab;
    std::vector<TH1D*> hEnergyLab;
    std::vector<TH2D*> hFreezeOutXYLab;
    std::vector<TH2D*> hFreezeOutZXLab;
    std::vector<TH2D*> hFreezeOutZTLab;
    std::vector<TH2D*> hProperTimeVsSpaceTimeRapidityLab;
    std::vector<TH1D*> hFreezeOutTimeLab;
    std::vector<TH1D*> hProperTimeLab;
    std::vector<TH1D*> hEtaLab;

    TH1D* hAbundance = new TH1D("hAbundance", "Particle Abundance;Particle;Entries", pdgCodes.size(), 0, pdgCodes.size());
    // hAbundance->Sumw2();
    for (size_t i = 0; i < pdgCodes.size(); ++i) {
        hAbundance->GetXaxis()->SetBinLabel(i + 1, particleNames[i]);

        // Center of mass frame histograms

        TString hAccCMSName = Form("hAcc_%zu", i);
        TString hAccCMSTitle = Form("Acceptance in the CMS frame: %s;#eta;p_{T} (GeV/c)", particleNames[i].Data());
        hAcceptanceCMS.push_back(new TH2D(hAccCMSName, hAccCMSTitle, nEtaBins, etaMin, etaMax, nPtBins, ptMin, ptMax));
        hAcceptanceCMS.back()->Sumw2();

        TString hEnergyImbalanceCMSName = Form("hEnergyImbalanceCMS_%zu", i);
        TString hEnergyImbalanceCMSTitle = Form("Energy imbalance in the CMS frame: %s;#eta;E_{model}-E_{calc}", particleNames[i].Data());
        hEnergyImbalanceCMS.push_back(new TH2D(hEnergyImbalanceCMSName, hEnergyImbalanceCMSTitle, nEtaBins, etaMin, etaMax, nEnergyImbalanceBins, energyImbalanceMin, energyImbalanceMax));
        hEnergyImbalanceCMS.back()->Sumw2();

        TString hMassCMSName = Form("hMassImbalanceCMS_%zu", i);
        TString hMassCMSTitle = Form("Mass imbalance in the CMS frame: %s;#eta;M_{model}-M_{PDG}", particleNames[i].Data());
        hMassImbalanceCMS.push_back(new TH2D(hMassCMSName, hMassCMSTitle, nEtaBins, etaMin, etaMax, nEnergyImbalanceBins, energyImbalanceMin, energyImbalanceMax));
        hMassImbalanceCMS.back()->Sumw2();

        TString hSpecName = Form("hPtSpectra_%zu", i);
        TString hSpecTitle = Form("p_{T} spectra: %s;p_{T} (GeV/c);#frac{1}{2#pi} #frac{d^{2}N}{p_{T} dy dp_{T}}", particleNames[i].Data());
        hPtSpectra.push_back(new TH1D(hSpecName, hSpecTitle, nBinsPtSpectra, ptBinEdges));
        hPtSpectra.back()->Sumw2();

        TString hEnergyCMSName = Form("hEnergyCMS_%zu", i);
        TString hEnergyCMSTitle = Form("Energy in the CMS frame: %s;E (GeV);Entries", particleNames[i].Data());
        hEnergyCMS.push_back(new TH1D(hEnergyCMSName, hEnergyCMSTitle, nEnergyBins, energyMin, energyMax));
        hEnergyCMS.back()->Sumw2();

        TString hFreezeOutXYCMSName = Form("hFreezeOutXYCMS_%zu", i);
        TString hFreezeOutXYCMSTitle = Form("Freeze-out in the CMS frame: %s;x (fm);y (fm)", particleNames[i].Data());
        hFreezeOutXYCMS.push_back(new TH2D(hFreezeOutXYCMSName, hFreezeOutXYCMSTitle, nXBins, xmin, xmax, nYBins, ymin, ymax));
        hFreezeOutXYCMS.back()->Sumw2();

        TString hFreezeOutZXCMSName = Form("hFreezeOutZXCMS_%zu", i);
        TString hFreezeOutZXCMSTitle = Form("Freeze-out in the CMS frame: %s;x (fm);z (fm)", particleNames[i].Data());
        hFreezeOutZXCMS.push_back(new TH2D(hFreezeOutZXCMSName, hFreezeOutZXCMSTitle, nXBins, xmin, xmax, nZBins, zmin, zmax));
        hFreezeOutZXCMS.back()->Sumw2();

        TString hFreezeOutZTCMSName = Form("hFreezeOutZTCMS_%zu", i);
        TString hFreezeOutZTCMSTitle = Form("Freeze-out in the CMS frame: %s;z (fm);t (fm/c)", particleNames[i].Data());
        hFreezeOutZTCMS.push_back(new TH2D(hFreezeOutZTCMSName, hFreezeOutZTCMSTitle, nXBins, xmin, xmax, nTBins, tmin, tmax));
        hFreezeOutZTCMS.back()->Sumw2();

        TString hProperTimeVsSpaceTimeRapidityCMSName = Form("hProperTimeVsSpaceTimeRapidityCMS_%zu", i);
        TString hProperTimeVsSpaceTimeRapidityCMSTitle = Form("Proper time vs. space-time rapidity in the CMS frame: %s;#eta_{s};#tau (fm/c)", particleNames[i].Data());
        hProperTimeVsSpaceTimeRapidityCMS.push_back(new TH2D(hProperTimeVsSpaceTimeRapidityCMSName, hProperTimeVsSpaceTimeRapidityCMSTitle, nSpaceTimeRapidityBins, spaceTimeRapidityMin, spaceTimeRapidityMax, nTauBins, tauMin, tauMax));
        hProperTimeVsSpaceTimeRapidityCMS.back()->Sumw2();

        TString hFreezeOutTimeCMSName = Form("hFreezeOutTimeCMS_%zu", i);
        TString hFreezeOutTimeCMSTitle = Form("Freeze-out time in the CMS frame: %s;t (fm/c);Entries", particleNames[i].Data());
        hFreezeOutTimeCMS.push_back(new TH1D(hFreezeOutTimeCMSName, hFreezeOutTimeCMSTitle, nTBins, tmin, tmax));
        hFreezeOutTimeCMS.back()->Sumw2();

        TString hProperTimeCMSName = Form("hProperTimeCMS_%zu", i);
        TString hProperTimeCMSTitle = Form("Proper time in the CMS frame: %s;#tau (fm/c);Entries", particleNames[i].Data());
        hProperTimeCMS.push_back(new TH1D(hProperTimeCMSName, hProperTimeCMSTitle, nTBins, tauMin, tauMax));
        hProperTimeCMS.back()->Sumw2();

        TString hEtaCMSName = Form("hEtaCMS_%zu", i);
        TString hEtaCMSTitle = Form("Pseudorapidity (#eta) in the CMS frame: %s;#eta;dN/d#eta", particleNames[i].Data());
        hEtaCMS.push_back(new TH1D(hEtaCMSName, hEtaCMSTitle, nEtaBins, etaMin, etaMax));
        hEtaCMS.back()->Sumw2();

        // Laboratory frame histograms

        TString hAccLabName = Form("hAccLab_%zu", i);
        TString hAccLabTitle = Form("Acceptance in the Lab frame: %s;#eta;p_{T} (GeV/c)", particleNames[i].Data());
        hAcceptanceLab.push_back(new TH2D(hAccLabName, hAccLabTitle, nEtaBins, etaMin, etaMax, nPtBins, ptMin, ptMax));
        hAcceptanceLab.back()->Sumw2();

        TString hEnergyImbalanceLabName = Form("hEnergyImbalanceLab_%zu", i);
        TString hEnergyImbalanceLabTitle = Form("Energy imbalance in the Lab frame: %s;#eta;E_{model}-E_{calc}", particleNames[i].Data());
        hEnergyImbalanceLab.push_back(new TH2D(hEnergyImbalanceLabName, hEnergyImbalanceLabTitle, nEtaBins, etaMin, etaMax, nEnergyImbalanceBins, energyImbalanceMin, energyImbalanceMax));
        hEnergyImbalanceLab.back()->Sumw2();

        TString hMassLabName = Form("hMassImbalanceLab_%zu", i);
        TString hMassLabTitle = Form("Mass imbalance in the Lab frame: %s;#eta;M_{model}-M_{PDG}", particleNames[i].Data());
        hMassImbalanceLab.push_back(new TH2D(hMassLabName, hMassLabTitle, nEtaBins, etaMin, etaMax, nEnergyImbalanceBins, energyImbalanceMin, energyImbalanceMax));
        hMassImbalanceLab.back()->Sumw2();

        TString hEnergyLabName = Form("hEnergyLab_%zu", i);
        TString hEnergyLabTitle = Form("Energy in the Lab frame: %s;E (GeV);Entries", particleNames[i].Data());
        hEnergyLab.push_back(new TH1D(hEnergyLabName, hEnergyLabTitle, nEnergyBins, energyMin, energyMax));
        hEnergyLab.back()->Sumw2();

        TString hFreezeOutXYLabName = Form("hFreezeOutXYLab_%zu", i);
        TString hFreezeOutXYLabTitle = Form("Freeze-out in the Lab frame: %s;x (fm);y (fm)", particleNames[i].Data());
        hFreezeOutXYLab.push_back(new TH2D(hFreezeOutXYLabName, hFreezeOutXYLabTitle, nXBins, xmin, xmax, nYBins, ymin, ymax));
        hFreezeOutXYLab.back()->Sumw2();

        TString hFreezeOutZXLabName = Form("hFreezeOutZXLab_%zu", i);
        TString hFreezeOutZXLabTitle = Form("Freeze-out in the Lab frame: %s;x (fm);z (fm)", particleNames[i].Data());
        hFreezeOutZXLab.push_back(new TH2D(hFreezeOutZXLabName, hFreezeOutZXLabTitle, nXBins, xmin, xmax, nZBins, zmin, zmax));
        hFreezeOutZXLab.back()->Sumw2();

        TString hFreezeOutZTLabName = Form("hFreezeOutZTLab_%zu", i);
        TString hFreezeOutZTLabTitle = Form("Freeze-out in the Lab frame: %s;z (fm);t (fm/c)", particleNames[i].Data());
        hFreezeOutZTLab.push_back(new TH2D(hFreezeOutZTLabName, hFreezeOutZTLabTitle, nXBins, xmin, xmax, nTBins, tmin, tmax));
        hFreezeOutZTLab.back()->Sumw2();

        TString hProperTimeVsSpaceTimeRapidityLabName = Form("hProperTimeVsSpaceTimeRapidityLab_%zu", i);
        TString hProperTimeVsSpaceTimeRapidityLabTitle = Form("Proper time vs. space-time rapidity in the Lab frame: %s;#eta_{s};#tau (fm/c)", particleNames[i].Data());
        hProperTimeVsSpaceTimeRapidityLab.push_back(new TH2D(hProperTimeVsSpaceTimeRapidityLabName, hProperTimeVsSpaceTimeRapidityLabTitle, nSpaceTimeRapidityBins, spaceTimeRapidityMin, spaceTimeRapidityMax, nTauBins, tauMin, tauMax));
        hProperTimeVsSpaceTimeRapidityLab.back()->Sumw2();

        TString hFreezeOutTimeLabName = Form("hFreezeOutTimeLab_%zu", i);
        TString hFreezeOutTimeLabTitle = Form("Freeze-out time in the Lab frame: %s;t (fm/c);Entries", particleNames[i].Data());
        hFreezeOutTimeLab.push_back(new TH1D(hFreezeOutTimeLabName, hFreezeOutTimeLabTitle, nTBins, tmin, tmax));
        hFreezeOutTimeLab.back()->Sumw2();

        TString hProperTimeLabName = Form("hProperTimeLab_%zu", i);
        TString hProperTimeLabTitle = Form("Proper time in the Lab frame: %s;#tau (fm/c);Entries", particleNames[i].Data());
        hProperTimeLab.push_back(new TH1D(hProperTimeLabName, hProperTimeLabTitle, nTBins, tauMin, tauMax));
        hProperTimeLab.back()->Sumw2();

        TString hEtaLabName = Form("hEtaLab_%zu", i);
        TString hEtaLabTitle = Form("Pseudorapidity (#eta) in the Lab frame: %s;#eta;dN/d#eta", particleNames[i].Data());
        hEtaLab.push_back(new TH1D(hEtaLabName, hEtaLabTitle, nEtaBins, etaMin, etaMax));
        hEtaLab.back()->Sumw2();
    } // for (size_t i = 0; i < pdgCodes.size(); ++i)


    /////////////////////
    //     Analysis    //
    /////////////////////

    Int_t eventCounter = 0;
    Int_t clock = 10000;
    Int_t clockIter = 0;

    // Loop over events
    for (Long64_t iEvent = 0; iEvent < events2read; iEvent++) {

        eventCounter++;
        if (eventCounter >= clock) {
            eventCounter = 0;
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
            double x = particle->x();
            double y = particle->y();
            double z = particle->z();
            double t = particle->t();
            double tau = particle->tau();
            double spaceTimeRapidity = particle->etaS();


            // Calculate reference multiplicity
            if (charge != 0 && pt > 0.3 && TMath::Abs(eta) < 1.0) {
                refMult++;
            } // if ( charge != 0 && pt>0.15 && TMath::Abs(eta) < 0.5 )

            // Fill distributions for all particles
            auto it = std::find(pdgCodes.begin(), pdgCodes.end(), pdgCode);
            if (it != pdgCodes.end()) {
                size_t i = std::distance(pdgCodes.begin(), it);

                // Fill abundance histogram
                hAbundance->Fill(i);

                // Center-of-mass frame histograms
            
                // Fill acceptance histogram
                hAcceptanceCMS[i]->Fill(eta, pt);
                double pdgEnergy = particle->ptot() * particle->ptot() + particle->pdgMass() * particle->pdgMass();
                pdgEnergy = TMath::Sqrt(pdgEnergy);
                hEnergyImbalanceCMS[i]->Fill(eta, particle->e() - pdgEnergy);
                hMassImbalanceCMS[i]->Fill(eta, particle->mass() - particle->pdgMass());
                hEnergyCMS[i]->Fill(particle->e());
                hFreezeOutXYCMS[i]->Fill(x, y);
                hFreezeOutZXCMS[i]->Fill(z, x);
                hFreezeOutZTCMS[i]->Fill(z, t);
                hProperTimeVsSpaceTimeRapidityCMS[i]->Fill(spaceTimeRapidity, tau);
                hFreezeOutTimeCMS[i]->Fill(t);
                hProperTimeCMS[i]->Fill(tau);
                hEtaCMS[i]->Fill(eta);

                // Fill spectrum if |rapidity| < 0.1
                if (std::abs(rapidity) < rapidityCut) {
                    int bin = hPtSpectra[i]->FindBin(pt);
                    double binWidth = hPtSpectra[i]->GetBinWidth(bin);
                    double weight = 1.0 / (2. * TMath::Pi() * pt * rapidityIntervalWidth * binWidth);
            
                    hPtSpectra[i]->Fill(pt, weight);
                }

                // Laboratory frame histograms
                TLorentzVector pCMS = particle->momentum();
                TLorentzVector pLab = McUtils::boostToLabFrame(pCMS, betaCM);
                TLorentzVector rCMS = particle->position();
                TLorentzVector rLab = McUtils::boostToLabFrame(rCMS, betaCM);
                particle->setMomentum(pLab);
                particle->setPosition(rLab);


                eta = particle->eta();
                z = particle->z();
                t = particle->t();
                tau = particle->tau();
                spaceTimeRapidity = particle->etaS();
                pdgEnergy = TMath::Sqrt(particle->ptot() * particle->ptot() + particle->pdgMass() * particle->pdgMass());
            
                hAcceptanceLab[i]->Fill(eta, pt);
                hEnergyImbalanceLab[i]->Fill(eta, particle->e() - pdgEnergy);
                hMassImbalanceLab[i]->Fill(eta, particle->mass() - particle->pdgMass());
                hEnergyLab[i]->Fill(particle->e());
                hFreezeOutXYLab[i]->Fill(x, y);
                hFreezeOutZXLab[i]->Fill(z, x);
                hFreezeOutZTLab[i]->Fill(z, t);
                hProperTimeVsSpaceTimeRapidityLab[i]->Fill(spaceTimeRapidity, tau);
                hFreezeOutTimeLab[i]->Fill(t);
                hProperTimeLab[i]->Fill(tau);
                hEtaLab[i]->Fill(eta);

            } // if (it != pdgCodes.end())


        } // for(Int_t iTrk=0; iTrk<nParticles; iTrk++)

        hRefMult->Fill(refMult);
        hImpactParVsRefMult->Fill(refMult, b);

    } // for(Long64_t iEvent=0; iEvent<events2read; iEvent++)


    // Write histograms to the output file
    TFile *oFile = new TFile(oFileName, "recreate");
    hImpactParameter->Write();
    hRefMult->Write();
    hImpactParVsRefMult->Write();
    // Center-of-mass frame histograms
    for (auto* h : hAcceptanceCMS) h->Write();
    for (auto* h : hEnergyImbalanceCMS) h->Write();
    for (auto* h : hMassImbalanceCMS) h->Write();
    for (auto* h : hPtSpectra) { h->Scale(1./events2read); h->Write(); }
    for (auto* h : hEnergyCMS) { h->Write(); }
    for (auto* h : hFreezeOutXYCMS) h->Write();
    for (auto* h : hFreezeOutZXCMS) h->Write();
    for (auto* h : hFreezeOutZTCMS) h->Write();
    for (auto* h : hProperTimeVsSpaceTimeRapidityCMS) h->Write();
    for (auto* h : hFreezeOutTimeCMS) h->Write();
    for (auto* h : hProperTimeCMS) h->Write();
    for (auto* h : hEtaCMS) h->Write();

    // Laboratory frame histograms
    for (auto* h : hAcceptanceLab) h->Write();
    for (auto* h : hEnergyImbalanceLab) h->Write();
    for (auto* h : hMassImbalanceLab) h->Write();
    for (auto* h : hEnergyLab) { h->Write(); }
    for (auto* h : hFreezeOutXYLab) h->Write();
    for (auto* h : hFreezeOutZXLab) h->Write();
    for (auto* h : hFreezeOutZTLab) h->Write();
    for (auto* h : hProperTimeVsSpaceTimeRapidityLab) h->Write();
    for (auto* h : hFreezeOutTimeLab) h->Write();
    for (auto* h : hProperTimeLab) h->Write();
    for (auto* h : hEtaLab) h->Write();

    hAbundance->Scale(1./events2read); hAbundance->Write();
    oFile->Close();

    myReader->Finish();

    std::cout << "Acceptance and spectra processing is finished" << std::endl;

    return 0;
}
