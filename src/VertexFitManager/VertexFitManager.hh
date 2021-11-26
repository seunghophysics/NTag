#ifndef VERTEXFITMANAGER_HH
#define VERTEXFITMANAGER_HH

#include "TVector3.h"
#include "PMTHitCluster.hh"
#include "Printer.hh"

class VertexFitManager
{
    public:
        VertexFitManager(const char* fitterName, Verbosity verbose=pDEFAULT)
        : fFitVertex(), fFitTime(0), fMsg(fitterName, verbose) {}

        virtual void Fit(const PMTHitCluster& hitCluster) = 0;
        TVector3 GetFitVertex() { return fFitVertex; }
        float GetFitTime() { return fFitTime; }
        void SetVerbosity(Verbosity verbose) { fMsg.SetVerbosity(verbose); }

    protected:
        TVector3 fFitVertex;
        float    fFitTime;

        Printer fMsg;
};

#endif