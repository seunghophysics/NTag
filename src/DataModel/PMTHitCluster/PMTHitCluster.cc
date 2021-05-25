#include <algorithm>
#include <cassert>

#include <geotnkC.h>

#include "Calculator.hh"
#include "PMTHitCluster.hh"

PMTHitCluster::PMTHitCluster()
:bSorted(false), bHasVertex(false) { nElements = 0; }

void PMTHitCluster::Append(const PMTHit& hit)
{ 
    int i = hit.i(); 
    //hit.Dump();
    // append only hits with meaningful PMT ID
    if (1 <= i && i <= MAXPM) {
        element.push_back(hit); nElements++;
    } 
}

void PMTHitCluster::SetVertex(const TVector3& inVertex)
{
    if (bHasVertex)
        RemoveVertex();

    vertex = inVertex; bHasVertex = true;
    SetToF();
}

void PMTHitCluster::RemoveVertex()
{
    if (bHasVertex) {
        bool unset = true;
        SetToF(unset);
        vertex = TVector3(); bHasVertex = false;
    }
}

void PMTHitCluster::SetToF(bool unset)
{
    if (!bHasVertex)
        std::cerr << "WARNING: Vertex is not set for PMTHitCluster in " << this
                  << ", skipping ToF-subtraction..."<< std::endl;
    else {
        for (auto& hit: element) {
            if (unset)
                hit.UnsetToFAndDirection();
            else
                hit.SetToFAndDirection(vertex);
        }
    }
}

void PMTHitCluster::Sort()
{
    std::sort(element.begin(), element.end());
    bSorted = true;
}

PMTHitCluster PMTHitCluster::Slice(int startIndex, float tWidth)
{
    if (!bSorted)
        Sort();

    PMTHitCluster selectedHits;
    if (bHasVertex)
        selectedHits.SetVertex(vertex);

    unsigned int searchIndex = (unsigned int)startIndex;

    while (searchIndex < nElements && element[searchIndex].t() - element[startIndex].t() < tWidth) {
        selectedHits.Append(element[searchIndex]);
        searchIndex++;
    }

    return selectedHits;
}

PMTHitCluster PMTHitCluster::Slice(int startIndex, float lowT, float upT)
{
    if (!bSorted)
        Sort();

    if (lowT > upT)
        std::cerr << "PMTHitCluster::Slice : lower bound is larger than upper bound." << std::endl;

    int low = std::lower_bound(element.begin(), element.end(), element[startIndex] + PMTHit(lowT, 0, 0)) - element.begin();
    int up = std::upper_bound(element.begin(), element.end(), element[startIndex] + PMTHit(upT, 0, 0)) - element.begin();

    PMTHitCluster selectedHits;
    if (bHasVertex)
        selectedHits.SetVertex(vertex);

    for (int iHit = low; iHit <= up; iHit++)
        selectedHits.Append(element[iHit]);

    return selectedHits;
}

std::vector<float> PMTHitCluster::T()
{
    std::vector<float> output;
    for (auto const& hit: element) {
        output.push_back(hit.t());
    }
    return output;
}

std::array<float, 6> PMTHitCluster::GetBetaArray()
{
    std::array<float, 6> beta = {0., 0., 0., 0., 0., 0};
    int nHits = nElements;
    if (!bHasVertex || nHits == 0) abort(); //return beta;

    for (int i = 0; i < nHits-1; i++) {
        for (int j = i+1; j < nHits; j++) {
            // cosine angle between two consecutive uv vectors
            float cosTheta = element[i].GetDirection().Dot(element[j].GetDirection());
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
    int nHits = nElements;
    int hit[3];

    // Pick 3 hits without repetition
    for (        hit[0] = 0;        hit[0] < nHits-2; hit[0]++) {
        for (    hit[1] = hit[0]+1; hit[1] < nHits-1; hit[1]++) {
            for (hit[2] = hit[1]+1; hit[2] < nHits;   hit[2]++) {
                openingAngles.push_back(GetOpeningAngle(element[hit[0]].GetDirection(),
                                                        element[hit[1]].GetDirection(),
                                                        element[hit[2]].GetDirection()));
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

    float minTRMS = 9999.;
    float tRMS;

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
                    if (displacement.Perp() > RINTK || abs(displacement.z()) > ZPINTK) continue;

                    // Skip grid point further away from the maximum search range
                    if (displacement.Mag() > VTXSRCRANGE) continue;

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