#include "NTagROOTTQReader.hh"

NTagROOTTQReader::NTagROOTTQReader(const char* inFileName, const char* outFileName, Verbosity verbose)
: NTagROOT(inFileName, outFileName, verbose) { bUseTMVA = false; }
NTagROOTTQReader::~NTagROOTTQReader() {}

void NTagROOTTQReader::ReadEvent()
{
    AppendRawHitInfo();

    rawtqTree->Fill();
    Clear();
    
    nProcessedEvents++;
}

void NTagROOTTQReader::WriteOutput()
{
    outFile->cd();
    rawtqTree->Write();
    outFile->Close();
}