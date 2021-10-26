#ifndef TAGGABLE_HH
#define TAGGABLE_HH

#include "TVector3.h"

enum TaggableType
{
    typeMissed,
    typeE,
    typeN,
    typeEN
};

class Taggable
{
    public:
        Taggable();
        Taggable(TaggableType tType, float time, float energy, TVector3 vertex);

        inline float Time() const { return t; }
        inline void SetTime(float time) { t = time; }
        inline float Energy() const { return E; }
        inline void SetEnergy(float e) { E = e; }
        inline TVector3 Vertex() const { return v; }
        inline void SetVertex(TVector3 vertex) { v = vertex; }
        inline TaggableType Type() const { return type; }
        inline TaggableType TaggedType() const { return taggedType; }
        inline void SetTaggedType(TaggableType t) { taggedType = t; }

        void SetCandidateIndex(const std::string& key, int id);
        int GetCandidateIndex(const std::string& key) const;

        void Dump();

    protected:
        TVector3 v;
        float t;
        float E;
        int earlyIndex, delayedIndex;
        TaggableType type;
        TaggableType taggedType;
};

#endif