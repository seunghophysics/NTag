#include <iomanip>

#include "EventParticles.hh"

void EventParticles::DumpAllElements()
{
    std::cout << "\n\033[4m No. Time (us) Momentum (MeV/c) \033[0m" << std::endl;

    for (int iCapture = 0; iCapture < nElements; iCapture++) {
        auto& particle = element[iCapture];
        auto vertex = particle.Vertex();
        std::cout << std::right << std::setw(3) << iCapture+1 << "  ";
        std::cout << std::right << std::setw(8) << (int)(particle.Time()*1e-3+0.5f) << "  ";
        std::cout << std::right << std::setw(11) << std::setprecision(2) << particle.Momentum().Mag() << "\n";
    }
}