#include <cmath>
#include <iostream>
#include <numeric>
#include <string>
#include <limits>

#include <TRandom3.h>
#include <TCollection.h>
#include <TList.h>
#include <TSystemDirectory.h>
#include <TSystemFile.h>

#include <geotnkC.h>

#include "Calculator.hh"

std::default_random_engine c_ranGen;
TRandom3 ranGen;


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
        //double angleAB = (180./M_PI) * uA.Angle(uB)/2.;
        //double angleAC = (180./M_PI) * uA.Angle(uC)/2.;
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

unsigned int GetMinIndex(std::vector<float>& vec)
{
    std::vector<float>::iterator iter = std::min_element(vec.begin(), vec.end());
    size_t index = std::distance(vec.begin(), iter);
    return index;
}

unsigned int GetMaxIndex(std::vector<float>& vec)
{
    std::vector<float>::iterator iter = std::max_element(vec.begin(), vec.end());
    size_t index = std::distance(vec.begin(), iter);
    return index;
}

void SetSeed(int seed)
{
    c_ranGen.seed(seed);
    ranGen.SetSeed(seed);
}

TString PickFile(TString dirPath, const char* extension)
{
    std::vector<TString> list = GetListOfFiles(dirPath, extension);
    return PickRandom(list);
}

TString PickSubdirectory(TString dirPath)
{
    std::vector<TString> list = GetListOfSubdirectories(dirPath);
    return PickRandom(list);
}

std::vector<TString> GetListOfFiles(TString dirPath, const char* extension, bool recursive)
{
    std::vector<TString> list;

    TSystemDirectory dir(dirPath, dirPath);
    TList *files = dir.GetListOfFiles();

    if (files) {
        TSystemFile *file;
        TString fileName, filePath;
        TIter next(files);
        while ((file = (TSystemFile*)next())) {
            fileName = file->GetName();
            filePath = file->GetTitle();
            if (recursive && file->IsDirectory() && fileName != "." && fileName != "..") {
                std::cout << "Checking files in directory: " << filePath << std::endl;
                std::vector<TString> subdirList = GetListOfFiles(filePath, extension);
                list.insert(list.end(), subdirList.begin(), subdirList.end());
            }
            if (!file->IsDirectory() && fileName.EndsWith(extension)) {
                list.push_back(filePath + "/" + fileName);
            }
        }
    }

    return list;
}

std::vector<TString> GetListOfSubdirectories(TString dirPath)
{
    std::vector<TString> list;

    TSystemDirectory dir(dirPath, dirPath);
    TList *files = dir.GetListOfFiles();

    if (files) {
        TSystemFile *file;
        TString fileName, filePath;
        TIter next(files);
        while ((file = (TSystemFile*)next())) {
            fileName = file->GetName();
            filePath = file->GetTitle();
            if (file->IsDirectory() && fileName != "." && fileName != "..") {
                list.push_back(filePath);
            }
        }
    }

    return list;
}

std::vector<unsigned int> GetRangeIndex(const std::vector<double>& sortedVec, double low, double high)
{
    std::vector<unsigned int> vIndex;
    //std::cout << "low: " << low << " high: " << high << std::endl;

    auto start = std::lower_bound(sortedVec.begin(), sortedVec.end(), low);
    auto end   = std::upper_bound(sortedVec.begin(), sortedVec.end(), high);

    //std::cout << "start_i: " << start-sortedVec.begin() << " end_i: " << end-sortedVec.begin() << std::endl;

    for (auto it=start; it!=end; ++it) {
        vIndex.push_back(it-sortedVec.begin());
    }

    return vIndex;
}

std::vector<std::pair<float, int>> Histogram(std::vector<float> vec, int nBins, float min, float max)
{
    std::vector<std::pair<float, int>> hist(nBins, std::pair<float, int>(0, 0));
    const float bWidth = (max - min) / float(nBins);

    std::sort(vec.begin(), vec.end());
    
    int index = 0;
    auto binStartItr = vec.begin();
    for (auto& bin: hist) {
        const float bMin = min + index*bWidth;
        const float bMax = min + (index+1)*bWidth;

        auto start = std::lower_bound(binStartItr, vec.end(), bMin);
        auto end = std::lower_bound(start, vec.end(), bMax*0.99999);
        binStartItr = end;
        
        bin.first = (bMin+bMax)/2.; // bin center
        bin.second = end - start;   // bin value
        index++;
    }
    
    return hist;
}

std::vector<std::string> Split(std::string target, std::string delim)
{
    std::vector<std::string> v;

    if (!target.empty()) {
        while (target.size() ){
            int index = target.find(delim);
            if (index != std::string::npos) {
                v.push_back(target.substr(0,index));
                target = target.substr(index+delim.size());
                if (target.size()==0) v.push_back(target);
            }
            else {
                v.push_back(target);
                target = "";
            }
        }         
    }
    return v;
}