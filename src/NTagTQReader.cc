#include "NTagTQReader.hh"

NTagTQReader::NTagTQReader(const char* inFileName, const char* outFileName, Verbosity verbose)
: NTagZBS(inFileName, outFileName, verbose) { useTMVA = false; }
NTagTQReader::~NTagTQReader() {}

void NTagTQReader::ReadEvent()
{
    //SetPromptVertex();
    //SetMCInfo();
    AppendRawHitInfo();
    //SetToFSubtractedTQ();
    
    rawtqTree->Fill();
    Clear();
}

void NTagTQReader::WriteOutput()
{
    outFile->cd();
    rawtqTree->Write();
    outFile->Close();
}