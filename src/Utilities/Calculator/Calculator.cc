#include <cmath>
#include <iostream>
#include <numeric>
#include <string>

#include <geotnkC.h>

#include "SKLibs.hh"

#include "PMTHit.hh"
#include "PMTHitCluster.hh"
#include "Calculator.hh"

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

float GetRMS(const std::vector<float>& vec)
{
    float N  = static_cast<float>(vec.size());
    float mean = 0.;
    float var  = 0.;

    for (auto const& value: vec)
        mean += value / N;
    for (auto const& value: vec)
        var += (value-mean)*(value-mean) / (N-1);

    return sqrt(var);
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

float GetOpeningAngle(TVector3 uA, TVector3 uB, TVector3 uC)
{
    // sides of the triangle formed by the three unit vectors
    double a = (uA-uB).Mag();
    double b = (uC-uA).Mag();
    double c = (uB-uC).Mag();

    // circumradius of the triangle
    double r = a*b*c / sqrt((a+b+c)*(-a+b+c)*(a-b+c)*(a+b-c));

    if (r >= 1)
        return 90.; // prevents NaN
    else
        return (180./M_PI) * asin(r);
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
    float stdev    = GetRMS(openingAngles);
    float skewness = GetSkew(openingAngles);


    return std::array<float, 4>{mean, median, stdev, skewness};
}

float GetDWall(TVector3 vtx)
{
    float vertex[3] = {(float)vtx.x(), (float)vtx.y(), (float)vtx.z()};
    return wallsk_(vertex);
}


float GetDWallInDirection(TVector3 vtx, TVector3 dir)
{
    dir = dir.Unit();

    float dot = vtx.Dot(dir) - vtx.z()*dir.z();
    float dirSq = dir.Perp2(); float vtxSq = vtx.Perp2();

    // Calculate distance to barrel and distance to top/bottom
    float distR = (-dot + sqrt(dot*dot + dirSq*(RINTK*RINTK - vtxSq))) / dirSq;
    float distZ = dir.z() > 0 ? (ZPINTK-vtx.z())/dir.z() : (ZMINTK-vtx.z())/dir.z();

    // Return the smaller
    return distR < distZ ? distR : distZ;
}

TVector3 FindTRMSMinimizingVertex(PMTHitCluster& pmtHitCluster, float INITGRIDWIDTH, float MINGRIDWIDTH, float GRIDSHRINKRATE, float VTXSRCRANGE)
{
    TVector3 originalVertex = pmtHitCluster.GetVertex();

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
                    pmtHitCluster.SetVertex(gridPoint);
                    tRMS = pmtHitCluster.Find(HitFunc::T, Calc::RMS);

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

    pmtHitCluster.SetVertex(originalVertex);

    return minGridPoint;
}

TString GetParticleName(int pid)
{
    if (!pidMap.count(2112)) {
        pidMap[2112]   = "n";
        pidMap[2212]   = "p";
        pidMap[22]     = "gamma";
        pidMap[11]     = "e-";
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