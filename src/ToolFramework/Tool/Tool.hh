#ifndef TOOL_HH
#define TOOL_HH

#include <string>
#include <sstream>

#include "DataModel.hh"
#include "Logger.hh"

class ToolChain;
class DataModel;

enum ExceptionBehavior
{
    eENDRUN,
    eSKIPEVENT,
    ePROCESSANDRETURN
};

class Tool
{
    public:
        Tool():safeToExecute(false), exeCounter(0) {}
        virtual ~Tool() {}

        void ConnectToToolChain(ToolChain* toolChain);
        
        void SetDataModel(DataModel* dataModel) { sharedData = dataModel; }
        void SetLogger(Logger* inLogger) { logger = inLogger; }

        virtual bool Initialize() = 0;
        virtual bool Execute() = 0;
        virtual bool Finalize() = 0;

        virtual bool CheckSafety() { safeToExecute = true; return safeToExecute; }
        bool CheckSafetyAndExecute();
        
        inline void IncrementCounter() { exeCounter++; }
        inline unsigned int GetCounter() { return exeCounter; }

        template <typename T>
        void Log(T msg, Verbosity msgType=pDEFAULT)
        {
            std::stringstream toolOneLineMsg;
            toolOneLineMsg << "[" << name << "] "  << msg << "\n";
            logger->Log(toolOneLineMsg.str(), msgType);
        }

        std::string GetName() { return name; }
        inline bool IsSafeToExecute() { return safeToExecute; }

    protected:
        std::string name;
        bool safeToExecute;
        unsigned int exeCounter;

        DataModel* sharedData;
        Logger* logger;
        
        static bool inputIsMC;
};

#endif