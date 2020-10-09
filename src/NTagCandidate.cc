#include "NTagCandidate.hh"
#include "NTagEventInfo.hh"

NTagCandidate::NTagCandidate() {}
NTagCandidate::~NTagCandidate() {}

void NTagCandidate::SetHitInfo(const std::vector<float>& rawT,
                               const std::vector<float>& resT,
                               const std::vector<float>& pmtQ,
                               const std::vector<int>& cabI,
                               const std::vector<int>& sigF)
{
    vHitRawTimes = rawT;
    vHitResTimes = resT;
    vHitChargePE = pmtQ;
    vHitCableIDs = cabI;
    vHitSigFlags = sigF;
}

void NTagCandidate::DumpHitInfo()
{   
    std::cout << "HitRawTimes \t HitResTimes \t HitChargePE \t HitCableIDs \t HitSigFlags" << std::endl;
    
    for (unsigned int iHit = 0; iHit < vHitRawTimes.size(); iHit++) {
        std::cout <<  vHitRawTimes[iHit];
        std::cout << "\t\t " << vHitResTimes[iHit];
        std::cout << "\t\t " << vHitChargePE[iHit];
        std::cout << "\t\t " << vHitCableIDs[iHit];
        
        if (!vHitSigFlags.empty()) std::cout << "\t " << vHitSigFlags[iHit];
        
        std::cout << std::endl;
    }
}