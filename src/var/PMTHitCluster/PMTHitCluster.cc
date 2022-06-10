#include <algorithm>
#include <numeric>
#include <cassert>
#include <limits>

#include <TTree.h>

#include <skheadC.h>
#include <tqrealroot.h>
#undef MAXPM
#undef MAXPMA
#include <geotnkC.h>

#include "Calculator.hh"
#include "PMTHitCluster.hh"

PMTHitCluster::PMTHitCluster()
:fIsSorted(false), fHasVertex(false) {}

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

PMTHitCluster::PMTHitCluster(sktqaz_common sktqaz)
:PMTHitCluster()
{
    for (int iHit=0; iHit<sktqaz.nhitaz; iHit++) {
        PMTHit hit{ /*T*/ sktqaz.taskz[iHit],
                    /*Q*/ sktqaz.qaskz[iHit],
                    /*I*/ sktqaz.icabaz[iHit],
                    /*F*/ sktqaz_.ihtflz[iHit]
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
    if ((1 <= i && i <= MAXPM) || (20001 <= i && i <= 20000+MAXPMA)) {
        fElement.push_back(hit);
        //fIndex.push_back(fElement.size());
    }
}

void PMTHitCluster::Append(const PMTHitCluster& hitCluster, bool inGateOnly)
{
    for (auto const& hit: hitCluster) {
        if (!inGateOnly || hit.f() & (1<<1))
            Append(hit);
    }
}

void PMTHitCluster::Clear()
{
    //*this = PMTHitCluster();
    fElement.clear();
    fIsSorted = false;
    fHasVertex = false;
    fVertex = TVector3();
    fMeanDirection = TVector3();
    ClearBranches();
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
    if (!fHasVertex || fVertex != inVertex) {
        if (fHasVertex) RemoveVertex();

        fVertex = inVertex;
        fHasVertex = true;

        SetToF();
        Sort();
    }
}

void PMTHitCluster::RemoveVertex()
{
    if (fHasVertex) {
        bool unset = true;
        SetToF(unset);

        fVertex = TVector3();
        fHasVertex = false;

        fIsSorted = false;
    }
}

void PMTHitCluster::FindMeanDirection()
{
    auto dirVec = GetProjection(HitFunc::Dir);
    fMeanDirection = GetMean(dirVec).Unit();
}

void PMTHitCluster::SetToF(bool unset)
{
    if (!fHasVertex)
        std::cerr << "WARNING: Vertex is not set for PMTHitCluster in " << this
                  << ", skipping ToF-subtraction..."<< std::endl;
    else {
        fIsSorted = false;
        for (auto& hit: fElement) {
            if (unset)
                hit.UnsetToFAndDirection();
            else
                hit.SetToFAndDirection(fVertex);
        }
    }
}

void PMTHitCluster::Sort()
{
    std::sort(fElement.begin(), fElement.end(), [](const PMTHit& hit1, const PMTHit& hit2) {return hit1.t() < hit2.t();} );
    fIsSorted = true;
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

void PMTHitCluster::FillCommon()
{
    int nHits = GetSize();
    int nIDHits = 0;
    int nODHits = 0;

    for (auto const& hit: fElement) {
        if (hit.i() >= 20000) nODHits++;
        if (hit.i() <  MAXPM) nIDHits++;
    }

    if (nIDHits) {
        sktqz_.nqiskz = nIDHits;
        rawtqinfo_.nqisk_raw = nIDHits;
        rawtqinfo_.pc2pe_raw = 2.46; // SK5

        int iIDHit = 0;
        for (auto const& hit: fElement) {
            if (hit.i() <  MAXPM) {
                sktqz_.tiskz[iIDHit] = hit.t();
                sktqz_.qiskz[iIDHit] = hit.q();
                sktqz_.icabiz[iIDHit] = hit.i();
                sktqz_.ihtiflz[iIDHit] = hit.f()<<16;
                rawtqinfo_.icabbf_raw[iIDHit] = hit.i() + (hit.f()<<16);
                rawtqinfo_.tbuf_raw[iIDHit] = hit.t() + (skheadqb_.it0xsk - skheadqb_.it0sk) / COUNT_PER_NSEC;
                rawtqinfo_.qbuf_raw[iIDHit] = hit.q();
                iIDHit++;
            }
        }
    }

    if (nODHits) {
        sktqaz_.nhitaz = nODHits;
        rawtqinfo_.nhitaz_raw = nODHits;
        rawtqinfo_.pc2pe_raw = 2.46; // SK5

        int iODHit = 0;
        for (auto const& hit: fElement) {
            if (hit.i() >= 20000) {
                sktqaz_.taskz[iODHit] = hit.t();
                sktqaz_.qaskz[iODHit] = hit.q();
                sktqaz_.icabaz[iODHit] = hit.i();
                sktqaz_.ihtflz[iODHit] = hit.f()<<16;
                rawtqinfo_.icabaz_raw[iODHit] = hit.i() + (hit.f()<<16);
                rawtqinfo_.taskz_raw[iODHit] = hit.t() + (skheadqb_.it0xsk - skheadqb_.it0sk) / COUNT_PER_NSEC;
                rawtqinfo_.qaskz_raw[iODHit] = hit.q();
                iODHit++;
            }
        }
    }
}

PMTHitCluster PMTHitCluster::Slice(int startIndex, Float tWidth)
{
    return SliceRange(fElement[startIndex].t(), 0, tWidth);
    /*
    if (!fIsSorted) Sort();

    PMTHitCluster selectedHits;
    if (fHasVertex)
        selectedHits.SetVertex(fVertex);

    unsigned int searchIndex = (unsigned int)startIndex;
    unsigned int nHits = fElement.size();

    while (searchIndex < nHits && fElement[searchIndex].t() - fElement[startIndex].t() < tWidth) {
        selectedHits.Append(fElement[searchIndex]);
        searchIndex++;
    }

    return selectedHits;
    */
}

PMTHitCluster PMTHitCluster::Slice(int startIndex, Float lowT, Float upT)
{
    return SliceRange(fElement[startIndex].t(), lowT, upT);
}

PMTHitCluster PMTHitCluster::SliceRange(Float startT, Float lowT, Float upT)
{
    if (!fIsSorted) Sort();

    if (lowT > upT)
        std::cerr << "PMTHitCluster::Slice : lower bound is larger than upper bound." << std::endl;

    PMTHit startHit(startT, 0, 1, 1);
    unsigned int low = GetLowerBoundIndex(startT + lowT);
    unsigned int up  = GetUpperBoundIndex(startT + upT);

    PMTHitCluster selectedHits;
    if (fHasVertex)
        selectedHits.SetVertex(fVertex);

    for (unsigned int iHit = low; iHit <= up; iHit++)
        selectedHits.Append(fElement[iHit]);

    return selectedHits;
}

PMTHitCluster PMTHitCluster::SliceRange(Float lowT, Float upT)
{
    return SliceRange(Float(0), lowT, upT);
}

unsigned int PMTHitCluster::GetIndex(PMTHit hit)
{
    bool isFound = false;
    unsigned int i = 0;
    for (i=0; i<GetSize(); i++) {
        if (fabs(hit.t() - fElement[i].t()) < 1 &&
            fabs(hit.q() - fElement[i].q()) < 1e-5 &&
            hit.i() == fElement[i].i()) {
            isFound = true;
            break;
        }
    }

    if (!isFound)
        std::cerr << "PMTHitCluster::GetIndex: Could not find the right index for the given hit "
                  << "t: " << hit.t() << " q: " << hit.q() << " i: " << hit.i()
                  << ", returning the max index...\n";
    return i;
}

void PMTHitCluster::AddTimeOffset(Float tOffset)
{
    for (auto& hit: fElement)
        hit = hit + tOffset;
}

void PMTHitCluster::ApplyDeadtime(Float deadtime)
{
    TVector3 tempVertex;
    bool bHadVertex = false;
    if (fHasVertex) {
        tempVertex = fVertex;
        RemoveVertex();
        bHadVertex = true;
    }

    std::array<Float, MAXPM+1>  IDHitTime;
    std::array<Float, MAXPMA+1> ODHitTime;
    IDHitTime.fill(std::numeric_limits<Float>::lowest());
    ODHitTime.fill(std::numeric_limits<Float>::lowest());

    std::vector<PMTHit> dtCorrectedHits;

    if (!fIsSorted) Sort();
    for (auto const& hit: fElement) {
        int hitPMTID = hit.i();
        if (1 <= hitPMTID && hitPMTID <= MAXPM) {
            if (hit.t() - IDHitTime[hitPMTID] > deadtime) {
                dtCorrectedHits.push_back(hit);
                IDHitTime[hitPMTID] = hit.t();
            }
        }
        else if (20001 <= hitPMTID && hitPMTID <= 20000+MAXPMA) {
            hitPMTID -= 20000;
            if (hit.t() - ODHitTime[hitPMTID] > deadtime) {
                dtCorrectedHits.push_back(hit);
                ODHitTime[hitPMTID] = hit.t();
            }
        }
    }

    fElement = dtCorrectedHits;

    if (bHadVertex)
        SetVertex(tempVertex);
}

std::array<float, 6> PMTHitCluster::GetBetaArray()
{
    std::array<float, 6> beta = {0., 0., 0., 0., 0., 0.};
    int nHits = fElement.size();

    if (!fHasVertex) {
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

    std::vector<int> perm(nHits);
    std::iota(perm.begin(), perm.end(), 0);
    Shuffle(perm);

    int MAXNCOMBOS = 20000;
    int nCombos = 0;
    // Pick 3 hits without repetition
    for (        hit[0] = 0;        hit[0] < nHits-2; hit[0]++) {
        for (    hit[1] = hit[0]+1; hit[1] < nHits-1; hit[1]++) {
            for (hit[2] = hit[1]+1; hit[2] < nHits;   hit[2]++) {
                openingAngles.push_back(GetOpeningAngle(fElement[perm[hit[0]]].GetDirection(),
                                                        fElement[perm[hit[1]]].GetDirection(),
                                                        fElement[perm[hit[2]]].GetDirection()));
                nCombos++;
                if (nCombos >= MAXNCOMBOS) goto calc;
            }
        }
    }

    calc:
    OpeningAngleStats stats;

    stats.mean     = GetMean(openingAngles);
    stats.median   = GetMedian(openingAngles);
    stats.stdev    = GetRMS(openingAngles);
    stats.skewness = GetSkew(openingAngles);

    return stats;
}

/*
TVector3 PMTHitCluster::FindTRMSMinimizingVertex(float INITGRIDWIDTH, float MINGRIDWIDTH, float GRIDSHRINKRATE, float VTXMAXRADIUS)
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
                    if (gridPoint.Mag() > VTXMAXRADIUS) continue;

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
*/

void PMTHitCluster::SetAsSignal(bool b)
{
    for (auto& hit: fElement) {
        hit.SetSignalFlag(b);
    }
}

float PMTHitCluster::GetSignalRatio()
{
    float sigSum = 0;
    for (auto& hit: fElement)
        sigSum += hit.s();

    return sigSum / GetSize();
}

//void PMTHitCluster::FindHitProperties()
//{
//    FindMeanDirection();
//
//    for (auto& hit: fElement) {
//        hit.FindMinAngle(this);
//        hit.FindDirAngle(fMeanDirection);
//        hit.FindAcceptance();
//    }
//}

PMTHitCluster PMTHitCluster::Slice(std::function<float(const PMTHit&)> lambda, float min, float max) const
{
    PMTHitCluster newCluster;

    for (auto const& hit: fElement) {
        if (min < lambda(hit) && lambda(hit) < max)
            newCluster.Append(hit);
    }

    return newCluster;
}

void PMTHitCluster::ApplyCut(std::function<float(const PMTHit&)> lambda, float min, float max)
{
    for (unsigned int iHit=0; iHit<GetSize(); iHit++) {
        auto& hit = fElement[iHit];
        if (min > lambda(hit) || lambda(hit) > max)
            fElement.erase(fElement.begin()+iHit);
    }
}

void PMTHitCluster::MakeBranches()
{
    if (fIsOutputTreeSet) {
        fOutputTree->Branch("t", &fT);
        fOutputTree->Branch("q", &fQ);
        fOutputTree->Branch("i", &fI);
        //fOutputTree->Branch("x", &fX);
        //fOutputTree->Branch("y", &fY);
        //fOutputTree->Branch("z", &fZ);
        fOutputTree->Branch("s", &fS);
    }
}

void PMTHitCluster::ClearBranches()
{
    fT.clear(); fQ.clear(); fI.clear(); fS.clear();
    //fX.clear(); fY.clear(); fZ.clear();
}

void PMTHitCluster::FillTree()
{
    if (fIsOutputTreeSet) {
        ClearBranches();
        auto vertex = fVertex;
        RemoveVertex();
        Sort();
        for (auto const hit: fElement) {
            auto hitPos = hit.GetPosition();
            fT.push_back(hit.t());
            fQ.push_back(hit.q());
            fI.push_back(hit.i());
            //fX.push_back(hitPos.x());
            //fY.push_back(hitPos.y());
            //fZ.push_back(hitPos.z());
            fS.push_back(hit.s());
        }
        fOutputTree->Fill();
        SetVertex(vertex);
    }
}

PMTHitCluster& PMTHitCluster::operator+=(const Float& time)
{
    AddTimeOffset(time);
    return *this;
}

PMTHitCluster& PMTHitCluster::operator-=(const Float& time)
{
    AddTimeOffset(-time);
    return *this;
}

PMTHitCluster operator+(const PMTHitCluster& hitCluster, const Float& time)
{
    PMTHitCluster newCluster = hitCluster;
    newCluster += time;
    return newCluster;
}

PMTHitCluster operator-(const PMTHitCluster& hitCluster, const Float& time)
{
    PMTHitCluster newCluster = hitCluster;
    newCluster -= time;
    return newCluster;
}
