#include <algorithm>
#include <cassert>
#include <limits>

#include <TTree.h>

#include <tqrealroot.h>
#undef MAXPM
#undef MAXPMA
#include <geotnkC.h>

#include "Calculator.hh"
#include "PMTHitCluster.hh"

PMTHitCluster::PMTHitCluster()
:bSorted(false), bHasVertex(false) {}

PMTHitCluster::PMTHitCluster(sktqz_common sktqz)
:PMTHitCluster()
{
    for (int iHit=0; iHit<sktqz.nqiskz; iHit++) {
        PMTHit hit{ /*T*/ sktqz.tiskz[iHit],
                    /*Q*/ sktqz.qiskz[iHit],
                    /*I*/ sktqz.icabiz[iHit],
                    /*F*/ sktqz_.ihtiflz[iHit]
                  };
        Append(hit);
    }
}

PMTHitCluster::PMTHitCluster(TQReal* tqreal, int flag)
:PMTHitCluster()
{
    AddTQReal(tqreal, flag);
}

void PMTHitCluster::Append(const PMTHit& hit)
{
    int i = hit.i();

    // append only hits with meaningful PMT ID
    if ((1 <= i && i <= MAXPM) || (20001 <= i && i <= 20000+MAXPMA))
        fElement.push_back(hit);
}

void PMTHitCluster::Append(const PMTHitCluster& hitCluster, bool inGateOnly)
{
    for (auto const& hit: hitCluster) {
        if (!inGateOnly || hit.f() & (1<<1)) 
            Append(hit);
    }
}

void PMTHitCluster::AddTQReal(TQReal* tqreal, int flag)
{
    auto& t = tqreal->T;
    auto& q = tqreal->Q;
    auto& i = tqreal->cables;
    
    for (unsigned int j=0; j<=t.size(); j++) {
        Append({t[j], q[j], i[j]&0x0000FFFF, flag});
    }
}

void PMTHitCluster::SetVertex(const TVector3& inVertex)
{
    if (bHasVertex)
        RemoveVertex();

    vertex = inVertex;
    bHasVertex = true;

    SetToF();
    Sort();
}

void PMTHitCluster::RemoveVertex()
{
    if (bHasVertex) {
        bool unset = true;
        SetToF(unset);

        vertex = TVector3();
        bHasVertex = false;
    }
}

void PMTHitCluster::SetToF(bool unset)
{
    if (!bHasVertex)
        std::cerr << "WARNING: Vertex is not set for PMTHitCluster in " << this
                  << ", skipping ToF-subtraction..."<< std::endl;
    else {
        bSorted = false;
        for (auto& hit: fElement) {
            if (unset)
                hit.UnsetToFAndDirection();
            else
                hit.SetToFAndDirection(vertex);
        }
    }
}

void PMTHitCluster::Sort()
{
    std::sort(fElement.begin(), fElement.end(), [](const PMTHit& hit1, const PMTHit& hit2) {return hit1.t() < hit2.t();} );
    bSorted = true;
}

void PMTHitCluster::FillTQReal(TQReal* tqreal)
{
    tqreal->nhits = GetSize();
    
    // temporary values
    tqreal->pc2pe = 2.46;
    tqreal->tqreal_version = 2;
    tqreal->qbconst_version = 510000;
    tqreal->tqmap_version = 60000;
    tqreal->pgain_version = 50000;
    tqreal->it0xsk = 0;

    tqreal->cables.clear();
    tqreal->T.clear();
    tqreal->Q.clear();

    for (auto const& hit: fElement) {
        tqreal->cables.push_back(hit.i() + (hit.f() << 16));
        tqreal->T.push_back(hit.t());
        tqreal->Q.push_back(hit.q());
    }
}

PMTHitCluster PMTHitCluster::Slice(int startIndex, Float tWidth)
{
    if (!bSorted) Sort();

    PMTHitCluster selectedHits;
    if (bHasVertex)
        selectedHits.SetVertex(vertex);

    unsigned int searchIndex = (unsigned int)startIndex;
    unsigned int nHits = fElement.size();

    while (searchIndex < nHits && fElement[searchIndex].t() - fElement[startIndex].t() < tWidth) {
        selectedHits.Append(fElement[searchIndex]);
        searchIndex++;
    }

    return selectedHits;
}

PMTHitCluster PMTHitCluster::Slice(int startIndex, Float lowT, Float upT)
{
    return SliceRange(fElement[startIndex].t(), lowT, upT);
}

PMTHitCluster PMTHitCluster::SliceRange(Float startT, Float lowT, Float upT)
{
    if (!bSorted) Sort();

    if (lowT > upT)
        std::cerr << "PMTHitCluster::Slice : lower bound is larger than upper bound." << std::endl;
    
    PMTHit startHit(startT, 0, 1, 1);
    unsigned int low = GetLowerBoundIndex(startT + lowT);
    unsigned int up = GetUpperBoundIndex(startT + upT);

    PMTHitCluster selectedHits;
    if (bHasVertex)
        selectedHits.SetVertex(vertex);

    for (unsigned int iHit = low; iHit <= up; iHit++)
        selectedHits.Append(fElement[iHit]);

    return selectedHits;
}

PMTHitCluster PMTHitCluster::SliceRange(Float lowT, Float upT)
{
    return SliceRange(Float(0), lowT, upT);
}

unsigned int PMTHitCluster::GetIndex(Float t)
{
    bool isFound = false;
    unsigned int i = 0;
    for (i=0; i<GetSize(); i++) {
        if (fabs(t-fElement[i].t()) < 1e-5) {
            isFound = true;
            break;
        }
    }

    if (!isFound)
        std::cerr << "PMTHitCluster::GetIndex: Could not find the right index for the given time " << t
                  << ", returning the max index...\n";
    return i;
}

void PMTHitCluster::ApplyDeadtime(Float deadtime)
{
    TVector3 tempVertex;
    bool bHadVertex = false;
    if (bHasVertex) {
        tempVertex = vertex;
        RemoveVertex();
        bHadVertex = true;
    }

    std::array<Float, MAXPM> hitTime;
    hitTime.fill(std::numeric_limits<Float>::min());

    std::vector<PMTHit> dtCorrectedHits;

    if (!bSorted) Sort();
    for (auto const& hit: fElement) {
        int hitPMTID = hit.i();
        if (1 <= hitPMTID && hitPMTID <= MAXPM) {
            if (hit.t() - hitTime[hitPMTID] > deadtime) {
                dtCorrectedHits.push_back(hit);
                hitTime[hitPMTID] = hit.t();
            }
        }
    }

    fElement = dtCorrectedHits;

    if (bHadVertex)
        SetVertex(tempVertex);
}

std::array<float, 6> PMTHitCluster::GetBetaArray()
{
    std::array<float, 6> beta = {0., 0., 0., 0., 0., 0};
    int nHits = fElement.size();

    if (!bHasVertex) {
        std::cerr << "PMTHitCluster::GetBetaArray : the hit cluster has no set vertex. Returning a 0-filled array...\n";
        return beta;
    }

    if (!nHits) {
        std::cerr << "PMTHitCluster::GetBetaArray : the hit cluster is empty. Returning a 0-filled array...\n";
        return beta;
    }

    for (int i = 0; i < nHits-1; i++) {
        for (int j = i+1; j < nHits; j++) {
            // cosine angle between two consecutive uv vectors
            float cosTheta = fElement[i].GetDirection().Dot(fElement[j].GetDirection());
            for (int k = 1; k <= 5; k++)
                beta[k] += GetLegendreP(k, cosTheta);
        }
    }

    for (int k = 1; k <= 5; k++)
        beta[k] = 2.*beta[k] / float(nHits) / float(nHits-1);

    // Return calculated beta array
    return beta;
}

OpeningAngleStats PMTHitCluster::GetOpeningAngleStats()
{
    std::vector<float> openingAngles;
    int nHits = fElement.size();

    int hit[3];

    // Pick 3 hits without repetition
    for (        hit[0] = 0;        hit[0] < nHits-2; hit[0]++) {
        for (    hit[1] = hit[0]+1; hit[1] < nHits-1; hit[1]++) {
            for (hit[2] = hit[1]+1; hit[2] < nHits;   hit[2]++) {
                openingAngles.push_back(GetOpeningAngle(fElement[hit[0]].GetDirection(),
                                                        fElement[hit[1]].GetDirection(),
                                                        fElement[hit[2]].GetDirection()));
            }
        }
    }

    OpeningAngleStats stats;

    stats.mean     = GetMean(openingAngles);
    stats.median   = GetMedian(openingAngles);
    stats.stdev    = GetRMS(openingAngles);
    stats.skewness = GetSkew(openingAngles);

    return stats;
}

TVector3 PMTHitCluster::FindTRMSMinimizingVertex(float INITGRIDWIDTH, float MINGRIDWIDTH, float GRIDSHRINKRATE, float VTXSRCRANGE)
{
    TVector3 originalVertex = vertex;

    float gridWidth = INITGRIDWIDTH;

    // Grid search starts from tank center
    TVector3 gridOrigin(0, 0, 0); // grid origin in the grid search loop (starts at tank center)
    TVector3 minGridPoint;        // temp point to save TRMS-minimizing grid point
    TVector3 gridPoint;           // point in grid to find TRMS

    Float minTRMS = 9999.;
    Float tRMS;

    float gridRLimit = (int)(2*RINTK/gridWidth)*gridWidth/2.;
    float gridZLimit = (int)(2*ZPINTK/gridWidth)*gridWidth/2.;

    // Repeat until grid width gets small enough
    while (gridWidth > MINGRIDWIDTH-0.1) {

        // Allocate coordinates to a grid point
        for (float dx=-gridRLimit; dx<gridRLimit+0.1; dx+=gridWidth) {
            for (float dy=-gridRLimit; dy<gridRLimit+0.1; dy+=gridWidth) {
                for (float dz=-gridZLimit; dz<gridZLimit+0.1; dz+=gridWidth) {
                    TVector3 displacement(dx, dy, dz);
                    gridPoint = gridOrigin + displacement;

                    // Skip grid point out of tank
                    if (gridPoint.Perp() > RINTK || abs(gridPoint.z()) > ZPINTK) continue;

                    // Skip grid point further away from the maximum search range
                    if (gridPoint.Mag() > VTXSRCRANGE) continue;

                    // Subtract ToF from the search vertex
                    SetVertex(gridPoint);
                    tRMS = Find(HitFunc::T, Calc::RMS);

                    // Save TRMS minimizing grid point
                    if (tRMS < minTRMS) {
                        minTRMS = tRMS;
                        minGridPoint = gridPoint;
                    }
                }
            }
        }

        // Change grid origin to the TRMS-minimizing grid point,
        // shorten the grid width,
        // and repeat until grid width gets small enough!
        gridOrigin = minGridPoint;
        gridWidth *= GRIDSHRINKRATE;
        gridRLimit *= GRIDSHRINKRATE;
        gridZLimit *= GRIDSHRINKRATE;
    }

    SetVertex(originalVertex);

    return minGridPoint;
}

PMTHitCluster operator+(const PMTHitCluster& hitCluster, const Float& time)
{
    PMTHitCluster newCluster;
    
    for (auto const& hit: hitCluster) {
        auto newHit = hit + time;
        newCluster.Append(newHit);
    }

    return newCluster;
}

PMTHitCluster operator-(const PMTHitCluster& hitCluster, const Float& time)
{
    PMTHitCluster newCluster;
    
    for (auto const& hit: hitCluster) {
        auto newHit = hit - time;
        newCluster.Append(newHit);
    }

    return newCluster;
}