#include "McDstQA.h"

ClassImp(McDstQA);


McDstQA::McDstQA(const char *input_file, const char *output_file)
{
  // Open output file.
  ofile = new TFile(output_file, "RECREATE");
  assert(ofile != nullptr);

  cut = nullptr;

  // Initialize histograms.
  hImpactPar = new TH1F("hImpactPar", ";b [fm];#frac{dN_{events}}{db}",
                        180, 0., 18.);
  hRefMult05 = new TH1F("hRefMult05", ";RefMult_{|#eta| < 0.5};#frac{dN_{events}}{d(RefMult_{|#eta| < 0.5)}}",
                        1000, 0., 1000.);
  hRefMult10 = new TH1F("hRefMult10", ";RefMult_{|#eta| < 1.0};#frac{dN_{events}}{d(RefMult_{|#eta| < 1.0)}}",
                        1000, 0., 1000.);
  hNTracks = new TH1F("hNTracks", ";N_{tracks};#frac{dN_{events}}{dN_{tracks}}",
                      1000, 0., 1000.);
  hSph05 = new TH1F("hSph05", ";S_{#prep}^{|#eta| < 0.5};#frac{dN_{events}}{dS_{#prep}^{|#eta| < 0.5}}",
                    100, 0., 1.0);
  hSph10 = new TH1F("hSph10", ";S_{#prep}^{|#eta| < 1.0};#frac{dN_{events}}{dS_{#prep}^{|#eta| < 1.0}}",
                    100, 0., 1.0);

  hPx = new TH1F("hPx", ";p_{x} (GeV/c);#frac{dN_{tracks}}{dp_{x}}",
                 512, -5., 5.);
  hPy = new TH1F("hPy", ";p_{y} (GeV/c);#frac{dN_{tracks}}{dp_{y}}",
                 512, -5., 5.);
  hPz = new TH1F("hPz", ";p_{z} (GeV/c);#frac{dN_{tracks}}{dp_{z}}",
                 512, -5., 5.);
  hEta = new TH1F("hEta", ";#eta;#frac{dN_{tracks}}{d#eta}",
                  2000, -10., 10.);
  hPdg = new TH1F("hPdg", ";pdg;#frac{dN_{tracks}}{d(pdg)}",
                  9, 0, 9.);
  TAxis *axis = hPdg->GetXaxis();
  assert(axis != nullptr);
  axis->SetBinLabel(1, "#pi^{-}");
  axis->SetBinLabel(2, "#pi^{+}");
  axis->SetBinLabel(3, "#pi^{0}");
  axis->SetBinLabel(4, "K^{-}");
  axis->SetBinLabel(5, "K^{+}");
  axis->SetBinLabel(6, "p");
  axis->SetBinLabel(7, "#bar{p}");
  axis->SetBinLabel(8, "K^{0}_{S}");
  axis->SetBinLabel(9, "K^{0}_{L}");

  hMSqrVsP = new TH2F("hMSqrVsP", ";Q*p charge*(GeV/c);m^{2} (GeV^{2}/c^{4})",
                      512, -5., 5.,
                      256, -1, 3.);

  // Create reader.
  reader = new McDstReader(input_file);
  assert(reader != nullptr);
  reader->Init();

  reader->SetStatus("*",0);
  reader->SetStatus("Event",1);
  reader->SetStatus("Particle",1);

  // Check TChain and get number of entries.
  assert(reader->chain() != nullptr);
  nEvents = reader->chain()->GetEntries();
  std::cout << "Number of events to read: " << nEvents << std::endl;
}

McDstQA::~McDstQA()
{
  ofile->cd();
  ofile->Write();
  delete ofile;
  delete reader;
}

void McDstQA::setMcDstCut(McDstCut *ptr)
{
  cut = ptr;
}


void McDstQA::run(int nev)
{
  if (nev < 0)
  {
    nev = nEvents;
  }
  else if (nev > nEvents)
  {
    std::cout << "Warning: nev > nEvents => nev = nEvents\n";
    nev = nEvents;
  }

  for (int iev = 0; iev < nev; ++iev)
  {
    int refmult05 = 0, refmult10 = 0;
    int nTracks = 0;

    if (!reader->loadEntry(iev))
    {
      std::cout << "mcReader->loadEntry(iEvent) == false. Skip event.\n";
      continue;
    }

    McDst *dst = reader->mcDst();
    if (dst == nullptr)
    {
      std::cout << "McDst == nullptr. Skip event.\n";
      continue;
    }
    McEvent *event = dst->event();
    if (event == nullptr)
    {
      std::cout << "McEvent == nullptr. Skip event.\n";
      continue;
    }

    nTracks = dst->numberOfParticles();

    // For transverse sphericity calculation.
    TMatrixTSym<double> matrix05(2);
    TMatrixTSym<double> matrix10(2);
    matrix05.Zero();
    matrix10.Zero();
    double pTsum05 = 0.;
    double pTsum10 = 0.;

    for (int itr = 0; itr < nTracks; ++itr)
    {
      McParticle *track = dst->particle(itr);
      int pdg = track->pdg();
      const TLorentzVector &momentum = track->momentum();

      // Calculate reference multiplicity and transverse sphericity.
      if (track->charge() != 0 && momentum.Pt() > 0.1)
      {
        if (fabs(momentum.Eta()) <= 1.0)
        {
          ++refmult10;

          // Transverse sphericity calculation.
          (matrix10)(0, 0) += momentum.Px() * momentum.Px() / momentum.Pt();
          (matrix10)(1, 1) += momentum.Py() * momentum.Py() / momentum.Pt();
          (matrix10)(0, 1) += momentum.Px() * momentum.Py() / momentum.Pt();
          (matrix10)(1, 0) += momentum.Px() * momentum.Py() / momentum.Pt();
          pTsum10 += momentum.Pt();
        }
        if (fabs(momentum.Eta()) <= 0.5)
        {
          ++refmult05;

          (matrix05)(0, 0) += momentum.Px() * momentum.Px() / momentum.Pt();
      	  (matrix05)(1, 1) += momentum.Py() * momentum.Py() / momentum.Pt();
      	  (matrix05)(0, 1) += momentum.Px() * momentum.Py() / momentum.Pt();
      	  (matrix05)(1, 0) += momentum.Px() * momentum.Py() / momentum.Pt();
      	  pTsum05 += momentum.Pt();
        }
      }

      // Track cut.
      if (cut != nullptr &&
          !cut->isGoodParticle(momentum.Eta(), momentum.Pt(), pdg))
      {
        continue;
      }

      // M square vs Q*p. Px, Py, Pz.
      hMSqrVsP->Fill(track->charge()*momentum.P(), momentum.M2());
      hPx->Fill(momentum.Px());
      hPy->Fill(momentum.Py());
      hPz->Fill(momentum.Pz());
      hEta->Fill(momentum.Eta());
      // PDG code.
      switch (pdg)
      {
      case 211:
        hPdg->Fill("#pi^{+}", 1.);
        break;
      case -211:
        hPdg->Fill("#pi^{-}", 1.);
        break;
      case 2212:
        hPdg->Fill("p", 1.);
        break;
      case -2212:
        hPdg->Fill("#bar{p}", 1.);
        break;
      case 321:
        hPdg->Fill("K^{+}", 1.);
        break;
      case -321:
        hPdg->Fill("K^{-}", 1.);
        break;
      case 111:
        hPdg->Fill("#pi^{0}", 1.);
        break;
      case 130:
        hPdg->Fill("K^{0}_{L}", 1.);
        break;
      case 310:
        hPdg->Fill("K^{0}_{S}", 1.);
        break;
      }
    }
    // Calculate eigen values for matrices.
    matrix05 *= 1./pTsum05;
    matrix10 *= 1./pTsum10;
    TMatrixDSymEigen eigenEstimator05(matrix05);
    TMatrixDSymEigen eigenEstimator10(matrix10);
    TVectorD eigen05 = eigenEstimator05.GetEigenValues();
    TVectorD eigen10 = eigenEstimator10.GetEigenValues();

    hSph05->Fill(2. * eigen05.Min() / ( eigen05[0] + eigen05[1] ));
    hSph10->Fill(2. * eigen10.Min() / ( eigen10[0] + eigen10[1] ));
    hImpactPar->Fill(event->impact());
    hRefMult05->Fill(refmult05);
    hRefMult10->Fill(refmult10);
    hNTracks->Fill(nTracks);
  }
}
