#include <iomanip>

#include "TrueCapture.hh"

TrueCapture::TrueCapture()
: captureTime(0), gammaEnergy(0) {}

void TrueCapture::Append(const Particle& gamma)
{
    // 22: gamma-ray
    if (gamma.PID() == 22) {

        if (!nElements) {
            captureTime = gamma.Time();
            vertex = gamma.Vertex();
        }

        vElements.push_back(gamma);
        gammaEnergy += gamma.Momentum().Mag();
        nElements++;
    }
    else
        std::cerr << "TrueCapture::Append: the input particle is not a gamma-ray... skipping this particle!" << std::endl;
}

void TrueCapture::Dump()
{
    std::cout << "Capture vertex: " << std::setprecision(2) << vertex.x() << " ," << vertex.y() << " ," << vertex.z()
              << " Time: " << captureTime << " ns" << " Energy: "  << gammaEnergy << " MeV" << std::endl;
}
