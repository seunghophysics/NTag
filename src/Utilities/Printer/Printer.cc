#include <iostream>

#include "Printer.hh"


void PrintNTag()
{
    std::cout << "\n\n";
    std::cout << "              _____________________________________\n"       ;
    std::cout << "                _   _     _____      _       ____  \n"       ;
    std::cout << "               | \\ |\"|   |_ \" _| U  /\"\\  uU /\"___|u\n" ;
    std::cout << "              <|  \\| |>    | |    \\/ _ \\/ \\| |  _ /\n"   ;
    std::cout << "              U| |\\  |u   /| |\\   / ___ \\  | |_| | \n"    ;
    std::cout << "               |_| \\_|   u |_|U  /_/   \\_\\  \\____| \n"   ;
    std::cout << "               ||   \\\\,-._// \\\\_  \\\\    >>  _)(|_  \n" ;
    std::cout << "               (_\")  (_/(__) (__)(__)  (__)(__)__) \n"      ;
    std::cout << "              _____________________________________\n"       ;

    PrintVersion();
}

void PrintVersion()
{
    std::cout << "                Commit ID: " << NTagVersion.substr(1, 11)
              // "              _____________________________________\n"
              << "\n                (" << NTagDate << ")\n\n" << std::endl;
}

void PrintBlock(TString line)
{
    std::string blockWall(50, '=');
    TString coloredLine = "\033[1;36m   " + line + "\033[m";
    
    std::cout << "\n" << blockWall << "\n";
    std::cout << coloredLine << "\n";
    std::cout << blockWall << "\n\n";
}