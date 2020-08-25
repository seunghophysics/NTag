#ifndef NTAGMESSAGE_HH
#define NTAGMESSAGE_HH 1

#include <ctime>
#include <TString.h>

enum {pDEFAULT, pWARNING, pERROR, pDEBUG};

class NTagMessage
{
    public:
        NTagMessage(const char* className="", unsigned int verbose=pDEFAULT);
        ~NTagMessage();

        virtual void  PrintTag(unsigned int);
        virtual void  Print(TString, unsigned int vType=pDEFAULT);
        virtual void  Print(const char*, unsigned int vType=pDEFAULT);
        virtual float Timer(TString, std::clock_t tStart, unsigned int vType=pDEFAULT);
        
    private:
        const char*  fClassName;
        unsigned int fVerbosity;
};

#endif