/*******************************************
*
* @file NTagZBS.hh
*
* @brief Defines NTagZBS.
*
********************************************/

#ifndef NTAGZBS_HH
#define NTAGZBS_HH 1

#include "NTagIO.hh"

/********************************************************
 * @brief The class for ZBS I/O.
 *
 * This class is an inherited class of NTagIO.
 * ZBS-specific I/O functions are implemented.
 *******************************************************/
class NTagZBS : public NTagIO
{
    public:
        /**
         * @brief Constructor of NTagZBS.
         * @details Calls NTagIO::Initialize and NTagEventInfo::SetVertexMode.
         * Prompt vertex information is taken from the APFit by default.
         * @param inFileName Input file name.
         * @param outFileName Output file name. "out/NTagOut.root" by default.
         * @param verbose #Verbosity. #pDEFAULT by default.
         */
        NTagZBS(const char* inFileName, const char* outFileName="out/NTagOut.root",
                Verbosity verbose=pDEFAULT);
        ~NTagZBS();

        // File I/O
        void OpenFile();  ///< @brief Opens ZBS file.
        void CloseFile(); ///< @brief Opens ZBS file.

        void SetFitInfo(); ///< @brief Specifies how to set prompt vertex information.
};

#endif