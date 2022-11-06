#include <algorithm>
#include <numeric>
#include <cassert>
#include <cmath>
#include <limits>
#include <iomanip>

#include <TTree.h>
#include <TMath.h>

#include <skheadC.h>
#include <tqrealroot.h>
#include <skbadcC.h>
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
                    /*F*/ sktqz.ihtiflz[iHit],
                    /*S*/ sktqz.ihtiflz[iHit]&(1<<12)
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
                    /*F*/ sktqaz.ihtflz[iHit],
                    /*S*/ sktqaz.ihtflz[iHit]&(1<<12)
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
    //else
    //    std::cerr << "[PMTHitCluster] " << hit.i() << " at t=" << hit.t() << " ns is not a valid PMT cable ID!\n";
}

void PMTHitCluster::Append(const PMTHitCluster& hitCluster, bool inGateOnly)
{
    for (auto const& hit: hitCluster) {
        if (inGateOnly) {
            if (hit.f() & (1<<1)) {
                Append(hit);
            }
            //else {
            //    std::cerr << "[PMTHitCluster] PMT ID " << hit.i() << " at t=" << hit.t() << " ns not in gate!\n";
            //}
        }
        else
            Append(hit);
    }
}

bool PMTHitCluster::AppendByCoincidence(PMTHitCluster& hitCluster)
{
    auto lastHit = GetLastHit();
    Float tSharedLowerBound = lastHit.t() - 100;
    Float tSharedUpperBound = lastHit.t() + 100;

    Sort();
    hitCluster.Sort();

    bool doAppend = false;
    for (auto const& hit: hitCluster) {
        if (doAppend) Append(hit);
        Float hitT = hit.t();
        if ((tSharedLowerBound < hitT) && (hitT < tSharedUpperBound) && (hit.i() == lastHit.i())) {
            doAppend = true;
        }
    }

    //if (!doAppend) {
    //    std::cerr << "PMTHitCluster::AppendByCoincidence WARNING: coincidence hit not found!\n";
    //    std::cerr << "LastHit: \n";
    //    lastHit.Dump();
    //    std::cerr << "Added cluster within [-500,+500] ns range: \n";
    //    hitCluster.SliceRange(lastHit.t()-500, lastHit.t()+500).DumpAllElements();
    //}

    return doAppend;
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

HitReductionResult PMTHitCluster::RemoveHits(std::function<bool(const PMTHit&)> lambda, Float tMin, Float tMax)
{
    auto cut = [=](PMTHit const & hit){ return (tMin<hit.t()) && (hit.t()<tMax) && lambda(hit); };

    HitReductionResult res;
    res.title        = "";
    res.tMin         = tMin;
    res.tMax         = tMax;
    res.nBeforeWhole = GetSize();
    res.nBeforeRange = CountRange(tMin, tMax);
    res.nMatch       = CountIf(lambda);
    res.nRemoved     = std::count_if(fElement.begin(), fElement.end(), cut);

    fElement.erase(std::remove_if(fElement.begin(), fElement.end(), cut), fElement.end());

    res.nAfterWhole = GetSize();
    int nActuallyRemoved = res.nBeforeWhole - res.nAfterWhole;
    assert(res.nRemoved == nActuallyRemoved);

    res.nAfterRange = CountRange(tMin, tMax);

    return res;
}

HitReductionResult PMTHitCluster::RemoveBadChannels(Float tMin, Float tMax)
{
    HitReductionResult res = {.nRemoved=0};

    if (fElement.empty()) return res;

    auto idCut = [](PMTHit const & hit){ return (hit.i() > MAXPM) ||
                                                (combad_.ibad[hit.i()-1] > 0) ||
                                                (comdark_.dark_rate[hit.i()-1] == 0); };
    auto odCut = [](PMTHit const & hit){ return (hit.i() < 20000) || (hit.i() > 20000+MAXPMA) ||
                                                (combada_.ibada[hit.i()-20000-1] > 0) ||
                                                (comdark_.dark_rate_od[hit.i()-20000-1] == 0); };
    auto cut = (fElement.at(0).i()<=MAXPM) ? idCut : odCut;

    res = RemoveHits(cut, tMin, tMax);
    res.title = "Bad PMTs";
    
    return res;
}

HitReductionResult PMTHitCluster::RemoveNegativeHits(Float tMin, Float tMax)
{
    HitReductionResult res = RemoveHits([](PMTHit const & hit){ return (hit.q()<0); }, tMin, tMax);
    res.title = "Q < 0";
    return res;
}

HitReductionResult PMTHitCluster::RemoveLargeQHits(float qThreshold, Float tMin, Float tMax)
{
    HitReductionResult res = RemoveHits([=](PMTHit const & hit){ return (hit.q()>qThreshold); }, tMin, tMax);
    res.title = Form("Q > %3.2f", qThreshold);
    return res;
}

unsigned int PMTHitCluster::CountIf(std::function<bool(const PMTHit&)> lambda)
{
    return std::count_if(fElement.begin(), fElement.end(), lambda);
}

unsigned int PMTHitCluster::CountRange(Float tMin, Float tMax)
{
    return CountIf([=](PMTHit const & hit){ return (tMin<hit.t()) && (hit.t()<tMax); });
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
        tqreal->cables.push_back(hit.i() + (hit.f()<<16) + (hit.s()<<28));
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
        if (hit.i() <= MAXPM) nIDHits++;
    }

    if (nIDHits) {
        sktqz_.nqiskz = nIDHits;
        rawtqinfo_.nqisk_raw = nIDHits;
        rawtqinfo_.pc2pe_raw = 2.46; // SK5

        int iIDHit = 0;
        for (auto const& hit: fElement) {
            if (hit.i() <=  MAXPM) {
                sktqz_.tiskz[iIDHit] = hit.t();
                sktqz_.qiskz[iIDHit] = hit.q();
                sktqz_.icabiz[iIDHit] = hit.i();
                sktqz_.ihtiflz[iIDHit] = hit.f() + (hit.s()<<12);
                if (479.2 < hit.t() && hit.t() < 1779.2)
                    sktqz_.ihtiflz[iIDHit] |= 1;
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
                sktqaz_.ihtflz[iODHit] = hit.f() + (hit.s()<<12);
                if (479.2 < hit.t() && hit.t() < 1779.2)
                    sktqaz_.ihtflz[iODHit] |= 1;
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
    return SliceRange(fElement.at(startIndex).t(), 0, tWidth);
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
    return SliceRange(fElement.at(startIndex).t(), lowT, upT);
}

PMTHitCluster PMTHitCluster::SliceRange(Float startT, Float lowT, Float upT)
{
    PMTHitCluster selectedHits;
    if (fElement.empty())
        return selectedHits;

    if (!fIsSorted) Sort();

    if (lowT > upT)
        std::cerr << "PMTHitCluster::Slice : lower bound is larger than upper bound." << std::endl;

    PMTHit startHit(startT, 0, 1, 1);
    unsigned int low = GetLowerBoundIndex(startT + lowT);
    unsigned int up  = GetUpperBoundIndex(startT + upT);

    if (fHasVertex)
        selectedHits.SetVertex(fVertex);

    for (unsigned int iHit = low; iHit <= up; iHit++)
        selectedHits.Append(fElement.at(iHit));

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
        if (fabs(hit.t() - fElement.at(i).t()) < 1 &&
            fabs(hit.q() - fElement.at(i).q()) < 1e-5 &&
            hit.i() == fElement.at(i).i()) {
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

HitReductionResult PMTHitCluster::ApplyDeadtime(Float deadtime, bool doRemove)
{
    HitReductionResult res;
    res.title        = Form("%3.0f ns deadtime", deadtime);
    res.tMin         = std::numeric_limits<Float>::infinity();
    res.tMax         = std::numeric_limits<Float>::infinity();
    res.nBeforeWhole = GetSize();
    res.nBeforeRange = res.nBeforeWhole;

    TVector3 tempVertex;
    bool bHadVertex = false;
    if (fHasVertex) {
        tempVertex = fVertex;
        RemoveVertex();
        bHadVertex = true;
    }

    std::array<Float, 20000+MAXPMA+1> HitTime;
    std::array<bool, 20000+MAXPMA+1> HitType;
    HitTime.fill(std::numeric_limits<Float>::lowest());
    HitType.fill(0);

    //std::array<Float, MAXPM+1>  IDHitTime;
    //std::array<Float, MAXPMA+1> ODHitTime;
    //IDHitTime.fill(std::numeric_limits<Float>::lowest());
    //ODHitTime.fill(std::numeric_limits<Float>::lowest());

    std::vector<PMTHit> dtCorrectedHits;

    //if (!fIsSorted) Sort();
    Sort();
    res.nRemovedBySignal = 0;
    for (auto& hit: fElement) {
        int hitPMTID = hit.i();
        Float tDiff = hit.t() - HitTime[hitPMTID];
        hit.SetTDiff(tDiff);
        if (!doRemove || tDiff>deadtime) {
            //hit.SetBurstFlag(tDiff<deadtime);
            dtCorrectedHits.push_back(hit);
            HitTime[hitPMTID] = hit.t();
            HitType[hitPMTID] = hit.s();
        }
        else if (hit.s()) {
            //std::cout << "Removing signal hit within deadtime " << deadtime << " ns: "; hit.Dump();
            res.nRemovedBySignal++;
        }
        else if (HitType[hitPMTID]) {
            //std::cout << "Removing noise hit due to signal within deadtime " << deadtime << " ns: "; hit.Dump();
            res.nRemovedBySignal++;
        }
        //else {
        //    HitTime[hitPMTID] = hit.t();
        //    HitType[hitPMTID] = hit.s();
        //    std::cout << "Removing noise hit due to noise within deadtime " << deadtime << " ns: "; hit.Dump();
        //}
    }

    res.nAfterRange     = dtCorrectedHits.size();
    res.nAfterWhole     = res.nAfterRange;
    res.nRemoved        = res.nBeforeRange - res.nAfterRange;
    res.nMatch          = res.nRemoved;
    res.nRemovedByNoise = res.nRemoved - res.nRemovedBySignal;

    fElement = dtCorrectedHits;

    if (bHadVertex)
        SetVertex(tempVertex);

    //if (doRemove/*&& nRemovedHits*/) {
        //std::cout << Form("[ApplyDeadtime]      Total %6d -> %6d hits: "
        //                  "removed %6d hits in time range [  -inf,    inf] usec (%d due to signal)\n",
        //                  allSize, GetSize(), nRemovedHits, nRemovedBySignal, deadtime);
    //    std::cout << std::setw(21) << std::left << Form("[ %4.0f ns deadtime ] ", deadtime);
    //    std::cout << Form("Removed %6d hits in time range [  -inf,    inf] usec: Total %6d -> %6d hits\n",
    //                      nRemovedHits, allSize, GetSize())//;

        //std::cout << "[ApplyDeadtime]      Removed " << nRemovedHits << Form(" ( %d due to signal ) ", nRemovedBySignal)
        //          << "hits for PMT deadtime " << deadtime << " ns\n";
    //}

    //return std::array<unsigned int,2>({nRemovedBySignal, nRemovedByNoise});
    return res;
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
            float cosTheta = fElement.at(i).GetDirection().Dot(fElement.at(j).GetDirection());
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
                /*
                auto hit0pos = fElement[perm[hit[0]]].GetPosition();
                auto hit1pos = fElement[perm[hit[1]]].GetPosition();
                auto hit2pos = fElement[perm[hit[2]]].GetPosition();

                float hit0x = hit0pos.x(); float hit0y = hit0pos.y(); float hit0z = hit0pos.z();
                float hit1x = hit1pos.x(); float hit1y = hit1pos.y(); float hit1z = hit1pos.z();
                float hit2x = hit2pos.x(); float hit2y = hit2pos.y(); float hit2z = hit2pos.z();

                auto hit0dir = fElement[perm[hit[0]]].GetDirection();
                auto hit1dir = fElement[perm[hit[1]]].GetDirection();
                auto hit2dir = fElement[perm[hit[2]]].GetDirection();

                float hitdir0x = hit0dir.x(); float hitdir0y = hit0dir.y(); float hitdir0z = hit0dir.z();
                float hitdir1x = hit1dir.x(); float hitdir1y = hit1dir.y(); float hitdir1z = hit1dir.z();
                float hitdir2x = hit2dir.x(); float hitdir2y = hit2dir.y(); float hitdir2z = hit2dir.z();

                auto vx = fVertex.x(); auto vy = fVertex.y(); auto vz = fVertex.z();
                */
                openingAngles.push_back(GetOpeningAngle(fElement.at(perm[hit[0]]).GetDirection(),
                                                        fElement.at(perm[hit[1]]).GetDirection(),
                                                        fElement.at(perm[hit[2]]).GetDirection()));
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

    assert(!std::isnan(stats.skewness));

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

void PMTHitCluster::SetBurstFlag(float tBurstWidth)
{
    for (auto& hit: fElement) {
        hit.SetBurstFlag(hit.dt()<tBurstWidth);
    }
}

unsigned int PMTHitCluster::GetNSignal()
{
    unsigned int sigSum = 0;
    for (auto& hit: fElement)
        sigSum += hit.s();
    return sigSum;
}

unsigned int PMTHitCluster::GetNBurst()
{
    unsigned int burSum = 0;
    for (auto& hit: fElement)
        burSum += hit.b();
    return burSum;
}

unsigned int PMTHitCluster::GetNNoisyPMT()
{
    unsigned int nNoisyPMT = 0;
    for (auto const& hit: fElement) {
        if (comdark_.dark_rate[hit.i()-1] > comdark_.dark_ave) nNoisyPMT++;
    }
    return nNoisyPMT;
}

float PMTHitCluster::GetSignalRatio()
{
    return float(GetNSignal()) / float(GetSize());
}

float PMTHitCluster::GetBurstRatio()
{
    return float(GetNBurst()) / float(GetSize());
}

float PMTHitCluster::GetBurstSignificance(float tBurstWindow)
{
    int obs = GetNBurst();
    float exp = 0;
    float flatDarkRatio = 0.5;
    for (auto const& hit: fElement) {
        exp += comdark_.dark_rate[hit.i()-1] * flatDarkRatio * tBurstWindow * 1e-6;
    }
    return (obs-exp)/sqrt(exp);
}

float PMTHitCluster::GetDarkLikelihood()
{
    float darkLLH = 1;
    for (auto const& hit: fElement) {
        float ratio = comdark_.dark_rate[hit.i()-1] / comdark_.dark_ave;
        darkLLH *= ratio;
        //if (ratio>0) darkLLH *= ratio;
    }

    return Sigmoid(std::log(darkLLH));
}

float PMTHitCluster::GetNoisyPMTRatio()
{
    return GetNNoisyPMT() / float(GetSize());
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
        auto& hit = fElement.at(iHit);
        if (min > lambda(hit) || lambda(hit) > max)
            fElement.erase(fElement.begin()+iHit);
    }
}

void PMTHitCluster::MakeBranches()
{
    if (fIsOutputTreeSet) {
        fOutputTree->Branch("t", &fT);
        fOutputTree->Branch("tof", &fToF);
        fOutputTree->Branch("q", &fQ);
        fOutputTree->Branch("i", &fI);
        fOutputTree->Branch("dt", &fDT);
        //fOutputTree->Branch("x", &fX);
        //fOutputTree->Branch("y", &fY);
        //fOutputTree->Branch("z", &fZ);
        fOutputTree->Branch("s", &fS);
        fOutputTree->Branch("b", &fB);
        fOutputTree->Branch("n", &fTag);
    }
}

void PMTHitCluster::ClearBranches()
{
    fT.clear(); fToF.clear(); fDT.clear(); fQ.clear(); fI.clear(); fS.clear(); fB.clear(); fTag.clear();
    //fX.clear(); fY.clear(); fZ.clear();
}

void PMTHitCluster::FillTree(bool asResidual)
{
    if (fIsOutputTreeSet) {
        ClearBranches();
        auto vertex = fVertex;
        if (!asResidual) RemoveVertex();
        Sort();
        for (auto const hit: fElement) {
            auto hitPos = hit.GetPosition();
            fT.push_back(hit.t());
            fToF.push_back(hit.GetToF());
            fDT.push_back(hit.dt());
            fQ.push_back(hit.q());
            fI.push_back(hit.i());
            //fX.push_back(hitPos.x());
            //fY.push_back(hitPos.y());
            //fZ.push_back(hitPos.z());
            fS.push_back(hit.s());
            fB.push_back(hit.b());
            fTag.push_back(hit.n());
        }
        fOutputTree->Fill();
        if (!asResidual) SetVertex(vertex);
    }
}

void PMTHitCluster::CheckNaN()
{
    for (auto const& hit: fElement) {
        float t = hit.t();
        float q = hit.q();
        assert(!std::isnan(t));
        assert(!std::isnan(q));
        assert(!std::isinf(t));
        assert(!std::isinf(q));
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
