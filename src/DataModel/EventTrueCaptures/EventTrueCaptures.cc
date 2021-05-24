#include <algorithm>
#include <iomanip>

#include "EventTrueCaptures.hh"

void EventTrueCaptures::Sort()
{
    std::sort(vElements.begin(), vElements.end(), 
    [](const TrueCapture& cap1, const TrueCapture& cap2){ return cap1.Time() < cap2.Time() ;});
}

void EventTrueCaptures::DumpAllElements()
{
    std::cout << "\n\033[4m No. X (cm) Y (cm) Z (cm) Time (us) Energy (MeV) \033[0m" << std::endl;

    for (int iCapture = 0; iCapture < nElements; iCapture++) {
        auto& trueCapture = vElements[iCapture];
        auto vertex = trueCapture.Vertex();
        std::cout << std::right << std::setw(3) << iCapture+1 << "  ";
        std::cout << std::right << std::setw(5) << (int)(vertex.x()+0.5f) << "  ";
        std::cout << std::right << std::setw(5) << (int)(vertex.y()+0.5f) << "  ";
        std::cout << std::right << std::setw(5) << (int)(vertex.z()+0.5f) << "  ";
        std::cout << std::right << std::setw(8) << (int)(trueCapture.Time()*1e-3+0.5f) << "  ";
        std::cout << std::right << std::setw(11) << std::setprecision(2) << trueCapture.Energy() << "\n";
    }
}