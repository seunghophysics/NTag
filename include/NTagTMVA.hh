/*******************************************
*
* @file NTagTMVA.hh
*
* @brief Defines NTagTMVA.
*
********************************************/

#ifndef NTAGTMVA_HH
#define NTAGTMVA_HH 1

#include <map>
#include <vector>
#include <string>

#include <TCut.h>
#include <TFile.h>

#include "NTagMessage.hh"
#include "NTagTMVAVariables.hh"

namespace TMVA
{
    class Reader;
    class Factory;
}

class NTagEventInfo;

/** A type definition for a pair of lower and upper limits. */
typedef std::pair<float, float>      Range;
/** A type definition for a map between a variable name and a #Range. */
typedef std::map<const char*, Range> RangeMap;


/********************************************************
 * @brief The class for TMVA processing.
 *
 * This class deals with TMVA-related processes.
 * There are two main functions of NTagTMVA,
 * one is to generate weights for various MVA methods
 * (i.e., training neural networks) and the other is to
 * apply a generated MVA weight and a method to
 * an NTag output with an \c ntvar tree.
 *
 * While this class can be standalone to fulfill the
 * aforementioned functions, this class is also a public
 * member of NTagEventInfo and handles operation related
 * to TMVA variables and calculating the classifier
 * output using the input method and weight in
 * NTagEventInfo::GetTMVAOutput. The cut conditions
 * specified by NTagTMVA::SetReaderCutRange apply to
 * both NTagEventInfo::GetTMVAOutput and
 * NTagTMVA::ApplyWeight.
 *
 * @see: NTagTMVAVariables
 *******************************************************/
class NTagTMVA
{
    public:
        /**
         * @brief Constructor of NTagTMVA.
         * @param verbose #Verbosity.
         * @details Calls NTagTMVA::Constructor.
         */
        NTagTMVA(Verbosity verbose=pDEFAULT);
        /**
         * @brief Constructor of NTagTMVA.
         * @param inFileName Input file name.
         * @param outFileName Output file name.
         * @param verbose #Verbosity.
         * @details Calls NTagTMVA::Constructor.
         */
        NTagTMVA(const char* inFileName, const char* outFileName, Verbosity verbose=pDEFAULT);
        ~NTagTMVA();

        /**
         * @brief Constructor function for NTagTMVA.
         * @details Sets up member NTagMessage and NTagTMVAVariables.
         * Sets MVA methods to use using NTagTMVA::SetMethods.
         * Sets default #fSigCut and #fBkgCut as "IsCapture == 1" and "IsCapture == 0".
         */
        void Constructor();

        /******************/
        /* Making weights */
        /******************/

        // Settings
        /**
         * @brief Specifies which MVA method to use in generating weights.
         * @details See the source code to which MVA method can be turned on or off.
         */
        void SetMethods();
        /**
         * @brief Turns on/off an MVA method in generating weights.
         * @param methodName MVA method name to turn on or off.
         */
        void UseMethod(const char* methodName) { fUse[methodName] = 1; }

        // Cuts used in training
        void SetSigCut(TString sc) { fSigCut = sc; }
        void SetBkgCut(TString bc) { fBkgCut = bc; }
        void AddSigCut(TString sc) { fSigCut += " && " + sc; }
        void AddBkgCut(TString bc) { fBkgCut += " && " + bc; }

        /**
         * @brief Generates weights with MVA methods specified with NTagTMVA::SetMethods and NTagTMVA::UseMethod.
         * @details The default path for the generated weights is \c weights/new.
         * @see <a href="https://root.cern.ch/doc/v610/TMVAClassification_8C.html">TMVA classification tutorial</a>.
         */
        void MakeWeights();

        /*******************/
        /* Applying weight */
        /*******************/

        // Settings
        /**
         * @brief Sets MVA method name and weight file path to apply. Also sets cuts for TMVA variables.
         * @param methodName MVA method name to apply.
         * @param weightFileName Path of the weight file to apply.
         * @details The default cuts for TMVA variables are specified in the source code.
         * @see NTagTMVA::SetReaderCutRange
         */
        void SetReader(TString methodName, TString weightFileName);
        /**
         * @brief Instantiate the TMVA reader #fReader and add variables to it. Also books the specified MVA method.
         */
        void InstantiateReader();

        // Cuts used in TMVA output generation
        /**
         * @brief Sets cut range for the TMVA variables input to the reader #fReader.
         * @details Add `key` and the specified #Range (`low`, `up`) to the private member #RangeMap #fRangeMap.
         * @param key A name of the variable.
         * @param low Lower limit for the variable `key`.
         * @param up Upper limit for the variable `key`.
         * @see NTagTMVAVariables::Clear for the available `key`s.
         */
        void SetReaderCutRange(const char* key, float low, float up) { fRangeMap[key] = Range(low, up); }
        /**
         * @brief Prints out all specified cuts to the output stream.
         */
        void DumpReaderCutRange();
        /**
         * @brief Checks if the variable `key` is within the cut range specified by NTagTMVA::SetReaderCutRange.
         * @return \c true if the variable \c key passes the cut, otherwise \c false.
         */
        bool IsInRange(const char* key);
        /**
         * @brief Checks if the capture candidate passes all cuts specified by NTagTMVA::SetReaderCutRange.
         * @return The result of the && operator applied to each return value of NTagTMVA::IsInRange with each variable.
         */
        bool CandidateCut();

        // TMVA output from TMVAVariables in event
        /**
         * @brief If the capture candidate passes variable cuts specified by NTagTMVA::SetReaderCutRange,
         * return classifier output of the capture candidate.
         * @param iCandidate The index of the input neutron capture candidate.
         * (0 < \c iCandidate < #NTagEventInfo::nCandidates)
         * @return The return value of #fReader->EvaluateMVA.
         */
        float GetOutputFromCandidate(int iCandidate);

        /**
         * @brief Applies MVA weight and method to an NTag output with \c ntvar tree.
         * @param methodName MVA method name to apply.
         * @param weightFileName Path of the weight file to apply.
         * @details Creates the branch "TMVAOutput" and fills the branch with classifier outputs evaluated
         * with the specified MVA method and weight. Replaces the branch if the branch name exists.
         * The default output name for an input name `example_input.root` is `example_input_(methodName).root`.
         */
        void ApplyWeight(TString methodName, TString weightFileName);

    private:
        NTagTMVAVariables fVariables;
        NTagMessage       msg;

        Verbosity fVerbosity;

        const char* fInFileName;
        const char* fOutFileName;

        // Making weights
        std::map<std::string, int> fUse;
        TMVA::Factory*             fFactory;
        TCut                       fSigCut,  ///< Signal cut
                                   fBkgCut;  ///< Background cut

        // Applying weight
        /** TMVA reader. */
        TMVA::Reader*  fReader;
        TString        fReaderMethodName;
        TString        fReaderWeightFileName;

        RangeMap fRangeMap; ///< A map of variable names to cuts (#Range).

    friend class NTagTMVAVariables;
    friend class NTagEventInfo;
    friend class NTagIO;
};

#endif