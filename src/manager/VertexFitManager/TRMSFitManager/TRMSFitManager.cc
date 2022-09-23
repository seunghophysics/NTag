#include "geotnkC.h"

#include "TRMSFitManager.hh"

TRMSFitManager::TRMSFitManager(Verbosity verbose)
: VertexFitManager("TRMSFitManager", verbose),
INITGRIDWIDTH(800), MINGRIDWIDTH(50), GRIDSHRINKRATE(0.5), VTXMAXRADIUS(5000) {}
TRMSFitManager::~TRMSFitManager() {}

void TRMSFitManager::Fit(const PMTHitCluster& hitCluster)
{
    // copy hit cluster
    auto cluster = hitCluster;

    // grid search parameters
    float gridWidth = INITGRIDWIDTH;
    float gridRLimit = (int)(2*RINTK/gridWidth)*gridWidth/2.;
    float gridZLimit = (int)(2*ZPINTK/gridWidth)*gridWidth/2.;
    TVector3 gridOrigin(0, 0, 0); // grid origin in the grid search loop (starts at tank center)
    TVector3 minGridPoint;        // temp point to save TRMS-minimizing grid point
    TVector3 gridPoint;           // point in grid to find TRMS

    float minTRMS = 9999.; float tRMS;

    // repeat until grid width gets small enough
    while (gridWidth > MINGRIDWIDTH-0.1) {

        // allocate coordinates to a grid point
        for (float dx=-gridRLimit; dx<gridRLimit+0.1; dx+=gridWidth) {
            for (float dy=-gridRLimit; dy<gridRLimit+0.1; dy+=gridWidth) {
                for (float dz=-gridZLimit; dz<gridZLimit+0.1; dz+=gridWidth) {
                    TVector3 displacement(dx, dy, dz);
                    gridPoint = gridOrigin + displacement;

                    // skip grid point out of tank
                    if (gridPoint.Perp() > RINTK || abs(gridPoint.z()) > ZPINTK) continue;

                    // skip grid point further away from the maximum search range
                    if (gridPoint.Mag() > VTXMAXRADIUS) continue;

                    // subtract ToF from the search vertex
                    cluster.SetVertex(gridPoint);
                    tRMS = cluster.Find(HitFunc::T, Calc::RMS);

                    // save TRMS minimizing grid point
                    if (tRMS < minTRMS) {
                        minTRMS = tRMS;
                        minGridPoint = gridPoint;
                    }
                }
            }
        }

        // change grid origin to the TRMS-minimizing grid point,
        // shorten the grid width,
        // and repeat until grid width gets small enough!
        gridOrigin = minGridPoint;
        gridWidth *= GRIDSHRINKRATE;
        gridRLimit *= GRIDSHRINKRATE;
        gridZLimit *= GRIDSHRINKRATE;
    }

    fFitVertex = minGridPoint;
    cluster.SetVertex(fFitVertex);
    fFitTime = cluster.Find(HitFunc::T, Calc::Mean);

    fFitGoodness = GetGoodness(cluster, fFitVertex, fFitTime);
}