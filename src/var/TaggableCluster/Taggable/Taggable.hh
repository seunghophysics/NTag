#ifndef TAGGABLE_HH
#define TAGGABLE_HH

#include "TVector3.h"

#include "NTagGlobal.hh"
#include "Particle.hh"

class Taggable
{
    public:
        Taggable();
        Taggable(TaggableType tType, float time, float energy, TVector3 vertex);

        inline float Time() const { return fT; }
        inline void SetTime(float time) { fT = time; }
        inline float Energy() const { return fE; }
        inline void SetEnergy(float e) { fE = e; }
        inline TVector3 Vertex() const { return fVertex; }
        inline void SetVertex(TVector3 vertex) { fVertex = vertex; }
        inline TaggableType Type() const { return fType; }
        inline TaggableType TaggedType() const { return fTaggedType; }
        inline void SetTaggedType(TaggableType t) { fTaggedType = t; }
        inline TVector3 ParentVertex() const { return fParentVertex; }
        inline float ParentT() const { return fParentT; }
        inline float ParentE() const { return fParentE; }
        inline int ParentIndex() const { return fParentIndex; }
        inline unsigned int ParentIntID() const { return fParentIntID; }

        void SetParent(const Particle& parent, int id);
        void SetCandidateIndex(const std::string& key, int id);
        int GetCandidateIndex(const std::string& key) const;

        void Dump();

    protected:
        TVector3 fVertex, fParentVertex;
        float fT, fParentT;
        float fE, fParentE;
        int fEarlyIndex, fDelayedIndex, fParentIndex;
        unsigned int fParentIntID;
        TaggableType fType;
        TaggableType fTaggedType;
};

#endif