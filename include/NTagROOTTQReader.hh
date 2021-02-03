/*******************************************
*
* @file NTagROOTTQReader.hh
*
* @brief Defines NTagROOTTQReader.
*
********************************************/

#ifndef NTagROOTTQReader_HH
#define NTagROOTTQReader_HH 1

#include "NTagROOT.hh"

/********************************************************
 * @brief The class for reading TQ information from ZBS.
 *
 * This class is an inherited class of NTagROOT, excluding
 * most of the most of the options inside
 * NTagROOT::ReadEvent except for
 * NTagEventInfo::AppendRawHitinfo so that only TQ
 * information can be saved to an output tree \c restq
 * without searching for capture candidates.
 *
 * This class will do its job if the option `-readTQ` is
 * passed on the command line;
 * for example, `NTag -in (some input ZBS) -readTQ` will
 * read TQ information from the input ZBS and give an
 * output with the tree `restq` with TQ filled in it.
 *******************************************************/
class NTagROOTTQReader : public NTagROOT
{
    public:
        NTagROOTTQReader(const char* inFileName, const char* outFileName, Verbosity verbose);
        ~NTagROOTTQReader();

        void ReadEvent();
        void WriteOutput();
};

#endif