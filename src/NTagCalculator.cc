#include <cmath>
#include <iostream>
#include <numeric>
#include <string>

#include <geotnkC.h>

#include "NTagEventInfo.hh"
#include "NTagCalculator.hh"

float Dot(const float a[3], const float b[3])
{
    return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}

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
        tVar += (T[iHit]-tMean)*(T[iHit]-tMean) / (nHits-1);

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

float GetDWallInMeanDirection(const std::vector<int>& PMTID, float v[3])
{
    float dWall;
    int nHits = PMTID.size();
    float u[3] = {0., 0., 0.};
    
    // Calculate mean direction
    for (int iHit = 0; iHit < nHits; iHit++) {
        float distFromVertexToPMT;
        float vecFromVertexToPMT[3];
        for (int dim = 0; dim < 3; dim++) {
            vecFromVertexToPMT[dim] = NTagConstant::PMTXYZ[PMTID[iHit]-1][dim] - v[dim];
            distFromVertexToPMT = Norm(vecFromVertexToPMT);
            u[dim] += vecFromVertexToPMT[dim] / distFromVertexToPMT;
        }
    }
    
    float uNorm = Norm(u);
    for (int dim = 0; dim < 3; dim++)
        u[dim] /= uNorm;
    
    float dot = u[0]*v[0] + u[1]*v[1];
    float uSq = u[0]*u[0] + u[1]*u[1];
    float vSq = v[0]*v[0] + v[1]*v[1];
    
    // Calculate distance to barrel and distance to top/bottom
    float distR = (- dot + sqrt(dot*dot - uSq*(vSq-RINTK*RINTK))) / uSq;
    float distZ = u[2] > 0 ? (ZPINTK-v[2])/u[2] : (ZMINTK-v[2])/u[2];

    // Return the smaller
    return distR < distZ ? distR : distZ;
}

float GetOpeningAngle(TVector3 uA, TVector3 uB, TVector3 uC)
{
    double dAB2 = (uA-uB).Mag2();
    double dBC2 = (uB-uC).Mag2();
    double dCA2 = (uC-uA).Mag2();
    
    double r2 = dAB2 * dBC2 * dCA2;
    
    if (r2 == 0) 
        return 0;
    else {
        r2 /= (2*(dAB2*dBC2 + dBC2*dCA2 + dCA2*dAB2) - (dAB2*dAB2 + dBC2*dBC2 + dCA2*dCA2));
        return (180./M_PI) * sinf(sqrt(r2));
    }
}

std::array<float, 4> GetOpeningAngleStats(const std::vector<int>& PMTID, float v[3])
{
    std::vector<float> openingAngles;
    int nHits = PMTID.size();
    int hit[3];
    
    // Pick 3 hits without repetition
    for (        hit[0] = 0;        hit[0] < nHits-2; hit[0]++) {
        for (    hit[1] = hit[0]+1; hit[1] < nHits-1; hit[1]++) {
            for (hit[2] = hit[1]+1; hit[2] < nHits;   hit[2]++) {
                
                // Define an array of three unit vectors
                TVector3 u[3];
                for (int i = 0; i < 3; i++) {
                    
                    // Fill i-th vector from vertex to the hit PMT
                    float i_th_vec[3];
                    for (int dim = 0; dim < 3; dim++) {
                        i_th_vec[dim] = NTagConstant::PMTXYZ[PMTID[hit[i]-1]][dim] - v[dim];
                    }
                    
                    // Get i-th unit vector
                    u[i] = TVector3(i_th_vec).Unit();
                }
                openingAngles.push_back(GetOpeningAngle(u[0], u[1], u[2]));
            }
        }
    }
    
    float mean     = GetMean(openingAngles);
    float median   = GetMedian(openingAngles);
    float stdev    = GetTRMS(openingAngles);
    float skewness = GetSkew(openingAngles);
    
    if (isnan(mean) || isnan(median) || isnan(stdev) || isnan(skewness)) {
        std::cout << "!!!! NaN detected !!!!" << std::endl;
        std::cout << "======================" << std::endl;
        for (auto const& angle: openingAngles) {
            std::cout << angle;
        }
    }
    
    return std::array<float, 4>{mean, median, stdev, skewness};
}

TString GetParticleName(int pid)
{
    if (!pidMap.count(2112)) {
        pidMap[2112] = "n";
        pidMap[2212] = "p";
        pidMap[22] = "gamma";
        pidMap[11] = "e-";
        pidMap[100045] = "d";
    }
    if (pidMap.count(pid))
        return pidMap[pid];
    else 
        return TString(std::to_string(pid));
}

TString GetInteractionName(int lmec)
{
    if (!intMap.count(18)) {
        intMap[18] = "Capture";
        intMap[7]  = "Compt.";
        intMap[9]  = "Brems.";
        intMap[10] = "Delta";
        intMap[11] = "Annihi.";
        intMap[12] = "Hadr.";
    }
    if (intMap.count(lmec))
        return intMap[lmec];
    else 
        return TString(std::to_string(lmec));
}