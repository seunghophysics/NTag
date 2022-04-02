#ifndef VERTEXFITMANAGER_HH
#define VERTEXFITMANAGER_HH

#include "TVector3.h"
#include "PMTHitCluster.hh"
#include "Printer.hh"

class VertexFitManager
{
    public:
        VertexFitManager(const char* fitterName, Verbosity verbose=pDEFAULT)
        : fFitVertex(), fFitTime(0), fFitGoodness(0), fMsg(fitterName, verbose) {}

        virtual void Fit(const PMTHitCluster& hitCluster) = 0;
        TVector3 GetFitVertex() { return fFitVertex; }
        float GetFitTime() { return fFitTime; }
        float GetFitGoodness() { return fFitGoodness; }
        void SetVerbosity(Verbosity verbose) { fMsg.SetVerbosity(verbose); }
        
        static float GetGoodness(const PMTHitCluster& hitCluster, const TVector3& vertex, const float& t0);

    protected:
        TVector3 fFitVertex;
        float    fFitTime;
        float    fFitGoodness;

        Printer fMsg;
};

#endif