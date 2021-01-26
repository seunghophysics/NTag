/*******************************************
*
* @file NTagIO.hh
*
* @brief Defines NTagIO.
*
********************************************/

#ifndef NTAGIO_HH
#define NTAGIO_HH 1

#include "NTagEventInfo.hh"

/********************************************************
 * @brief The class in charge of SK data I/O.
 *
 * This class is an inherited class of NTagEventInfo.
 * The general idea is to have two main functions
 * i.e., file I/O and event processing. This
 * class is also a base class for NTagZBS and NTagROOT.
 * Format-independent functions used for file I/O are
 * implemented in this class. Format-specific functions
 * are implemented in its daughter classes.
 *
 * NTagIO, as a subclass of NTagEventInfo, uses the
 * inherited member functions to process each event.
 * Refer to NTagIO::ReadFile,
 * which uses the member functions provided by
 * NTagEventInfo in a specified order in
 * NTagIO::ReadMCEvent for MC events and
 * NTagIO::ReadDataEvent, NTagIO::ReadSHEEvent,
 * and NTagIO::ReadAFTEvent for data events, for the
 * event-wise instructions applied to the input file.
 *******************************************************/
class NTagIO : public NTagEventInfo
{
    public:
        /**
         * @brief Constructor of NTagIO class.
         * @details Basic I/O settings and trees are set up in this constructor.
         * @param inFileName Input file name.
         * @param outFileName Ouptut file name.
         * @param verbose #Verbosity.
         * @see NTagIO::CreateBranchesToNtvarTree, NTagIO::CreateBranchesToTruthTree
         */
        NTagIO(const char* inFileName, const char* outFileName, Verbosity verbose);

        /**
         * @brief Destructor of NTagIO. Ends BONSAI.
         */
        ~NTagIO();



        /////////////////
        // Initializer //
        /////////////////

        /**
         * @brief Initialize SK data options and opens the input file.
         * @see NTagIO::SKInitialize, NTagIO::OpenFile
         */
        virtual void Initialize();



        ///////////////
        // File I/O //
        //////////////

            /*******************/
            /* format-specific */
            /*******************/

            /**
             * @brief Opens SK data file.
             * @details This function must be implemented in a daughter class.
             * @see NTagROOT::OpenFile, NTagZBS::OpenFile
             */
            virtual void OpenFile() {}

            /**
             * @brief Closes SK data file.
             * @details This function must be implemented in a daughter class.
             * @see NTagROOT::CloseFile, NTagZBS::CloseFile
             */
            virtual void CloseFile() {}

            /**
             * @brief Saves prompt vertex fit information.
             * @details This function must be implemented in a daughter class.
             * If you're using APFit or BONSAI,
             * just specify either NTagEventInfo::SetAPFitInfo or NTagEventInfo::SetLowFitInfo.
             * @see NTagROOT::SetFitInfo, NTagZBS::SetFitInfo
             */
            virtual void SetFitInfo() {}


            /**********************/
            /* format-independent */
            /**********************/

            /**
             * @brief Sets SK options and geometry. Initializes Zebra and BONSAI.
             */
            virtual void SKInitialize();

            /**
             * @brief Starts reading input file and looping over events.
             * @details The steering code here is \c skread.
             * \c skread is called and all data variables copied to the SK fortran common blocks
             * are read by the member functions of NTagEventInfo if the read status of the event
             * is normal. For MC, events with prompt vertices within PMTs are skipped.
             * @see NTagIO::ReadEvent
             */
            virtual void ReadFile();

            /**
             * @brief Calls NTagIO::ReadMCEvent for MC and NTagIO::ReadDataEvent for data events.
             */
            virtual void ReadEvent();

            /**
             * @brief Instructions to each loop for an MC event.
             * @details See the source code for details.
             */
            virtual void ReadMCEvent();

            /**
             * @brief Instructions to each loop for a data event.
             * @details The trigger information is read from the common variable \c skhead_.idtgsk
             * and corresponding event instructions are called.
             * - If the trigger is SHE :
             *     NTagIO::ReadSHEEvent
             * - If the trigger is AFT :
             *     NTagIO::ReadAFTEvent
             * - If the previous event was SHE but the current event is not AFT :
             *     Start searching for neutron capture candidates with the previous SHE event only
             *     and fill the member variables of NTagEventInfo to the tree #ntvarTree.
             */
            virtual void ReadDataEvent();

            /**
             * @brief Instructions for SHE-triggered events.
             * @details Saves the prompt vertex information and the raw hit TQ vectors
             * #vTISKZ, #vQISKZ, and #vCABIZ of NTagEventInfo.
             * Does not fill #ntvarTree. #ntvarTree is filled at NTagIO::ReadAFTEvent
             * if the next event is AFT, otherwise it's filled at NTagIO::ReadMCEvent.
             */
            virtual void ReadSHEEvent();

            /**
             * @brief Instructions for HE(or NOT-SHE)-triggered events.
             * @details Saves the prompt vertex information and the raw hit TQ vectors
             * #vTISKZ, #vQISKZ, and #vCABIZ of NTagEventInfo.
             * Also, neutron capture candidates are searched for 
			 * and the member variables are filled to the tree #ntvarTree.
             */
            virtual void ReadnoSHEEvent();

            /**
             * @brief Instructions for AFT-triggered events.
             * @details Appends raw TQ information from the common \c sktqz to
             * the raw TQ hit vectors #vTISKZ, #vQISKZ, and #vCABIZ of NTagEventInfo.
             * Neutron capture candidates are searched for and the member variables are
             * filled to the tree #ntvarTree.
             */
            virtual void ReadAFTEvent();

            /** @brief Creates output file and write trees.
             * #truthTree is written only if the input file is MC.
             */
            virtual void WriteOutput();

            /** @brief Instructions for SIGINT signal, i.e., \c Ctrl+c.
             * NTagIO::WriteOutput is called.
             */
            virtual void DoWhenInterrupted();

            static void  SIGINTHandler(int sigNo);



        //////////////////
        // Tree-related //
        //////////////////

        /**
         * @brief Creates branches to #truthTree with certain member variables from MC.
         * See the source code for the branch names and filled variables.
         */
        virtual void CreateBranchesToTruthTree();

        /**
         * @brief Creates branches to #truthTree with member variables.
         * See the source code for the branch names and filled variables.
         */
        virtual void CreateBranchesToNtvarTree();

        /**
         * @brief Creates additional branches out of feature variables
         * declared in NTagCandidate::SetVariables to #ntvarTree.
         */
        virtual void AddCandidateVariablesToNtvarTree();

        /**
         * @brief Create branches to #rawtqTree with raw TQ hit vectors #vTISKZ, #vQISKZ, and #vCABIZ.
         */
        virtual void CreateBranchesToRawTQTree();

        /**
         * @brief Create branches to #restqTree with residual TQ hit vectors #vSortedT, #vSortedT, and #vSortedPMTID.
         */
        virtual void CreateBranchesToResTQTree();

        /**
         * @brief Fills trees.
         */
        virtual void FillTrees();


        ////////////
        // Others //
        ////////////

        /**
         * @brief Set source file #fSigTQFile and tree #fSigTQTree to extract signal TQ hits
         * for #vISIGZ in NTagEventInfo::AppendRawHitInfo.
         */
        void SetSignalTQ(const char* fSigTQName);

        /**
         * @brief Check if the event being processed is MC or data with the run number.
         */
        virtual void CheckMC();

    protected:
        const char* fInFileName;
        const char* fOutFileName;
        int         lun;
        bool        candidateVariablesAdded;

        TFile*      outFile;
        TTree*      truthTree; /*!< A tree of member variables. (created for MC inputs only)
                                    @see: NTagIO::CreateBranchesToTruthTree */
        TTree*      ntvarTree; /*!< A tree of member variables.
                                    @see: NTagIO::CreateBranchesToNtvarTree */
        TTree*      rawtqTree; /*!< A tree of Raw TQ hit vectors. (#vTISKZ, #vQISKZ, and #vCABIZ)
                                    @see: NTagIO::CreateBranchesToRawTQTree */
        TTree*      restqTree; /*!< A tree of Residual TQ hit vectors. (#vSortedT, #vSortedQ, and #vSortedPMTID)
                                    @see: NTagIO::CreateBranchesToResTQTree */

    private:
        static NTagIO* instance;
};

#endif
