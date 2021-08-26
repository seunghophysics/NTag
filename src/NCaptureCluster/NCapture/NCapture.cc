#include <iomanip>
#include <iostream>

#include "NCapture.hh"

NCapture::NCapture()
: t(0), E(0), nGamma(0), v() {}

void NCapture::AddGamma(const Particle& gamma)
{
    // 22: gamma-ray
    if (gamma.PID() == 22) {

        if (!nGamma) {
            t = gamma.Time();
            v = gamma.Vertex();
        }

        E += gamma.Momentum().Mag();
        nGamma++;
    }
    else
        std::cerr << "NCapture::AddGamma: the input particle is not a gamma-ray... skipping this particle!" << std::endl;
}

void NCapture::Dump()
{
    std::cout << "Capture vertex: " << std::setprecision(2) << v.x() << " ," << v.y() << " ," << v.z()
              << " Time: " << t << " ns" << " Energy: "  << E << " MeV" << std::endl;
}