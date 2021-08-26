#include <iomanip>

#include <neworkC.h>

#include "SKLibs.hh"
#include "Calculator.hh"
#include "ParticleCluster.hh"

ParticleCluster::ParticleCluster(vcwork_common primaryCommon, secndprt_common secondaryCommon)
{
    // Primaries
    for (int iVec = 0; iVec < primaryCommon.nvect; iVec++) {
        int g3pid = primaryCommon.ip[iVec];
        int g4pid = 0;
        if (g3pid == 4) {
            // get pid from neut vectors
            float posnu[3]; nerdnebk_(posnu);
            g4pid = nework_.ipne[iVec];
        }
        else g4pid = gG3toG4PIDMap[g3pid];
        Particle primary(g4pid, 0, TVector3(skvect_.pos), TVector3(skvect_.pin[iVec]));
        Append(primary);
    }
    
    // Secondaries
    for (int iSec = 0; iSec < secondaryCommon.nscndprt; iSec++) {

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
    [](const Particle& particle1, const Particle& particle2){ 
        //if (particle1.GetName().Contains("nu")) {
        //    return true;
        //}
        //else
            return particle1.Time() < particle2.Time(); });
}

void ParticleCluster::DumpAllElements()
{
    std::cout << "\n\033[4m No.   Particle Time (us) Interaction     Parent Momentum (MeV/c) \033[0m" << std::endl;

    for (int iParticle = 0; iParticle < fElement.size(); iParticle++) {
        auto& particle = fElement[iParticle];
        auto vertex = particle.Vertex();
        auto parentName = GetParticleName(particle.ParentPID());
        auto mom = particle.Momentum().Mag();
        std::cout << std::right << std::setw(3) << iParticle+1 << "  ";
        std::cout << std::right << std::setw(10) << particle.GetName() << " ";
        std::cout << std::right << std::setw(8) << (int)(particle.Time()*1e-3+0.5f) << "  ";
        std::cout << std::right << std::setw(11) << particle.GetIntName() << " ";
        std::cout << std::right << std::setw(10) << (parentName=="0" ? TString("-") : parentName) << " ";
        if (mom<10)
            std::cout << std::right << std::setw(16) << std::setprecision(1) << mom << "\n";
        else
            std::cout << std::right << std::setw(14) << std::fixed << (int)(mom+0.5f) << "\n";
    }
}