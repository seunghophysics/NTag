#include <iomanip>

#include <neworkC.h>

#include "SKLibs.hh"
#include "Calculator.hh"
#include "ParticleCluster.hh"
#include "Printer.hh"

ParticleCluster::ParticleCluster(vcwork_common primaryCommon, secndprt_common secondaryCommon)
{
    ReadCommonBlock(primaryCommon, secondaryCommon);
}

void ParticleCluster::ReadCommonBlock(vcwork_common primaryCommon, secndprt_common secondaryCommon)
{
    Clear();

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
        Particle primary(g4pid, 0, TVector3(primaryCommon.pos), TVector3(primaryCommon.pin[iVec]));
        Append(primary);
    }

    // Secondaries
    for (int iSec = 0; iSec < secondaryCommon.nscndprt; iSec++) {

        int detsimCode = secondaryCommon.iprtscnd[iSec];
        int pdgCode = detsimCode;
        if (detsimCode > 100000)
            pdgCode = gG3toG4PIDMap[detsimCode];
        if (detsimCode > 1e9) // nucleus code straight from skg4
            pdgCode = detsimCode;

        Particle secondary(pdgCode,
                           secondaryCommon.tscnd[iSec]*1e-3,
                           TVector3(secondaryCommon.vtxscnd[iSec]),
                           TVector3(secondaryCommon.pscnd[iSec]),
                           secondaryCommon.iprntprt[iSec],
                           secondaryCommon.lmecscnd[iSec],
                           TVector3(secondaryCommon.vtxprnt[iSec]),
                           TVector3(secondaryCommon.pprntinit[iSec]));

        Append(secondary);
    }

    Sort();
    FindParents();
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

void ParticleCluster::DumpAllElements() const
{
    Printer msg;

    if (nework_.modene) {
        msg.PrintBlock("NEUT MC", pEVENT);
        std::cout << "\033[4m Neutrino Type      Interaction  Momentum (GeV/c)\033[0m\n ";
        std::cout << std::right << std::setw(13) << GetParticleName(nework_.ipne[0]) << " ";
        std::cout << std::right << std::setw(16) << GetNEUTModeName(nework_.modene) << " ";
        std::cout << std::right << std::setw(14) << std::fixed << std::setprecision(2) << TVector3(nework_.pne[0]).Mag() << "\n\n";
    }

    msg.PrintBlock("MC Particles", pEVENT);
    std::cout << "\033[4m No.   Particle Time (us) Interaction     Parent KE (MeV) \033[0m" << std::endl;

    for (unsigned int iParticle = 0; iParticle < fElement.size(); iParticle++) {
        auto& particle = fElement.at(iParticle);
        auto vertex = particle.Vertex();
        auto parentName = GetParticleName(particle.ParentPID());
        //auto mom = particle.Momentum().Mag();
        auto ke = particle.Energy();
        std::cout << std::right << std::setw(3) << iParticle+1 << "  ";
        std::cout << std::right << std::setw(10) << particle.GetName() << " ";
        if (particle.Time()< 10)
        std::cout << " " << std::right << std::setw(8) << std::fixed << std::setprecision(2) << particle.Time() << " ";
        else
        std::cout << std::right << std::setw(8) << (int)(particle.Time()+0.5f) << "  ";
        std::cout << std::right << std::setw(11) << particle.GetIntName() << " ";
        std::cout << std::right << std::setw(10) << (parentName=="0" ? TString("-") : parentName) << " ";
        if (ke<10)
            std::cout << std::right << std::setw(8) << std::setprecision(1) << ke << "\n";
        else
            std::cout << std::right << std::setw(6) << std::fixed << (int)(ke+0.5f) << "\n";
    }
}

void ParticleCluster::MakeBranches()
{
    if (fIsOutputTreeSet) {
        fOutputTree->Branch("PID", &fPIDVector);
        fOutputTree->Branch("ParentPID", &fParentPIDVector);
        fOutputTree->Branch("ParentIndex", &fParentIndexVector);
        fOutputTree->Branch("ParentPX", &fParentPXVector);
        fOutputTree->Branch("ParentPY", &fParentPYVector);
        fOutputTree->Branch("ParentPZ", &fParentPZVector);
        fOutputTree->Branch("ParentVX", &fParentVXVector);
        fOutputTree->Branch("ParentVY", &fParentVYVector);
        fOutputTree->Branch("ParentVZ", &fParentVZVector);
        fOutputTree->Branch("IntID", &fInteractionIDVector);
        fOutputTree->Branch("t", &fTimeVector);
        fOutputTree->Branch("x", &fXVector);
        fOutputTree->Branch("y", &fYVector);
        fOutputTree->Branch("z", &fZVector);
        fOutputTree->Branch("px", &fPXVector);
        fOutputTree->Branch("py", &fPYVector);
        fOutputTree->Branch("pz", &fPZVector);
        fOutputTree->Branch("KE", &fKEVector);
    }
}

void ParticleCluster::FillTree()
{
    fParentPXVector.clear();
    fParentPYVector.clear();
    fParentPZVector.clear();
    fParentVXVector.clear();
    fParentVYVector.clear();
    fParentVZVector.clear();

    fPIDVector.clear();
    fParentPIDVector.clear();
    fParentIndexVector.clear();
    fInteractionIDVector.clear();
    fTimeVector.clear();
    fXVector.clear();
    fYVector.clear();
    fZVector.clear();
    fPXVector.clear();
    fPYVector.clear();
    fPZVector.clear();
    fKEVector.clear();

    for (auto const& particle: fElement) {
        auto const& vertex = particle.Vertex();
        auto const& momentum = particle.Momentum();
        fPIDVector.push_back(particle.PID());
        fParentPIDVector.push_back(particle.ParentPID());
        fParentPXVector.push_back(particle.ParentMomentum().x());
        fParentPYVector.push_back(particle.ParentMomentum().y());
        fParentPZVector.push_back(particle.ParentMomentum().z());
        fParentVXVector.push_back(particle.ParentVertex().x());
        fParentVYVector.push_back(particle.ParentVertex().y());
        fParentVZVector.push_back(particle.ParentVertex().z());
        fParentIndexVector.push_back(particle.ParentIndex());
        fInteractionIDVector.push_back(particle.IntID());
        fTimeVector.push_back(particle.Time());
        fXVector.push_back(vertex.x());
        fYVector.push_back(vertex.y());
        fZVector.push_back(vertex.z());
        fPXVector.push_back(momentum.x());
        fPYVector.push_back(momentum.y());
        fPZVector.push_back(momentum.z());
        fKEVector.push_back(particle.Energy());
    }

    if (fIsOutputTreeSet) fOutputTree->Fill();
}

void ParticleCluster::FindParents()
{
    for (int i=0; i<GetSize(); i++) {
        for (int j=0; j<GetSize(); j++) {
            if (i==j) continue;
            auto& parent = fElement.at(i);
            auto& particle = fElement.at(j);
            if (particle.IsDaughterOf(parent)) {
                particle.SetParentIndex(i);
            }
        }
    }
}