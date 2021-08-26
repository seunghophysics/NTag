#include <algorithm>
#include <iomanip>

#include "DecayECluster.hh"

DecayECluster::DecayECluster() {}
DecayECluster::DecayECluster(const ParticleCluster& particleCluster)
{
    for (auto const& particle : particleCluster) {
        if (particle.IntID() == iDECAY && abs(particle.PID()) == ELECTRON)
            Append(Taggable(particle.Time(), particle.Energy(), particle.Vertex()));
    }
}

void DecayECluster::Sort()
{
    std::sort(fElement.begin(), fElement.end(), 
    [](const Taggable& taggable1, const Taggable& taggable2){ return taggable1.Time() < taggable2.Time() ;});
}

void DecayECluster::DumpAllElements()
{
    std::cout << "\n\033[4m No. X (cm) Y (cm) Z (cm) Time (us) Energy (MeV) \033[0m" << std::endl;

    for (int iTaggable = 0; iTaggable < fElement.size(); iTaggable++) {
        auto& taggable = fElement[iTaggable];
        auto vertex = taggable.Vertex();
        std::cout << std::right << std::setw(3) << iTaggable+1 << "  ";
        std::cout << std::right << std::setw(5) << (int)(vertex.x()+0.5f) << "  ";
        std::cout << std::right << std::setw(5) << (int)(vertex.y()+0.5f) << "  ";
        std::cout << std::right << std::setw(5) << (int)(vertex.z()+0.5f) << "  ";
        std::cout << std::right << std::setw(8) << (int)(taggable.Time()*1e-3+0.5f) << "  ";
        std::cout << std::right << std::setw(11) << std::setprecision(2) << taggable.Energy() << "\n";
    }
}