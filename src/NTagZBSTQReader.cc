#include "NTagZBSTQReader.hh"

NTagZBSTQReader::NTagZBSTQReader(const char* inFileName, const char* outFileName, Verbosity verbose)
: NTagZBS(inFileName, outFileName, verbose) { bUseTMVA = false; }
NTagZBSTQReader::~NTagZBSTQReader() {}

void NTagZBSTQReader::ReadEvent()
{
    AppendRawHitInfo();

    rawtqTree->Fill();
    Clear();
}

void NTagZBSTQReader::WriteOutput()
{
    outFile->cd();
    rawtqTree->Write();
    outFile->Close();
}