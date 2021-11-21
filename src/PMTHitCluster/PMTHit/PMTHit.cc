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

PMTHit operator+(const PMTHit& hit, const Float& time)
{
    return PMTHit(hit.t()+time, hit.q(), hit.i(), hit.f());
}

PMTHit operator-(const PMTHit& hit, const Float& time)
{
    return PMTHit(hit.t()-time, hit.q(), hit.i(), hit.f());
}