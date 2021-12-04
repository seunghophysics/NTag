#ifndef PMTHIT_HH
#define PMTHIT_HH

#include <iostream>
#include <functional>

#include <TVector3.h>

#include <skparmC.h>
#include <geopmtC.h>

#include "Calculator.hh"

#ifdef USE_DOUBLE
typedef double Float;
#else
typedef float Float;
#endif

class PMTHitCluster;

namespace NTagConstant{
    static const float (*PMTXYZ)[3] = geopmt_.xyzpm; /*!< An array of PMT coordinates.
                                                          Index 0 for x, 1 for y, 2 for z-coordinates. [cm] */
    static const float C_WATER = 21.5833; ///< The speed of light in pure water. [cm/ns]
}

class PMTHit
{
    public:
        PMTHit(): fT(0), fToF(0), fQ(0), fPMTID(0), fFlag(-1), fIsSignal(false), fMinAngle(0), fDirAngle(0), fAcceptance(0) {}
        PMTHit(Float t, float q, int i, int f);

        inline const Float& t() const { return fT; }
        inline const float& q() const { return fQ; }
        inline const unsigned int& i() const { return fPMTID; }
        inline const int& f() const { return fFlag; }
        inline const bool& s() const { return fIsSignal; }

        inline void SetSignalFlag(bool b) { fIsSignal=b; }
        inline void Dump() const { std::cout << "T: " << fT << " Q: " << fQ << " I: " << fPMTID << " F: " << fFlag << " ToF: " << fToF << "\n"; }

        void SetToFAndDirection(const TVector3& vertex)
        {
            fT += fToF;
            TVector3 displacement = fPMTPosition - vertex;
            fHitDirection = displacement.Unit();
            fToF = displacement.Mag() / NTagConstant::C_WATER;
            fT -= fToF;
        }

        inline void UnsetToFAndDirection()
        {
            fT += fToF;
            fToF = 0;
            fHitDirection = TVector3();
        }

        inline const Float& GetToF() const { return fToF; }
        inline const TVector3& GetDirection() const { return fHitDirection; }
        inline const TVector3& GetPosition() const { return fPMTPosition; }

        inline bool operator<(const PMTHit &hit) const { return fT < hit.t(); }

        void FindMinAngle(PMTHitCluster* cluster);
        void FindDirAngle(TVector3 vec);
        void FindAcceptance();
        inline const float& GetMinAngle() const { return fMinAngle; }
        inline const float& GetDirAngle() const { return fDirAngle; }
        inline const float& GetAcceptance() const { return fAcceptance; }

        PMTHit& operator+=(const Float& time);
        PMTHit& operator-=(const Float& time);

    protected:
        Float fT, fToF;
        float fQ;
        unsigned int fPMTID;
        int fFlag;
        bool fIsSignal;
        TVector3 fPMTPosition;
        TVector3 fHitDirection;
        
        float fMinAngle;
        float fDirAngle;
        float fAcceptance;

    //ClassDef(PMTHit, 1)
};

PMTHit operator+(const PMTHit& hit, const Float& time);
PMTHit operator-(const PMTHit& hit, const Float& time);

namespace HitFunc
{
    const std::function<float(const PMTHit&)> T = [](const PMTHit& hit)->float { return hit.t(); };
    const std::function<float(const PMTHit&)> Q = [](const PMTHit& hit)->float { return hit.q(); };
    const std::function<int(const PMTHit&)> I = [](const PMTHit& hit)->int { return hit.i(); };
    const std::function<int(const PMTHit&)> MinAngle = [](const PMTHit& hit)->int { return hit.GetMinAngle(); };
    const std::function<int(const PMTHit&)> DirAngle = [](const PMTHit& hit)->int { return hit.GetDirAngle(); };
    const std::function<int(const PMTHit&)> Acceptance = [](const PMTHit& hit)->int { return hit.GetAcceptance(); };
    const std::function<TVector3(const PMTHit&)> Dir = [](const PMTHit& hit)->TVector3 { return hit.GetDirection(); };
}

#endif