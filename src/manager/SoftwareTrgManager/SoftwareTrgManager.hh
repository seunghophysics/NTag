#ifndef SOFTWARETRIGMANAGER_HH
#define SOFTWARETRIGMANAGER_HH

#include <vector>

#include "Printer.hh"
#include "PMTHitCluster.hh"

#include <mcinfo.h>

class SoftwareTrgManager
{
    public:
        SoftwareTrgManager();
        SoftwareTrgManager(int runNo);
        ~SoftwareTrgManager(){};

        void ApplyTrigger(PMTHitCluster* signalHits);
        void MakeTQRAW(int pmtID, float t, float q, float tOffset);
        int FindMainTrigger(int numTriggers, float tOffset);

        void FillCommon();
        void FillTrgOffset(MCInfo& inputMCINFO);

   private: 
      int fRawGate;
      // Constants for software trigger
      static const float PC2PE_MC; 
      static const float CNT2PC_S; 
      static const float CNT2PC_M; 
      static const float CNT2PC_L; 

      static const int QBEE_QTC_SMALL;
      static const int QBEE_QTC_MEDIUM;
      static const int QBEE_QTC_LARGE;

      static const int IQ_INGATE_FLAG;
      static const int SWTRG_SAME_GATE_WIDTH;

      int fIT0SK;
      int fIDTGSK;
      
      std::vector<int>   fSubTrigger_Type;
      std::vector<int>   fSubTrigger_Time;
      std::vector<float> fSubTrigger_TimeRel;
      std::vector<int>   fSubTrigger_Index;

};

#endif
