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
        void InitializeLOWFIT(int refRunNo=62428);

        void UseLOWFIT(bool turnOn=true, int refRunNo=62428);
        void Fit(const PMTHitCluster& hitCluster);
        void FitLOWFIT(const PMTHitCluster& hitCluster);

        inline float GetFitEnergy() { return fFitEnergy; }
        inline float GetFitDirKS() { return fFitDirKS; }
        inline float GetFitOvaQ() { return fFitOvaQ; }

        void DumpFitResult();

    private:
        pmt_geometry* fPMTGeometry;
        likelihood*   fLikelihood;

        float    fFitEnergy;
        float    fFitDirKS;
        float    fFitOvaQ;

        bool fIsInitialized;
        bool fUseLOWFIT;
};

#endif