#ifndef NTAGTQREADER_HH
#define NTAGTQREADER_HH 1

#include "NTagZBS.hh"

class NTagTQReader : public NTagZBS
{
    public:
        NTagTQReader(const char* inFileName, const char* outFileName, Verbosity verbose);
        ~NTagTQReader();
        
        void ReadEvent();
        void WriteOutput();
};

#endif