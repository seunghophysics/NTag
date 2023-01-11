#ifndef TRIGGERMANAGER_HH
#define TRIGGERMANAGER_HH 1

#include <vector>

#include <mcinfo.h>

#include "Printer.hh"
#include "PMTHitCluster.hh"

class TriggerManager
{
    public:
        TriggerManager();
        TriggerManager(int runNo);
        ~TriggerManager() {};

        void ApplyTrigger(PMTHitCluster& signalHits);
        void MakeTQRAW(int pmtID, float t, float q, float tOffset);
        int FindMainTrigger(int numTriggers, float tOffset);

        void FillCommon();
        void FillTrgOffset(MCInfo& inputMCINFO);

        std::vector<int> GetTriggerType() { return fSubTrigger_Type; };
        std::vector<int> GetTriggerTime() { return fSubTrigger_Time; };
        std::vector<float> GetTriggerTimeRel() { return fSubTrigger_TimeRel; };
        std::vector<int> GetTriggerIndex() { return fSubTrigger_Index; };

   private: 
      int fRawGate;
      int fRawGateOD;

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