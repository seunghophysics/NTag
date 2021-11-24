#include "PMTHit.hh"

PMTHit::PMTHit(Float t, float q, int i, int f)
: fT(t), fToF(0), fQ(q), fPMTID(i), fFlag(f), fIsSignal(false)
{
    if (1 <= fPMTID && fPMTID <= MAXPM) {
        fPMTPosition = TVector3(NTagConstant::PMTXYZ[fPMTID-1]);
    }
    else {
        fPMTPosition = TVector3();
    }
}

PMTHit& PMTHit::operator+=(const Float& time)
{
    fT += time;
    return *this;
}

PMTHit& PMTHit::operator-=(const Float& time)
{
    fT -= time;
    return *this;
}

PMTHit operator+(const PMTHit& hit, const Float& time)
{
    PMTHit newHit = hit;
    newHit += time;
    return newHit;
}

PMTHit operator-(const PMTHit& hit, const Float& time)
{
    PMTHit newHit = hit;
    newHit -= time;
    return newHit;
}