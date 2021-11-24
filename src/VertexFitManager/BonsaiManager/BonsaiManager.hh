#ifndef BONSAIMANAGER_HH
#define BONSAIMANAGER_HH

#include "VertexFitManager.hh"

class pmt_geometry;
class likelihood;

float* GetPMTPositionArray();

class BonsaiManager : public VertexFitManager
{
    public:
        BonsaiManager(Verbosity verbose=pDEFAULT);
        ~BonsaiManager();

        void Initialize();
        void InitializeSKLOWE(int refRunNo=62428);

        void Fit(const PMTHitCluster& hitCluster);
        void FitSKLOWE(const PMTHitCluster& hitCluster);

        void DumpFitResult();

    private:
        pmt_geometry* fPMTGeometry;
        likelihood*   fLikelihood;

        float    fFitTime;
        float    fFitEnergy;
        float    fFitGoodness;
        float    fFitDirKS;
        float    fFitOvaQ;

        bool fIsInitialized;
};

#endif