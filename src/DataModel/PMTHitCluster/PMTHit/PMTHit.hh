#ifndef PMTHIT_HH
#define PMTHIT_HH

#include <iostream>
#include <functional>

#include <TVector3.h>

#include <skparmC.h>
#include <geopmtC.h>

class PMTHitCluster;

namespace NTagConstant{
    static const float (*PMTXYZ)[3] = geopmt_.xyzpm; /*!< An array of PMT coordinates.
                                                          Index 0 for x, 1 for y, 2 for z-coordinates. [cm] */
    static const float C_WATER = 21.5833; ///< The speed of light in pure water. [cm/ns]
}

class PMTHit
{
    public:
        PMTHit(): T(0), Q(0), I(0), S(0), ToF(0), pmtPosition(), hitDirection() {}
        PMTHit(float t, float q, int i);
        PMTHit(PMTHit const& hit);

        inline const float& t() const { return T; }
        inline const float& q() const { return Q; }
        inline const unsigned int& i() const { return I; }
        inline const bool& s() const {return S; }

        inline void SetSignalFlag(bool b) { S=b; }
        inline void Dump() const { std::cout << "T: " << T << " Q: " << Q << " I: " << I << "\n"; }

        inline void SetToFAndDirection(const TVector3& vertex)
        {
            TVector3 displacement = pmtPosition - vertex;
            hitDirection = displacement.Unit();
            ToF = displacement.Mag() / NTagConstant::C_WATER;
            T -= ToF;
        }

        inline void UnsetToFAndDirection()
        {
            T += ToF;
            ToF = 0;
            hitDirection = TVector3();
        }

        inline const float& GetToF() const { return ToF; }
        inline const TVector3& GetDirection() const { return hitDirection; }
        inline const TVector3& GetPosition() const { return pmtPosition; }

        inline bool operator<(const PMTHit &hit) const { return T < hit.t(); }

    private:
        TVector3 pmtPosition;
        TVector3 hitDirection;
        float T, Q, ToF;
        unsigned int I;
        bool S;
        
    ClassDef(PMTHit, 1)
};

PMTHit operator+(const PMTHit& hit1, const PMTHit& hit2);
PMTHit operator-(const PMTHit& hit1, const PMTHit& hit2);

namespace HitFunc
{
    const std::function<float(const PMTHit&)> T = [](const PMTHit& hit)->float { return hit.t(); };
    const std::function<float(const PMTHit&)> Q = [](const PMTHit& hit)->float { return hit.q(); };
    const std::function<TVector3(const PMTHit&)> Dir = [](const PMTHit& hit)->TVector3 { return hit.GetDirection(); };
}

#endif