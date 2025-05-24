#include "McUtils.h"

// const double McUtils::M_NUCLEON = 0.931494;
const double McUtils::M_NUCLEON = 0.938272029;

//________________
double McUtils::s_from_Etot(double Etot, double mProj, double mTarg) {
    return mProj * mProj + mTarg * mTarg + 2 * mTarg * Etot;
}

//________________
double McUtils::s_from_Etot(double Etot, int aProj, int aTarg) {
    double mProj = aProj * M_NUCLEON;
    double mTarg = aTarg * M_NUCLEON;
    return s_from_Etot(Etot, mProj, mTarg);
}

//________________
double McUtils::s_from_Ekin(double Ekin, double mProj, double mTarg) {
    double Etot = Ekin + mProj;
    return s_from_Etot(Etot, mProj, mTarg);
}

//________________
double McUtils::s_from_Ekin(double Ekin, int AProj, int ATarg) {
    double mProj = AProj * M_NUCLEON;
    double mTarg = ATarg * M_NUCLEON;
    return s_from_Ekin(Ekin, mProj, mTarg);
}

//________________
double McUtils::s_from_plab(double plab, double mProj, double mTarg) {
    return mProj * mProj + mTarg * mTarg + 2 * mTarg * std::sqrt(mProj * mProj + plab * plab);
}

//________________
double McUtils::s_from_plab(double plab, int AProj, int ATarg) {
    double mProj = AProj * M_NUCLEON;
    double mTarg = ATarg * M_NUCLEON;
    return s_from_plab(plab, mProj, mTarg);
}

//________________
double McUtils::beta_from_Ekin(double Ekin, int AProj, int ATarg) {
    // Beam energy and momentum
    double EProj = AProj * (M_NUCLEON + Ekin);
    double MProj = AProj * M_NUCLEON;
    double pProj = sqrt(EProj * EProj - MProj * MProj);

    // Target energy (at rest)
    double MTarg = ATarg * M_NUCLEON;
    double ETarg = MTarg;

    // Center-of-mass boost velocity
    double beta_cm = pProj / (EProj + ETarg);
    return beta_cm;
}

//________________
double McUtils::beta_from_plab(double pProj, int AProj, int ATarg) {
    // Beam energy and momentum
    double MProj = AProj * M_NUCLEON;
    double EProj = sqrt(pProj * pProj + MProj * MProj);

    // Target energy (at rest)
    double MTarg = ATarg * M_NUCLEON;
    double ETarg = MTarg;

    // Center-of-mass boost velocity
    double beta_cm = pProj / (EProj + ETarg);
    return beta_cm;
}

//________________
TLorentzVector McUtils::boost_z(const TLorentzVector &p, double beta) {
    // Boost vector
    double gamma = 1.0 / sqrt(1.0 - beta * beta);
    double pz = p.Pz();
    double E = p.E();
    double pz_new = gamma * (pz - beta * E);
    double E_new = gamma * (E - beta * pz);
    
    // Create new Lorentz vector
    TLorentzVector p_new(p.Px(), p.Py(), pz_new, E_new);
    return p_new;
}

//________________
TLorentzVector McUtils::boostToLabFrame(const TLorentzVector &pCMS, double beta) {
    return boost_z(pCMS, beta);
}

//________________
TLorentzVector McUtils::boostToCMSFrame(const TLorentzVector &pLab, double beta) {
    return boost_z(pLab, -beta);
}