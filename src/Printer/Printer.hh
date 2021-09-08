#ifndef PRINTER_HH
#define PRINTER_HH

#include <ctime>

#include <TString.h>

/******************************************
*
* @brief Verbosity flags
*
* @see Printer::PrintTag
* @see Printer::Print
*
*******************************************/
enum Verbosity
{
pNONE,    ///< Prints no message.
pERROR,   ///< Prints error messages only.
pWARNING, ///< Prints warning and error messages.
pDEFAULT, ///< Prints all messages except debug messages.
pDEBUG    ///< Prints all messages including debug messages.
};

/******************************************
* @brief Block sizes for
* Printer::PrintBlock.
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
 * Printer::Print method to print message.
 * Since all Printer instances in NTag classes are
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
 * Timer function is also provided by Printer::Timer.
 *
 * @see #Verbosity
 *******************************************************/
class Printer
{
    public:
        /**
         * @brief Constructor of Printer.
         * @details Sample message with \c className "TempClass": [NTagTempClass] Sample message.
         * @param className The class name to print in all messages. Use the name of the owner class.
         * @param verbose #Verbosity.
         */
        Printer(const char* className="", Verbosity verbose=pDEFAULT);
        ~Printer();

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

        virtual void  PrintTitle(TString line);

        /**
         * @brief Prints time that has been taken since the input \c tStart.
         * @param line A line to print.
         * @param vType Message type (in #Verbosity). If \c vType &le #fVerbosity, \c line is printed.
         * @param tStart The start time of the timer.
         * @details A `std::clock_t` object \c tStart must have been declared before using this method.
         * Sample usage: `std::clock_t startTimer; (some codes...;) msg.Timer("Code execution", startTimer);`
         */
        virtual float Timer(TString line, std::clock_t tStart, Verbosity vType=pDEFAULT);

        inline void SetVerbosity(Verbosity verbose) { fVerbosity = verbose; }

    private:
        const char*  fClassName;
        Verbosity    fVerbosity;
};

#endif