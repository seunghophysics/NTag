/*******************************************
*
* @file NTagZBSTQReader.hh
*
* @brief Defines NTagZBSTQReader.
*
********************************************/

#ifndef NTAGZBSTQREADER_HH
#define NTAGZBSTQREADER_HH 1

#include "NTagZBS.hh"

/********************************************************
 * @brief The class for reading TQ information from ZBS.
 *
 * This class is an inherited class of NTagZBS, excluding
 * most of the most of the options inside
 * NTagZBS::ReadEvent except for
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
class NTagZBSTQReader : public NTagZBS
{
    public:
        NTagZBSTQReader(const char* inFileName, const char* outFileName, Verbosity verbose);
        ~NTagZBSTQReader();

        void ReadEvent();
        void WriteOutput();
};

#endif