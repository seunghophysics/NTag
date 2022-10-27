#include <cstdint>

#include "skparmC.h"
#include "skheadC.h"
#include "skdayC.h"
#include "sktqC.h"
#include "skroot_loweC.h"
#include "lfparmC.h"

#include "pmt_geometry.h"
#include "likelihood.h"
#include "goodness.h"
#include "fourhitgrid.h"
#include "bonsaifit.h"

#include "SKIO.hh"
#include "SKLibs.hh"

#include "BonsaiManager.hh"

float* GetPMTPositionArray()
{
    static float pmtPosition[3*MAXPM];
    for (int i=0; i<MAXPM; i++) {
        pmtPosition[3*i] = geopmt_.xyzpm[i][0];
        pmtPosition[3*i+1] = geopmt_.xyzpm[i][1];
        pmtPosition[3*i+2] = geopmt_.xyzpm[i][2];
    }
    return pmtPosition;
}

bool BonsaiManager::fIsLOWFITInitialized = false;

BonsaiManager::BonsaiManager(Verbosity verbose):
VertexFitManager("BonsaiManager", verbose), fPMTGeometry(nullptr), fLikelihood(nullptr),
fFitEnergy(-1), fFitDirKS(-1), fFitOvaQ(-1),
fRefRunNo(62428), fUseLOWFIT(false)
{}

BonsaiManager::~BonsaiManager()
{
    if (fPMTGeometry) delete fPMTGeometry;
    if (fLikelihood) delete fLikelihood;
    if (fIsLOWFITInitialized) cfbsexit_();
}

void BonsaiManager::Initialize()
{
    if (!skheadg_.sk_geometry) {
        skheadg_.sk_geometry = 5; // default: SK5
        geoset_();
    }

    fPMTGeometry = new pmt_geometry(MAXPM, GetPMTPositionArray());
    fLikelihood = new likelihood(fPMTGeometry->cylinder_radius(), fPMTGeometry->cylinder_height());
    fLikelihood->set_hits(NULL);
}

void BonsaiManager::InitializeLOWFIT(int refRunNo)
{
    SetRefRunNo(refRunNo);

    if (!SKIO::IsZEBRAInitialized()) kzinit_();
    skrunday_(); 
    skwt_gain_corr_();
    darklf_(&refRunNo);

    int maxpm = MAXPM;
    cfbsinit_(&maxpm, GetPMTPositionArray());

    // 1000 subtraction is for treatment of run start offset
    int elapsedDays = skday_data_.relapse[refRunNo-1000]; 
    waterTransparency = 12431.3;
    if (refRunNo) lfwater_(&elapsedDays, &waterTransparency);

    // Use G4 lowfit parameters
    if (fUseSKG4Parameter)
      wtpar_.flag_g4_wtpar = 1; // use SKG4 parameter
    else 
      wtpar_.flag_g4_wtpar = 0; // use SKDETSIM parameter

    fIsLOWFITInitialized = true;
}

void BonsaiManager::UseLOWFIT(bool turnOn, int refRunNo)
{
    SetRefRunNo(refRunNo);

    fUseLOWFIT = turnOn;
    if (fUseLOWFIT && !fIsLOWFITInitialized)
        InitializeLOWFIT(refRunNo);
}

void BonsaiManager::UseSKG4Parameter( bool turnOn )
{
    fUseSKG4Parameter = turnOn;
}

void BonsaiManager::Fit(const PMTHitCluster& hitCluster)
{
    if (fUseLOWFIT) {
        FitLOWFIT(hitCluster);
    }
    else {
        auto t = hitCluster[HitFunc::T];
        auto q = hitCluster[HitFunc::Q];
        //auto i = hitCluster.GetProjection(HitFunc::I);

        std::vector<int> i;
        for (auto const& hit: hitCluster)
            i.push_back(hit.i());

        goodness hits(fLikelihood->sets(), fLikelihood->chargebins(),
                      fPMTGeometry, hitCluster.GetSize(),
                      i.data(), t.data(), q.data());

        if (hits.nselected() >= 4) {
            fourhitgrid grid(fPMTGeometry->cylinder_radius(), fPMTGeometry->cylinder_height(), &hits);
            bonsaifit fitter(fLikelihood);
            fLikelihood->set_hits(&hits);
            fLikelihood->maximize(&fitter, &grid);

            // successful fit
            if (fLikelihood->nfit()) {
                float vertex[3] = {fitter.xfit(), fitter.yfit(), fitter.zfit()};
                float likelihood0, likelihood1, likelihood2, goodness[1], result[6];
                fFitVertex = TVector3(vertex);
                likelihood2 = fLikelihood->goodness(likelihood0, vertex, goodness);

                fLikelihood->tgood(vertex, 0, likelihood1);
                likelihood0 = fitter.maxq();

                fitter.fitresult();
                fFitTime = fLikelihood->get_zero();
                fLikelihood->get_dir(result);
                result[5] = fLikelihood->get_ll0();

                fFitGoodness = likelihood1;

                // direction
                // dirks
                // energy
                fFitEnergy = 0;
                fFitDirKS = 0;
                fFitOvaQ = 0;
            }
        }

        fLikelihood->set_hits(NULL);
        fFitGoodness = GetGoodness(hitCluster, fFitVertex, fFitTime);
    }
}

void BonsaiManager::FitLOWFIT(const PMTHitCluster& hitCluster)
{
    // clear sktq
    for (int iPMT=0; iPMT<MAXPM; iPMT++) {
        skt_.tisk[iPMT] = 0;
        skq_.qisk[iPMT] = 0;
        skchnl_.ihcab[iPMT] = 0;
    }

    // hitCluster->sktq
    // multiple hits to the same PMT should be counted as one
    int iHit = 0;
    float maxQ = 0;
    for (auto const& hit: hitCluster) {
        int iPMT = hit.i()-1;
        if (!skt_.tisk[iPMT]) {
            skt_.tisk[iPMT] = hit.t();
            skchnl_.ihcab[iHit] = hit.i();
            iHit++;
        }
        skq_.qisk[iPMT] += hit.q();

        // mxqisk: ID of PMT with max Q
        if (skq_.qisk[iPMT] > maxQ) {
            maxQ = skq_.qisk[iPMT];
            skq_.mxqisk = hit.i();
        }
    }
    // nqisk: total number of hit PMTs
    skq_.nqisk = iHit+1;

    // lfallfit_sk4_data / mc
    int nhitcut_lowfit = 1100;
    int fitFlag=0; int flagSkip=0; int flagLog=1;

    int original_nrunsk = skhead_.nrunsk;

    if (skhead_.mdrnsk == 0) {
        // for mc, switch nrunsk to reference run number temporarily
        skhead_.nrunsk = fRefRunNo;

        if (skheadg_.sk_geometry >= 6)
            lfallfit_sk6_mc_(&waterTransparency, &nhitcut_lowfit, &flagSkip, &flagLog, &fitFlag);
        else if (skheadg_.sk_geometry == 5)
            lfallfit_sk5_mc_(&waterTransparency, &nhitcut_lowfit, &flagSkip, &flagLog, &fitFlag);
        else
            lfallfit_sk4_final_qe43_mc_(&waterTransparency, &nhitcut_lowfit, &flagSkip, &flagLog, &fitFlag);

        // ...and switch back
        skhead_.nrunsk = original_nrunsk;
    }
    else {
        // for non-normal run, switch nrunsk to reference run number temporarily
        if (fRefRunNo) skhead_.nrunsk = fRefRunNo;

        std::cout << skheadg_.sk_geometry << std::endl;
        if (skheadg_.sk_geometry >= 6)
            lfallfit_sk6_data_(&waterTransparency, &nhitcut_lowfit, &flagSkip, &flagLog, &fitFlag);
        else if (skheadg_.sk_geometry == 5)
            lfallfit_sk5_data_(&waterTransparency, &nhitcut_lowfit, &flagSkip, &flagLog, &fitFlag);
        else
            lfallfit_sk4_final_qe43_(&waterTransparency, &nhitcut_lowfit, &flagSkip, &flagLog, &fitFlag);

        // ...and switch back
        skhead_.nrunsk = original_nrunsk;
    }

    // retreive common block
    fFitVertex = TVector3(skroot_lowe_.bsvertex[0], skroot_lowe_.bsvertex[1], skroot_lowe_.bsvertex[2]);
    fFitTime = skroot_lowe_.bsvertex[3];

    fFitGoodness = skroot_lowe_.bsgood[1];
    if (skroot_lowe_.bsenergy<9998) {
        fFitEnergy = skroot_lowe_.bsenergy;
        fFitDirKS = skroot_lowe_.bsdirks;
        fFitOvaQ = fFitGoodness*fFitGoodness - fFitDirKS*fFitDirKS;
    }
    else {
        fFitEnergy = -1; fFitDirKS = -1; fFitOvaQ = -1;
    }
}

void BonsaiManager::DumpFitResult()
{
    fMsg.Print(Form("Fit vertex: %3.2f, %3.2f, %3.2f", fFitVertex.x(), fFitVertex.y(), fFitVertex.z()));
    fMsg.Print(Form("Fit time: %3.2f ns", fFitTime));
    fMsg.Print(Form("Fit energy: %3.2f MeV", fFitEnergy));
    fMsg.Print(Form("Fit goodness: %3.2f", fFitGoodness));
    fMsg.Print(Form("Fit dirKS: %3.2f", fFitDirKS));
    fMsg.Print(Form("Fit ovaQ: %3.2f", fFitOvaQ));
}
