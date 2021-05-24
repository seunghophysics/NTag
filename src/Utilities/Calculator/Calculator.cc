#include <cmath>
#include <iostream>
#include <numeric>
#include <string>
#include <limits>

#include <geotnkC.h>

#include "SKLibs.hh"

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
    // make sure the inputs are unit vectors
    // uA = uA.Unit(); uB = uB.Unit(); uC = uC.Unit();
    
    // sides of the triangle formed by the three unit vectors
    double a = (uA-uB).Mag();
    double b = (uC-uA).Mag();
    double c = (uB-uC).Mag();
    
    if (a*b*c == 0) {
        double angleAB = (180./M_PI) * uA.Angle(uB)/2.;
        double angleAC = (180./M_PI) * uA.Angle(uC)/2.;
        return uA.Angle(uB) == 0 ? (uA.Angle(uC) == 0 ? 0 : uA.Angle(uC)) : uA.Angle(uB); 
    }
    
    else {
        // circumradius of the triangle
        double r = a*b*c / sqrt((a+b+c)*(-a+b+c)*(a-b+c)*(a+b-c));
    
        if (r>=1)
            return 90.; // prevents NaN
        else
            return (180./M_PI) * asin(r);
    }
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