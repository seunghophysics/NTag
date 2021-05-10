#ifndef LOGGER_HH
#define LOGGER_HH

#define RED "\033[38;5;196m"
#define DARKRED "\033[38;5;88m"
#define GREEN "\033[38;5;46m"
#define DARKGREEN "\033[38;5;22m"
#define BLUE "\033[38;5;21m"
#define DARKBLUE "\033[38;5;18m"
#define YELLOW "\033[38;5;226m"
#define DARKYELLOW "\033[38;5;142m"
#define ORANGE "\033[38;5;208m"
#define DARKORANGE "\033[38;5;130m"
#define PINK "\033[38;5;201m"
#define DARKPINK "\033[38;5;129m"
#define PURPLE "\033[38;5;57m"
#define DARKPURPLE "\033[38;5;54m"
#define CYAN  "\033[38;5;51m"
#define DARKCYAN  "\033[38;5;39m"
#define WHITE "\033[38;5;255m"
#define GRAY "\033[38;5;243m"
#define PLAIN "\033[0m"

#include <iostream>
#include <fstream>

enum Verbosity
{
    pNONE,    ///< NTag prints no message.
    pERROR,   ///< NTag prints error messages only.
    pWARNING, ///< NTag prints warning and error messages.
    pDEFAULT, ///< NTag prints all messages except debug messages.
    pDEBUG    ///< NTag prints all messages including debug messages.
};

class Logger;

bool fileExists(const std::string& filename);
template <class T>
Logger& operator<<(Logger& logger, T in);

class Logger
{
    public:
        Logger(Verbosity verbose=pDEFAULT);
        Logger(std::string filePath, Verbosity verbose=pDEFAULT);
        ~Logger();

        inline bool StreamsToCerr() const { return useCerr; }
        inline void StreamToCerr(bool b) { useCerr = b; }

        template <typename T>
        void Log(T msg, Verbosity msgType=pDEFAULT)
        {
            if (msgType <= fVerbosity) {
                if (msgType == pERROR) {
                    StreamToCerr(true);
                    (*this) << RED << msg;
                    ofstr.close();
                    exit(1);
                }
                else if (msgType == pWARNING) {
                    StreamToCerr(true);
                    (*this) << YELLOW << msg << PLAIN;
                    StreamToCerr(false);
                }
                else {
                    (*this) << msg;
                }
            }
        }

        void SetOutputPath(std::string outPath) { ofstr.open(outPath, std::ios_base::app); }

        void Flush() { std::cout << std::flush; }
        std::ofstream& GetOutputFileStream() { return ofstr; }

    private:
        Verbosity fVerbosity;
        bool useCerr;

        std::ofstream ofstr;
};

template <class T>
Logger& operator<<(Logger& logger, T in)
{
    logger.GetOutputFileStream() << in;
    if (logger.StreamsToCerr())
        std::cerr << in;
    else
        std::cout << in;
    return logger;
}

Logger& operator<<(Logger& logger, std::ostream& (*func)(std::ostream&));

#endif