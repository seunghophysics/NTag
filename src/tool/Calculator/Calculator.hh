/*******************************************
*
* @file Calculator.hh
*
* @brief Defines useful calculator functions.
*
********************************************/

#ifndef CALCULATOR_HH
#define CALCULATOR_HH

#include <algorithm>
#include <array>
#include <functional>
#include <map>
#include <vector>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <cmath>
#include <random>

#include <TRandom3.h>
#include <TString.h>
#include <TVector3.h>

/** Global random number generator */
extern TRandom3 ranGen;
extern std::default_random_engine c_ranGen;

extern "C"
{
    /**
     * @brief Calculates a vertex's distance to the SK tank wall.
     * @param vertex A 3-dim float array with SK x, y, z coordinates in cm.
     */
    float wallsk_(float* vertex);
}

/** Gets current working directory. */
static std::string GetCWD()
{
    char tempPath[255];
    return ( getcwd(tempPath, sizeof(tempPath)) ? std::string( tempPath ) + "/" : std::string("") );
}

/**
 * @brief Gets path to the given environment variable.
 * @param pathName Name of the environment variable.
 */
static std::string GetENV(const char* pathName)
{
    char tempPath[255];
    return ( getenv(pathName) ? std::string( getenv(pathName) ) + "/" : std::string("") );
}

float Sigmoid(const float x);

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

/**
 * @brief Calculates the sum of the values in the given vector.
 * @param vec The vector to calculate sum.
 * @return The sum of the values stored in \c vec.
 */
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
template <typename T>
float GetRMS(const std::vector<T>& vec)
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
 * @return The skewness of the value distribution of \c vec.
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

    if (m3==0) return 0;
    else return m3 / pow(GetRMS(vec), 1.5);
    //return skew;
}

/**
 * @brief Find an index of a value in the given vector.
 * @param vec A vector to find index.
 * @param value A value to find index.
 * @return The index of a value in the given vector if the value exists, otherwise -1.
 */
template<typename T>
int FindIndex(const std::vector<T>& vec, const T& value) {
    int index = std::find(vec.begin(), vec.end(), value) - vec.begin();
    if ((unsigned int)index == vec.size()) index = -1;
    return index;
}

/**
 * @brief Get i-th Legendre polynomial \f$P_i (x)\f$ evaluated at x.
 * @param i The order of Legendre polynomial.
 * @param x The x value to evaluate the polynomial.
 * @return The i-th Legendre polynomial \f$P_i (x)\f$ evaluated at x.
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

/**
 * @brief Calculates the distance to the SK tank wall in the given direction.
 * @param vtx The input vertex with SK x, y, z coordinates in cm.
 * @param dir The input direction vertex.
 * @return The distance to the wall in the given direction in cm.
 */
float GetDWallInDirection(TVector3 vtx, TVector3 dir);

/**
 * @brief Calculates the nearest distance to the SK tank wall.
 * @param vtx The input vertex with SK x, y, z coordinates in cm.
 * @return The nearest distance to the wall in cm.
 */
float GetDWall(TVector3 vtx);

/**
 * @brief Returns the index of the minimum value in a given vector.
 * @param vec The input vector.
 * @return The index of the minimum value in a given vector.
 */
unsigned int GetMinIndex(std::vector<float>& vec);

/**
 * @brief Returns the index of the minimum value in a given vector.
 * @param vec The input vector.
 * @return The index of the minimum value in a given vector.
 */
unsigned int GetMaxIndex(std::vector<float>& vec);

/** Sets seed of the random number generator */
void SetSeed(int seed);

/**
 * @brief Pick a random value in a given vector.
 * @param vec The input vector.
 * @return A randomly picked element of a given vector.
 */
template <typename T>
T PickRandom(const std::vector<T>& vec)
{
    int nFiles = vec.size();
    int pickedIndex = (int)(nFiles * ranGen.Rndm());
    return vec[pickedIndex];
}

/**
 * @brief Shuffle a given vector.
 * @param vec The input vector.
 */
template <typename T>
void Shuffle(std::vector<T>& vec)
{
    std::shuffle(std::begin(vec), std::end(vec), c_ranGen);
}

/**
 * @brief Pick a random subdirectory from a given path.
 * @param dirPath The given directory path in string.
 * @return A randomly picked subdirectory path in string.
 */
TString PickSubdirectory(TString dirPath);

/**
 * @brief Pick a random file from a given path.
 * @param dirPath The given directory path in string.
 * @param extension The file extension to limit the output. If empty, all file extensions are allowed.
 * @return A randomly picked file path in string.
 */
TString PickFile(TString dirPath, const char* extension="");

/**
 * @brief Get a list of files in a given directory path.
 * @param dirPath The given directory path in string.
 * @param extension The file extension to limit the output. If empty, all file extensions are allowed.
 * @param recursive If set to \c true, all subfiles in subdirectories are searched for recursively.
 * @return A vector of subfile paths in string.
 */
std::vector<TString> GetListOfFiles(TString dirPath, const char* extension="", bool recursive=false);

/**
 * @brief Get a list of subdirectories in a given directory path.
 * @param dirPath The given directory path in string.
 * @return A vector of subdirectory paths in string.
 */
std::vector<TString> GetListOfSubdirectories(TString dirPath);

/**
 * @brief Given a sorted vector, get a list of indices whose elements are within the input range.
 * @param sortedVec A sorted vector of double.
 * @param low The lower bound of the range.
 * @param high The upper bound of the range.
 * @return A vector of indices whose elements are within the input range.
 */
std::vector<unsigned int> GetRangeIndex(const std::vector<double>& sortedVec, double low, double high);

/**
 * @brief Histogram a given vector of float.
 * @param vec The input vector of float to histogram.
 * @param nBins The number of bins.
 * @param min The lower bound of the range.
 * @param max The upper bound of the range.
 * @return A vector of pairs, whose first float is the bin center and the second int is the bin count.
 */
std::vector<std::pair<float, int>> Histogram(std::vector<float> vec, int nBins, float min, float max);

/**
 * @brief Tokenize a string with a given delimeter.
 * @param target A target string to tokenize.
 * @param delim The delimiter for tokenization.
 * @return A vector of tokens (string) splitted by a given delimeter.
 */
std::vector<std::string> Split(std::string target, std::string delim);

template int      GetSum<int>      (const std::vector<int>&      vec);
template float    GetSum<float>    (const std::vector<float>&    vec);
template int      GetMean<int>     (const std::vector<int>&      vec);
template float    GetMean<float>   (const std::vector<float>&    vec);
template TVector3 GetMean<TVector3>(const std::vector<TVector3>& vec);

namespace Calc
{
    const std::function<float(const std::vector<float>&)> Sum  = std::bind(GetSum<float>, std::placeholders::_1);
    const std::function<float(const std::vector<float>&)> Mean = std::bind(GetMean<float>, std::placeholders::_1);
    const std::function<float(const std::vector<float>&)> RMS  = std::bind(GetRMS<float>, std::placeholders::_1);
}

#endif