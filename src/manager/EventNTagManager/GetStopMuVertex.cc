#include "GetStopMuVertex.hh"

#include <iostream>
#include <array>

#include "skparmC.h"
#include "sktqC.h"
#include "apmringC.h"
#include "apmueC.h"
#include "apmsfitC.h"
#include "appatspC.h"
#include "geotnkC.h"
#include "skwaterlenC.h"

#include "SKLibs.hh"
#include "SKIO.hh"
#include "Printer.hh"

TVector3 GetStopMuVertex()
{
    Printer msg("GetStopMuVertex");
    msg.PrintBlock("Estimating muon stopping point...", pEVENT, pDEFAULT, false);

    float initPoint[3], momDir[3];
    float muonMom;
    float goodness, entryQ; // probably dummy
    int iRing = 0; // first ring
    enum {iGamma, iElectron, iMuon};

    SKIO::DisableConsoleOut();
    stmfit_(initPoint, momDir, goodness, entryQ);
    SKIO::EnableConsoleOut();

    if (goodness < 0)
        msg.Print("STMFIT error occurred.", pWARNING);

    // 1-ring muon
    apcommul_.apnring = 1; apcommul_.apip[iRing] = 13;

    for (int dim = 0; dim < 3; dim++) {
        apcommul_.appos[dim] = initPoint[dim];
        apcommul_.apdir[iRing][dim] = momDir[dim];
    }

    // Set cherenkov angle roughly to 42 deg
    apcommul_.apangcer[iRing] = 42.;

    SKIO::DisableConsoleOut();
    int iCall = 0, iTrCor = 0, iPAng = 0; // probably dummy
    int nRing = 1;
    sparisep_(iCall, iTrCor, iPAng, nRing); // momentum

    // MS-fit
    pffitres_.pffitflag = 1; // 0: normal fit, 1: fast fit
    int iPID = iMuon + 1; // muon (+1 for fortran)
    pfdodirfit_(iPID); // direction fit

    // Replace APFit direction with MS-fit direction
    for (int dim = 0; dim < 3; dim++)
        apcommul_.apdir[iRing][dim] = pffitres_.pfdir[2][iRing][dim];

    sparisep_(iCall, iTrCor, iPAng, nRing); // momentum
    sppang_(apcommul_.apip[iRing], apcommul_.apamom[iRing], apcommul_.apangcer[iRing]); // estimated cherenkov angle

    // Calculate momentum
    appatsp_.approb[iRing][iElectron] = -100.; // e-like probability
    appatsp_.approb[iRing][iMuon]     = 0.;    // mu-like probability (set this higher than e-like!)
    spfinalsep_();
    SKIO::EnableConsoleOut();

    for (int dim = 0; dim < 3; dim++)
        momDir[dim] = apcommul_.apdir[iRing][dim];

    // final mu-like momentum
    muonMom = appatsp2_.apmsamom[iRing][iMuon];

    // Muon range as a function of momentum (almost linear, PDG2020)
    const int binSize = 15;
    std::array<float, binSize> momenta = {0., 339.6, 1301., 2103., 3604., 4604., 5605., 7105., 8105., 9105.,
                        10110., 12110., 14110., 17110., 20110.};
    std::array<float, binSize> ranges = {0., 103.9, 567.8, 935.3, 1595., 2023., 2443., 3064., 3472., 3877.,
                        4279., 5075., 5862., 7030., 8183.};

    int iBin = 0; float range = 0;

    for (iBin = 0; momenta[iBin] < muonMom && iBin < binSize; iBin++);

    if (iBin < binSize)
        // linear interpolation for muon momentum within data
        range = ranges[iBin-1]
                + (ranges[iBin]-ranges[iBin-1]) * (muonMom-momenta[iBin-1])/(momenta[iBin]-momenta[iBin-1]);
    else
        // for muon momentum larger than 20 GeV/c,
        // assume constant stopping power 2.3 MeV/cm
        range = muonMom / 2.3;

    TVector3 stopPoint;
    stopPoint = TVector3(initPoint) + range * TVector3(momDir);

    // for estimated stop point outside tank, force it to be within tank
    float r = stopPoint.Perp();
    if (r > RINTK)
        stopPoint *= RINTK/r;
    float z = abs(stopPoint.z());
    if (z > ZPINTK)
        stopPoint *= ZPINTK/z;

    std::cout << std::endl;
    msg.Print(Form("Initial position : (%3.0f, %3.0f, %3.0f) cm", initPoint[0], initPoint[1], initPoint[2]), pDEFAULT);
    msg.Print(Form("Momentum direction : (%3.3f, %3.3f, %3.3f)", momDir[0], momDir[1], momDir[2]), pDEFAULT);
    msg.Print(Form("Momentum : %3.0f MeV/c", muonMom), pDEFAULT);
    msg.Print(Form("Stopping point : (%3.0f, %3.0f, %3.0f) cm", stopPoint.x(), stopPoint.y(), stopPoint.z()), pDEFAULT);

    return stopPoint;
}