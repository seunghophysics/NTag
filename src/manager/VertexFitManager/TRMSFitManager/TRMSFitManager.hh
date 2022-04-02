#ifndef TRMSFITMANAGER_HH
#define TRMSFITMANAGER_HH

#include "VertexFitManager.hh"

class TRMSFitManager : public VertexFitManager
{
    public:
        TRMSFitManager(Verbosity verbose=pDEFAULT);
        ~TRMSFitManager();

        void SetParameters(float initgridwidth, float mingridwidth, float gridshrinkrate, float vtxsrcrange)
        {
            INITGRIDWIDTH = initgridwidth;
            MINGRIDWIDTH = mingridwidth;
            GRIDSHRINKRATE = gridshrinkrate;
            VTXSRCRANGE = vtxsrcrange;
        }

        void Fit(const PMTHitCluster& hitCluster);

    private:
        float INITGRIDWIDTH, MINGRIDWIDTH, GRIDSHRINKRATE, VTXSRCRANGE;
};

#endif