#ifndef TOOL_HH
#define TOOL_HH

#include <string>

class EventData;

class Tool
{
    public:
        Tool() {}
        virtual ~Tool() {}

        void ConnectEventData(EventData& data) { eventData = &data; }
        virtual bool Initialize() = 0;
        virtual bool Execute() = 0;
        virtual bool Finalize() = 0;
        
        std::string GetName() { return name; }

    protected:
        std::string name;
        EventData* eventData;
};

#endif