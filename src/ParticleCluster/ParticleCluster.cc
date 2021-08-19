#include <iomanip>

#include <neworkC.h>

#include "SKLibs.hh"
#include "Calculator.hh"
#include "ParticleCluster.hh"

ParticleCluster::ParticleCluster(vcwork_common primaryCommon, secndprt_common secondaryCommon)
{
    // Primaries
    for (int iVec = 0; iVec < primaryCommon.nvect; iVec++) {
        int g3pid = skvect_.ip[iVec];
        int g4pid = 0;
        if (g3pid == 4) {
            // get pid from neut vectors
            float posnu[3]; nerdnebk_(posnu);
            g4pid = nework_.ipne[0];
        }
        else g4pid = gG3toG4PIDMap[g3pid];
        Particle primary(g4pid, 0, TVector3(skvect_.pos), TVector3(skvect_.pin[iVec]));
        Append(primary);
    }
    
    // Secondaries
    int nAllSec = secondaryCommon.nscndprt;
    for (int iSec = 0; iSec < nAllSec; iSec++) {

        Particle secondary(secondaryCommon.iprtscnd[iSec],
                           secondaryCommon.tscnd[iSec],
                           TVector3(secondaryCommon.vtxscnd[iSec]),
                           TVector3(secondaryCommon.pscnd[iSec]),
                           secondaryCommon.iprntprt[iSec],
                           secondaryCommon.lmecscnd[iSec]);

        Append(secondary);
    }
}

void ParticleCluster::SetT0(float t0)
{
    for (auto& particle: fElement)
        particle.AddT0(t0);
}

void ParticleCluster::Sort()
{
    std::sort(fElement.begin(), fElement.end(), 
    [](const Particle& cap1, const Particle& cap2){ return cap1.Time() < cap2.Time() ;});
}

void ParticleCluster::DumpAllElements()
{
    std::cout << "\n\033[4m No. Particle Time (us) Momentum (MeV/c) \033[0m" << std::endl;

    for (int iCapture = 0; iCapture < fElement.size(); iCapture++) {
        auto& particle = fElement[iCapture];
        auto vertex = particle.Vertex();
        std::cout << std::right << std::setw(3) << iCapture+1 << "  ";
        std::cout << std::right << std::setw(8) << particle.GetName() << " ";
        std::cout << std::right << std::setw(8) << (int)(particle.Time()*1e-3+0.5f) << "  ";
        std::cout << std::right << std::setw(11) << std::setprecision(2) << particle.Momentum().Mag() << "\n";
    }
}