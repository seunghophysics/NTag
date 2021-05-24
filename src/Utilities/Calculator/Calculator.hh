/*******************************************
*
* @file Calculator.hh
*
* @brief Defines calculator functions which
* are commonly used throughout NTag.
*
********************************************/

#ifndef CALCULATOR_HH
#define CALCULATOR_HH 1

#include <algorithm>
#include <array>
#include <functional>
#include <map>
#include <vector>

#include <TString.h>
#include <TVector3.h>

class PMTHitCluster;

static std::map<int, TString> pidMap;
static std::map<int, TString> intMap;

/**
 * @brief Get dot produt of two arrays.
 * @param vec1 A size-3 float array.
 * @param vec2 A size-3 float array.
 * @return The dot product of the given two arrays.
 */
float Dot(const float vec1[3], const float vec2[3]);

/**
 * @brief Get norm of a size-3 float array.
 * @param vec A size-3 float array.
 * @return The norm of the given array.
 */
float Norm(const float vec[3]);

/**
 * @brief Get norm of the vector with given x, y, z coordinates.
 * @param x X coordinate of a vector.
 * @param y Y coordinate of a vector.
 * @param z Z coordinate of a vector.
 * @return The norm of the vector with given coordinates.
 */
float Norm(float x, float y, float z);

/**
 * @brief Get distance between two points specified by the two given size-3 float arrays.
 * @param vec1 A size-3 float array of coordinates of a vector 1.
 * @param vec2 A size-3 float array of coordinates of a vector 2.
 * @return The distance between vector 1 and 2.
 */
float GetDistance(const float vec1[3], const float vec2[3]);

template <typename T>
T GetSum(const std::vector<T>& vec)
{
    T t{};
    for (auto& element: vec){
        t += element;
    }
    return t;
}

/**
 * @brief Calculates the mean of the value distribution of the given vector.
 * @param vec The vector to calculate mean.
 * @return The mean of the value distribution of \c vec.
 */
template <typename T>
T GetMean(const std::vector<T>& vec)
{
    return GetSum<T>(vec) * (1/(float)(vec.size()));
}

/**
 * @brief Gets the RMS value of a float vector.
 * @param vec A vector of float values.
 * @return The RMS value of a float vector \p T.
 */
float GetRMS(const std::vector<float>& vec);

/**
 * @brief Calculates the median of the value distribution of the given vector.
 * @param vec The vector to calculate median.
 * @return The median of the value distribution of \c vec.
 */
template <typename T>
float GetMedian(const std::vector<T>& vec)
{
    std::vector<T> v = vec;
    std::sort(v.begin(), v.end());
    int N = v.size();

    if (N % 2 == 0)
        return (v[N/2 - 1] + v[N/2]) / 2.;
    else
        return v[N/2];
}

/**
 * @brief Calculates the skewness of the value distribution of the given vector.
 * @param vec The vector to calculate skewness.
 * @return The skewness of the value distribution of \c vFloat.
 */
template <typename T>
float GetSkew(const std::vector<T>& vec)
{
    float m3 = 0;
    float mean = GetMean(vec);
    float N = vec.size();

    for (auto const& value: vec) {
        m3 += pow((value - mean), 3.);
    }
    m3 /= N;

    return m3 / pow(GetRMS(vec), 1.5);
}

/**
 * @brief Get i-th Legendre polynomial P_i(x) evaluated at x.
 * @param i The order of Legendre polynomial.
 * @param x The x value to evaluate the polynomial.
 * @return The i-th Legendre polynomial P_i(x) evaluated at x.
 */
float GetLegendreP(int i, float& x);

/**
 * @brief Calculates an opening angle given three unit vectors.
 * @param uA A unit vector.
 * @param uB A unit vector.
 * @param uC A unit vector.
 * @return The opening angle (deg) defined by `uA`, `uB`, and `uC`.
 */
float GetOpeningAngle(TVector3 uA, TVector3 uB, TVector3 uC);

float GetDWallInDirection(TVector3 vtx, TVector3 dir);

float GetDWall(TVector3 vtx);

/**
 * @brief Returns particle name given a PDG encoding.
 * @param pid The PDG encoding of a particle.
 * @return Particle name in `TString`.
 */
TString GetParticleName(int pid);

/**
 * @brief Returns interaction name given a Geant3 interaction code.
 * @param lmec The Geant3 interaction code of an interaction.
 * @return Interaction name in `TString`.
 */
TString GetInteractionName(int lmec);

template int GetSum<int>(const std::vector<int>& vec);
template float GetSum<float>(const std::vector<float>& vec);
template int GetMean<int>(const std::vector<int>& vec);
template float GetMean<float>(const std::vector<float>& vec);
template TVector3 GetMean<TVector3>(const std::vector<TVector3>& vec);

namespace Calc
{
    const std::function<float(const std::vector<float>&)> Sum = std::bind(GetSum<float>, std::placeholders::_1);
    const std::function<float(const std::vector<float>&)> Mean = std::bind(GetMean<float>, std::placeholders::_1);
    const std::function<float(const std::vector<float>&)> RMS = std::bind(GetRMS, std::placeholders::_1);
}

#endif