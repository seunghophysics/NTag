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

    for (auto const& particle : particleCluster) {

        // skip particles outside tank
        //if (GetDWall(particle.Vertex()) < 0) continue;

        // gamma-ray produced by n-capture
        if (particle.IntID() == iNCAPTURE && particle.PID() == GAMMA) {

            bool isNewCapture = true;

            // check saved captures
            for (unsigned int iCapture = 0; iCapture < fElement.size(); iCapture++) {

                Taggable& capture = fElement[iCapture];

                // if a matching capture time exists,
                // just add the gamma-ray to the matching capture
                if (fabs((double)(particle.Time()-capture.Time()))<1.e-4) {
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
    msg.PrintBlock("MC Taggables", pEVENT);
    std::cout << "\033[4m No. Type     Time (us) Dist (cm) DWall (cm) Energy (MeV) Early Delayed TaggedAs\033[0m" << std::endl;

    for (unsigned int iTaggable = 0; iTaggable < fElement.size(); iTaggable++) {
        auto& taggable = fElement[iTaggable];
        auto vertex = taggable.Vertex();
        auto time = taggable.Time();
        auto earlyIndex = taggable.GetCandidateIndex("Early")+1;
        auto delayedIndex = taggable.GetCandidateIndex("Delayed")+1;
        auto taggedType = taggable.TaggedType();
        std::cout << std::right << std::setw(3) << iTaggable+1 << "  ";
        std::cout << std::right << std::setw(8) << (taggable.Type() == typeE ? "mu-e" : "nCapture") << " ";
        if (time<10)
        std::cout << " " << std::right << std::setw(8) << std::fixed << std::setprecision(2) << time << " ";
        else
        std::cout << std::right << std::setw(6) << std::fixed << (int)(time+0.5f) << "    ";
        std::cout << std::right << std::setw(8) << (int)((taggable.Vertex()-fPromptVertex).Mag()) << " ";
        std::cout << std::right << std::setw(10) << (int)(GetDWall(vertex)) << "  ";
        std::cout << std::right << std::setw(11) << std::setprecision(2) << taggable.Energy() << "  ";
        std::cout << std::right << std::setw(5) << (earlyIndex ? std::to_string(earlyIndex) : "-") << " ";
        std::cout << std::right << std::setw(7) << (delayedIndex ? std::to_string(delayedIndex) : "-") << " ";
        std::cout << std::right << std::setw(8) << (taggedType==typeMissed ? "-" : (
                                                    taggedType==typeE ?      "e" : (
                                                    taggedType==typeN?       "n" :
                                                    /* else */               "e/n"))) << "\n";
    }
}

void TaggableCluster::MakeBranches()
{
     if (fIsOutputTreeSet) {
        fOutputTree->Branch("Type", &fTypeVector);
        fOutputTree->Branch("TaggedType", &fTaggedTypeVector);
        fOutputTree->Branch("t", &fTimeVector);
        fOutputTree->Branch("E", &fEnergyVector);
        fOutputTree->Branch("tagvx", &fXVector);
        fOutputTree->Branch("tagvy", &fYVector);
        fOutputTree->Branch("tagvz", &fZVector);
        fOutputTree->Branch("DistFromPV" ,&fDistVector);
        fOutputTree->Branch("DWall" ,&fDWallVector);
        fOutputTree->Branch("EarlyIndex" ,&fEarlyIndexVector);
        fOutputTree->Branch("DelayedIndex" ,&fDelayedIndexVector);
    }
}

void TaggableCluster::FillTree()
{
    fTypeVector.clear();
    fTaggedTypeVector.clear();
    fTimeVector.clear();
    fEnergyVector.clear();
    fXVector.clear();
    fYVector.clear();
    fZVector.clear();
    fDistVector.clear();
    fDWallVector.clear();
    fEarlyIndexVector.clear();
    fDelayedIndexVector.clear();

    for (auto const& taggable: fElement) {
        auto const& vertex = taggable.Vertex();
        fTypeVector.push_back(taggable.Type());
        fTaggedTypeVector.push_back(taggable.TaggedType());
        fTimeVector.push_back(taggable.Time());
        fEnergyVector.push_back(taggable.Energy());
        fXVector.push_back(vertex.x());
        fYVector.push_back(vertex.y());
        fZVector.push_back(vertex.z());
        fDistVector.push_back((taggable.Vertex()-fPromptVertex).Mag());
        fDWallVector.push_back(GetDWall(vertex));
        fEarlyIndexVector.push_back(taggable.GetCandidateIndex("Early"));
        fDelayedIndexVector.push_back(taggable.GetCandidateIndex("Delayed"));
    }

    if (fIsOutputTreeSet) fOutputTree->Fill();
}