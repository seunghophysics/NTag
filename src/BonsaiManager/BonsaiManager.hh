#ifndef BONSAIMANAGER_HH
#define BONSAIMANAGER_HH

#include "TVector3.h"

#include "PMTHitCluster.hh"
#include "Printer.hh"

class pmt_geometry;
class likelihood;

float* GetPMTPositionArray();

class BonsaiManager
{
    public:
        BonsaiManager();
        ~BonsaiManager();
        
        void Initialize();
        void InitializeSKLOWE(int refRunNo=62428);
        
        void Fit(const PMTHitCluster& hitCluster);
        void FitSKLOWE(const PMTHitCluster& hitCluster);
        
        void DumpFitResult();
    
    private:
        pmt_geometry* fPMTGeometry;
        likelihood*   fLikelihood;
        
        TVector3 fFitVertex;
        float    fFitTime;
        float    fFitEnergy;
        float    fFitGoodness;
        float    fFitDirKS;
        float    fFitOvaQ;
        
        Printer fMsg;
};

#endif