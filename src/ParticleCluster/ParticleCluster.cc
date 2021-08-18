#include <iomanip>

#include "Calculator.hh"
#include "ParticleCluster.hh"

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