/*******************************************
*
* @file NTagMessage.hh
*
* @brief Defines NTagMessage.
*
********************************************/

#ifndef NTAGMESSAGE_HH
#define NTAGMESSAGE_HH 1

#include <ctime>

#include <TString.h>

/******************************************
*
* @brief Verbosity flags for NTag classes.
*
* @see NTagMessage::PrintTag
* @see NTagMessage::Print
*
*******************************************/
enum Verbosity
{
pNONE,    ///< NTag prints no message.
pERROR,   ///< NTag prints error messages only.
pWARNING, ///< NTag prints warning and error messages.
pDEFAULT, ///< NTag prints all messages except debug messages.
pDEBUG    ///< NTag prints all messages including debug messages.
};

/******************************************
* @brief Block sizes for 
* NTagMessage::PrintBlock.
*******************************************/
enum BlockSize
{
    pMAIN = 60,
    pEVENT = 45,
    pSUBEVENT = 40,
    pCANDIDATE = 30
};

/********************************************************
 * @brief The class for printing messages.
 *
 * This class is a message printer. Use
 * NTagMessage::Print method to print message.
 * Since all NTagMessage instances in NTag classes are
 * named "msg", in most cases messages can be printed
 * by including a line (for example, a debug message)
 * `msg.Print("Example message.", pDEBUG);`.
 *
 * To include variables in the message,
 * use the method `Form` provided by CERN's ROOT package
 * and the format specifiers of C \c printf.
 * For instance:
 * `msg.Print(Form("Some float variable: %f", float_var));`
 *
 * Timer function is also provided by NTagMessage::Timer.
 *
 * @see #Verbosity
 *******************************************************/
class NTagMessage
{
    public:
        /**
         * @brief Constructor of NTagMessage.
         * @details Sample message with \c className "TempClass": [NTagTempClass] Sample message.
         * @param className The class name to print in all messages. Use the name of the owner class.
         * @param verbose #Verbosity.
         */
        NTagMessage(const char* className="", Verbosity verbose=pDEFAULT);
        ~NTagMessage();

        /**
         * @brief Prints colored (red for errors and yellow for warnings) tags.
         */
        virtual void  PrintTag(Verbosity);

        /**
         * @brief Prints one-liners.
         * @param line A line to print.
         * @param vType Message type (in #Verbosity). If \c vType &le #fVerbosity, \c line is printed.
         * @param newLine If \c false, no new line is made at the end of output.
         * @details Sample usage: `msg.Print("some message", pWARNING);`
         */
        virtual void  Print(TString line, Verbosity vType=pDEFAULT, bool newLine=true);

        /**
         * @brief Print a block.
         * @param line A line to print.
         * @param vType Message type (in #Verbosity). If \c vType &le #fVerbosity, \c line is printed.
         * @param newLine If \c false, no new line is made at the end of output.
         */
        virtual void  PrintBlock(TString line, BlockSize size=pMAIN, Verbosity vType=pDEFAULT, bool newLine=true);

        /**
         * @brief Prints time that has been taken since the input \c tStart.
         * @param line A line to print.
         * @param vType Message type (in #Verbosity). If \c vType &le #fVerbosity, \c line is printed.
         * @param tStart The start time of the timer.
         * @details A `std::clock_t` object \c tStart must have been declared before using this method.
         * Sample usage: `std::clock_t startTimer; (some codes...;) msg.Timer("Code execution", startTimer);`
         */
        virtual float Timer(TString line, std::clock_t tStart, Verbosity vType=pDEFAULT);

    private:
        const char*  fClassName;
        Verbosity    fVerbosity;
};

#endif