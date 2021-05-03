#ifndef TOOL_HH
#define TOOL_HH

#include <string>
#include <sstream>

#include "Logger.hh"

class ToolChain;
class DataModel;

class Tool
{
    public:
        Tool() {}
        virtual ~Tool() {}

        void ConnectToToolChain(ToolChain* toolChain);
        
        virtual bool Initialize() = 0;
        virtual bool Execute() = 0;
        virtual bool Finalize() = 0;
        
        template <typename T>
        void Log(T msg, Verbosity msgType=pDEFAULT)
        {
            std::stringstream toolOneLineMsg;
            toolOneLineMsg << "[" << name << "] "  << msg << "\n"; 
            logger->Log(toolOneLineMsg.str(), msgType);
        }
        
        std::string GetName() { return name; }

    protected:
        std::string name;
        
        DataModel* eventData;
        Logger* logger;
};

#endif