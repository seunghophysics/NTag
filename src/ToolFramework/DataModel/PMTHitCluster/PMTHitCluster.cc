#include <algorithm>
#include <cassert>

#include "Calculator.hh"
#include "PMTHitCluster.hh"

PMTHitCluster::PMTHitCluster()
:bSorted(false), bHasVertex(false) { nElements = 0; }

void PMTHitCluster::Append(const PMTHit& hit)
{ 
    int i = hit.i(); 
    if (1 <= i && i <= MAXPM) // append only hits with meaningful PMT ID
        Cluster::Append(hit);
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
        for (auto& hit: vElements) {
            if (unset)
                hit.UnsetToFAndDirection();
            else
                hit.SetToFAndDirection(vertex);
        }
    }
}

void PMTHitCluster::Sort()
{
    std::sort(vElements.begin(), vElements.end());
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

    while (searchIndex < nElements && vElements[searchIndex].t() - vElements[startIndex].t() < tWidth) {
        selectedHits.Append(vElements[searchIndex]);
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

    int low = std::lower_bound(vElements.begin(), vElements.end(), vElements[startIndex] + PMTHit(lowT, 0, 0)) - vElements.begin();
    int up = std::upper_bound(vElements.begin(), vElements.end(), vElements[startIndex] + PMTHit(upT, 0, 0)) - vElements.begin();

    PMTHitCluster selectedHits;
    if (bHasVertex)
        selectedHits.SetVertex(vertex);

    for (int iHit = low; iHit <= up; iHit++)
        selectedHits.Append(vElements[iHit]);

    return selectedHits;
}

std::vector<float> PMTHitCluster::T()
{
    std::vector<float> output;
    for (auto const& hit: vElements) {
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
            float cosTheta = vElements[i].GetDirection().Dot(vElements[j].GetDirection());
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
                openingAngles.push_back(GetOpeningAngle(vElements[hit[0]].GetDirection(),
                                                        vElements[hit[1]].GetDirection(),
                                                        vElements[hit[2]].GetDirection()));
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