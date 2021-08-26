#include <algorithm>
#include <iomanip>

#include "NCapture.hh"
#include "NCaptureCluster.hh"

NCaptureCluster::NCaptureCluster() {}
NCaptureCluster::NCaptureCluster(const ParticleCluster& particleCluster)
{
    for (auto const& particle : particleCluster) {
        
        // gamma-ray produced by n-capture
        if (particle.IntID() == iNCAPTURE && particle.PID() == GAMMA) {
            
            bool isNewCapture = true;

            // Check saved captures
            for (int iCapture = 0; iCapture < fElement.size(); iCapture++) {
                
                NCapture& capture = fElement[iCapture];
                
                // if a matching capture time exists, 
                // just add the gamma-ray to the matching capture
                if (fabs((double)(particle.Time()-capture.Time()))<1.e-7) {
                    isNewCapture = false;
                    capture.AddGamma(particle);
                }
            }
            
            // if this n-capture gamma-ray is new
            if (isNewCapture) {
                NCapture capture;
                capture.AddGamma(particle);
                Append(capture);
            }
        }
    }
}

void NCaptureCluster::Sort()
{
    std::sort(fElement.begin(), fElement.end(), 
    [](const NCapture& cap1, const NCapture& cap2){ return cap1.Time() < cap2.Time() ;});
}

void NCaptureCluster::DumpAllElements()
{
    std::cout << "\n\033[4m No. X (cm) Y (cm) Z (cm) Time (us) Energy (MeV) \033[0m" << std::endl;

    for (int iCapture = 0; iCapture < fElement.size(); iCapture++) {
        auto& nCapture = fElement[iCapture];
        auto vertex = nCapture.Vertex();
        std::cout << std::right << std::setw(3) << iCapture+1 << "  ";
        std::cout << std::right << std::setw(5) << (int)(vertex.x()+0.5f) << "  ";
        std::cout << std::right << std::setw(5) << (int)(vertex.y()+0.5f) << "  ";
        std::cout << std::right << std::setw(5) << (int)(vertex.z()+0.5f) << "  ";
        std::cout << std::right << std::setw(8) << (int)(nCapture.Time()*1e-3+0.5f) << "  ";
        std::cout << std::right << std::setw(11) << std::setprecision(2) << nCapture.Energy() << "\n";
    }
}