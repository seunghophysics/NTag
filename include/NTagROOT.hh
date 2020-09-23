/*******************************************
*
* @file NTagROOT.hh
*
* @brief Defines NTagROOT.
*
********************************************/

#ifndef NTAGROOT_HH
#define NTAGROOT_HH 1

#include "NTagIO.hh"

/********************************************************
 * @brief The class for SKROOT I/O.
 *
 * This class is an inherited class of NTagIO.
 * SKROOT-specific I/O functions are implemented.
 *******************************************************/
class NTagROOT : public NTagIO
{
    public:
        /**
         * @brief Constructor of NTagROOT.
         * @details Calls NTagIO::Initialize and NTagEventInfo::SetVertexMode.
         * Prompt vertex information is taken from the BONSAI fit by default.
         * @param inFileName Input file name.
         * @param outFileName Output file name. "out/NTagOut.root" by default.
         * @param verbose #Verbosity. #pDEFAULT by default.
         */
        NTagROOT(const char* inFileName, const char* outFileName="out/NTagOut.root",
                 Verbosity verbose=pDEFAULT);
        ~NTagROOT();

        // File I/O
        void OpenFile();  ///< @brief Opens SKROOT file.
        void CloseFile(); ///< @brief Closes SKROOT file.

        /// @brief Specifies how to read secondary bank.
        void ReadSecondaries();

        /// @brief Specifies how to set prompt vertex information.
        void SetFitInfo();
};

#endif