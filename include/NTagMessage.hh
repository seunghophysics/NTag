#ifndef NTAGMESSAGE_HH
#define NTAGMESSAGE_HH 1

#include <ctime>

#include <TString.h>

enum Verbosity {pNONE, pERROR, pWARNING, pDEFAULT, pDEBUG};

class NTagMessage
{
    public:
        NTagMessage(const char* className="", Verbosity verbose=pDEFAULT);
        ~NTagMessage();

        virtual void  PrintTag(Verbosity);
        virtual void  Print(TString, Verbosity vType=pDEFAULT);
        virtual void  Print(const char*, Verbosity vType=pDEFAULT);
        virtual float Timer(TString, std::clock_t tStart, Verbosity vType=pDEFAULT);

    private:
        const char*  fClassName;
        Verbosity fVerbosity;
};

#endif