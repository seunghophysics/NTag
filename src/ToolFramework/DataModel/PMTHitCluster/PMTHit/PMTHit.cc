#include "PMTHit.hh"

PMTHit::PMTHit(float t, float q, int i)
: T(t), Q(q), I(i), ToF(0), pmtPosition(NTagConstant::PMTXYZ[i-1]) {}

PMTHit::PMTHit(PMTHit const& hit)
: T(hit.t()), Q(hit.q()), I(hit.i()), ToF(hit.GetToF()), pmtPosition(hit.GetPosition()), hitDirection(hit.GetDirection()) {}

PMTHit operator+(const PMTHit& hit1, const PMTHit& hit2)
{
    return PMTHit(hit1.t()+hit2.t(), hit1.q(), hit2.i());
}

PMTHit operator-(const PMTHit& hit1, const PMTHit& hit2)
{
    return PMTHit(hit1.t()-hit2.t(), hit1.q(), hit2.i());
}