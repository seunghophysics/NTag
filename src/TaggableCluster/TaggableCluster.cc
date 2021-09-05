#include <algorithm>
#include <iomanip>

#include "Calculator.hh"
#include "Printer.hh"
#include "TaggableCluster.hh"

TaggableCluster::TaggableCluster() {}
TaggableCluster::TaggableCluster(const ParticleCluster& particleCluster)
{
    ReadParticleCluster(particleCluster);
}

void TaggableCluster::ReadParticleCluster(const ParticleCluster& particleCluster)
{
    Clear();

    // taggable n
    for (auto const& particle : particleCluster) {
        
        // gamma-ray produced by n-capture
        if (particle.IntID() == iNCAPTURE && particle.PID() == GAMMA) {
            
            bool isNewCapture = true;

            // Check saved captures
            for (int iCapture = 0; iCapture < fElement.size(); iCapture++) {
                
                Taggable& capture = fElement[iCapture];
                
                // if a matching capture time exists, 
                // just add the gamma-ray to the matching capture
                if (fabs((double)(particle.Time()-capture.Time()))<1.e-7) {
                    isNewCapture = false;
                    capture.SetEnergy(capture.Energy()+particle.Energy());
                }
            }
            
            // if this n-capture gamma-ray is new
            if (isNewCapture) {
                Taggable capture(typeN, particle.Time(), particle.Energy(), particle.Vertex());
                Append(capture);
            }
        }
        
        // decay electrons
        else if (particle.IntID() == iDECAY && abs(particle.PID()) == ELECTRON)
            Append(Taggable(typeE, particle.Time(), particle.Energy(), particle.Vertex()));
    }
}

void TaggableCluster::Sort()
{
    std::sort(fElement.begin(), fElement.end(), 
    [](const Taggable& taggable1, const Taggable& taggable2){ return taggable1.Time() < taggable2.Time() ;});
}

void TaggableCluster::DumpAllElements() const
{
    Printer msg;
    msg.PrintTitle("MC Taggables");
    std::cout << "\033[4m No. Type Time (us) Dist (cm) DWall (cm) Energy (MeV) Early Delayed\033[0m" << std::endl;

    for (int iTaggable = 0; iTaggable < fElement.size(); iTaggable++) {
        auto& taggable = fElement[iTaggable];
        auto vertex = taggable.Vertex();
        auto time = taggable.Time()*1e-3;
        auto earlyIndex = taggable.GetCandidateIndex("Early")+1;
        auto delayedIndex = taggable.GetCandidateIndex("Delayed")+1;
        std::cout << std::right << std::setw(3) << iTaggable+1 << "  ";
        std::cout << std::right << std::setw(4) << (taggable.Type() == typeE ? "e" : "n") << " ";
        //std::cout << std::right << std::setw(5) << (int)(vertex.x()+0.5f) << "  ";
        //std::cout << std::right << std::setw(5) << (int)(vertex.y()+0.5f) << "  ";
        //std::cout << std::right << std::setw(5) << (int)(vertex.z()+0.5f) << "  ";
        if (time<10)
        std::cout << " " << std::right << std::setw(8) << std::fixed << std::setprecision(2) << time << " ";
        else
        std::cout << std::right << std::setw(6) << std::fixed << (int)(time+0.5f) << "    ";
        std::cout << std::right << std::setw(8) << (int)((taggable.Vertex()-fPromptVertex).Mag()) << " ";
        std::cout << std::right << std::setw(9) << (int)(GetDWall(vertex)) << " ";
        std::cout << std::right << std::setw(11) << std::setprecision(2) << taggable.Energy() << " ";
        std::cout << std::right << std::setw(5) << (earlyIndex ? std::to_string(earlyIndex) : "-") << " ";
        std::cout << std::right << std::setw(7) << (delayedIndex ? std::to_string(delayedIndex) : "-") << "\n";
    }
}

void TaggableCluster::MakeBranches()
{
     if (fIsOutputTreeSet) {
        fOutputTree->Branch("type", &fTypeVector);
        fOutputTree->Branch("t", &fTimeVector);
        fOutputTree->Branch("E", &fEnergyVector);
        fOutputTree->Branch("vx", &fXVector);
        fOutputTree->Branch("vy", &fYVector);
        fOutputTree->Branch("vz", &fZVector);
    }
}

void TaggableCluster::FillTree()
{
    fTypeVector.clear();
    fTimeVector.clear();
    fEnergyVector.clear();
    fXVector.clear();
    fYVector.clear();
    fZVector.clear();
    
    for (auto const& taggable: fElement) {
        auto const& vertex = taggable.Vertex();
        fTypeVector.push_back(taggable.Type());
        fTimeVector.push_back(taggable.Time());
        fEnergyVector.push_back(taggable.Energy());
        fXVector.push_back(vertex.x());
        fYVector.push_back(vertex.y());
        fZVector.push_back(vertex.z());
    }

    if (fIsOutputTreeSet) fOutputTree->Fill();
}