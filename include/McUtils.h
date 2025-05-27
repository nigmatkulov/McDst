#ifndef McUtils_h
#define McUtils_h

// C++ headers
#include <cmath>

// ROOT headers
#include "TLorentzVector.h"

class McUtils {
public:

    /// Nucleon mass (in GeV)
    static const double M_NUCLEON;

    /** 
     * Convert total energy in the lab frame to the center of mass frame
     * Etot: total energy in GeV
     * mProj: mass of the projectile in GeV
     * mTarg: mass of the target in GeV
     */
    static double s_from_Etot(double Etot, double mProj, double mTarg);

    /** 
     * Convert total energy in the lab frame to the center of mass frame
     * Etot: total energy in GeV
     * AProj: mass number of the projectile
     * ATarg: mass number of the target
     */
    static double s_from_Etot(double Etot, int AProj, int ATarg);

    /** 
     * Convert kinetic energy in the lab frame to the center of mass frame
     * Ekin: kinetic energy in GeV
     * mProj: mass of the projectile in GeV
     * mTarg: mass of the target in GeV
     */
    static double s_from_Ekin(double Ekin, double mProj, double mTarg);

    /** 
     * Convert kinetic energy in the lab frame to the center of mass frame
     * Ekin: kinetic energy in GeV
     * A1: mass number of the projectile
     * A2: mass number of the target
     */
    static double s_from_Ekin(double Ekin, int AProj, int ATarg);

    /** 
     * Convert lab momentum to center of mass energy
     * plab: lab momentum in GeV/c
     * mProj: mass of the projectile in GeV
     * mTarg: mass of the target in GeV
     */
    static double s_from_plab(double plab, double mProj, double mTarg);

    /** 
     * Convert lab momentum to center of mass energy
     * plab: lab momentum in GeV/c
     * AProj: mass number of the projectile
     * ATarg: mass number of the target
     */
    static double s_from_plab(double plab, int AProj, int ATarg);

    /** 
     * Convert kinetic energy in the lab frame to the velocity in the center of mass frame
     * Ekin: kinetic energy in GeV
     * A_beam: mass number of the beam
     * A_target: mass number of the target
     */
    static double beta_from_Ekin(double Ekin, int A_beam, int A_target);

    /** 
     * Convert beam momentum to velocity in the center of mass frame
     * p_beam: beam momentum in GeV/c
     * A_beam: mass number of the beam
     * A_target: mass number of the target
     */
    static double beta_from_plab(double p_beam, int A_beam, int A_target);

    /** 
     * Boost a Lorentz vector in the z direction
     * p: Lorentz vector to be boosted
     * beta: boost velocity (v/c)
     */
    static TLorentzVector boost_z(const TLorentzVector &p, double beta);

    /** 
     * Boost a Lorentz vector in the z direction from CMS to lab frame
     * p: Lorentz vector to be boosted
     * beta: boost velocity (v/c)
     */
    static TLorentzVector boostToLabFrame(const TLorentzVector &pCMS, double beta);

    /** 
     * Boost a Lorentz vector in the z direction from CMS to lab frame
     * p: Lorentz vector to be boosted
     * beta: boost velocity (v/c)
     */
    static TLorentzVector boostToCMSFrame(const TLorentzVector &pLab, double beta);

    /** 
     * Center-of-mass rapidity from beam momentum
     */
    static double yCM_from_pBeam(const double &pBeam);

    /** 
     * Center-of-mass rapidity from beam energy
     */
    static double yCM_from_EBeam(const double &eBeam);

    /** 
     * Center-of-mass rapidity from kinetic energy
     */
    static double yCM_from_Ekin(const double &eKin);

    /** 
     * Beam rapidity from beam momentum
     */
    static double yBeam_from_pBeam(const double &pBeam);

    /** 
     * Beam rapidity from beam energy
     */
    static double yBeam_from_EBeam(const double &eBeam);

    /** 
     * Beam rapidity from kinetic energy
     */
    static double yBeam_from_Ekin(const double &eKin);
};

#endif // #define McUtils_h