#include <cmath>
#include <iostream>

#include "NTagCalculator.hh"

float Norm(const float vec[3])
{
    return sqrt(vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2]);
}

float Norm(float x, float y, float z)
{
    return sqrt(x*x + y*y + z*z);
}

float GetDistance(const float vec1[3], const float vec2[3])
{
    float tmpVec[3];

    for (int i = 0; i < 3; i++)
        tmpVec[i] = vec1[i] - vec2[i];

    return Norm(tmpVec);
}

float GetLegendreP(int i, float& x)
{
    float result = 0.;

    switch (i) {
        case 1:
            result = x; break;
        case 2:
            result = (3*x*x-1)/2.; break;
        case 3:
            result = (5*x*x*x-3*x)/2; break;
        case 4:
            result = (35*x*x*x*x-30*x*x+3)/8.; break;
        case 5:
            result = (63*x*x*x*x*x-70*x*x*x+15*x)/8.; break;
    }

    return result;
}

float GetTRMS(const std::vector<float>& T)
{
    int   nHits  = T.size();
    float tMean = 0.;
    float tVar  = 0.;

    for (int iHit = 0; iHit < nHits; iHit++)
        tMean += T[iHit] / nHits;
    for (int iHit = 0; iHit < nHits; iHit++)
        tVar += (T[iHit]-tMean)*(T[iHit]-tMean) / nHits;

    return sqrt(tVar);
}

std::vector<float> GetVectorFromStartIndex(const std::vector<float>& sortedT, int startIndex, float tWidth)
{
    std::vector<float> selectedT;
    selectedT.push_back(sortedT[startIndex]);

    unsigned int searchIndex = (unsigned int)startIndex + 1;

    while (searchIndex < sortedT.size() && sortedT[searchIndex] - sortedT[startIndex] < tWidth) {
        selectedT.push_back(sortedT[searchIndex]);
        searchIndex++;
    }

    return selectedT;
}

int GetNhitsFromStartIndex(const std::vector<float>& sortedT, int startIndex, float tWidth)
{
    std::vector<float> selectedT = GetVectorFromStartIndex(sortedT, startIndex, tWidth);
    return selectedT.size();
}

float GetQSumFromStartIndex(const std::vector<float>& sortedT, const std::vector<float>& Q, int startIndex, float tWidth)
{
    std::vector<float> selectedT = GetVectorFromStartIndex(sortedT, startIndex, tWidth);
    float sumQ      = 0.;

    for (unsigned int iHit = 0; iHit < selectedT.size(); iHit++) {
        sumQ += Q[iHit];
    }

    return sumQ;
}

float GetTRMSFromStartIndex(const std::vector<float>& sortedT, int startIndex, float tWidth)
{
    std::vector<float> selectedT = GetVectorFromStartIndex(sortedT, startIndex, tWidth);

    return GetTRMS(selectedT);
}

int GetNhitsFromCenterTime(const std::vector<float>& T, float centerTime, float tWidth)
{
    int NXX = 0;

    for (const auto& t: T) {
        if (t < centerTime - tWidth/2.) continue;
        if (t > centerTime + tWidth/2.) break;
        NXX++;
    }

    return NXX;
}