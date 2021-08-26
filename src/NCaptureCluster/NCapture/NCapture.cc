#include <iostream>

#include "NCapture.hh"

NCapture::NCapture()
: Taggable(), nGamma(0) {}

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