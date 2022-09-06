#include <iostream>
#include <iomanip>
#include <string.h>

#include "nbnkC.h"

#include "SKLibs.hh"
#include "NTagBankIO.hh"

void ReadNTAGBank()
{
    int version;
    const char* bankName = "NTAG";
    int nameLength = strlen(bankName);

    // initialize buffer
    int bufferSize  = 8 + MAXNP*28;
    char buffer[4*bufferSize];
    float* floatBuffer = (float*)buffer;
    int* intBuffer = (int*)buffer;

    // locate NTAG bank
    int nsg;
    kznsg0_(bankName, nsg, nameLength);
    if (nsg < 0) {
      std::cerr << "NTAG bank is missing!\n";
    }
    else {
        int iSegment=0, iEntry=0, ndata;
        // fill buffer with the 0-th segment of the located bank
        kzget0_(bankName, iSegment, ndata, intBuffer, nameLength);

            version       = intBuffer[iEntry++];   // 0
            ntag_.nn      = intBuffer[iEntry++];   // 1
            ntag_.trgtype = intBuffer[iEntry++];   // 2
            ntag_.n200m   = intBuffer[iEntry++];   // 3
            ntag_.lasthit = floatBuffer[iEntry++]; // 4
            ntag_.t200m   = floatBuffer[iEntry++]; // 5

        if (version>=1)
            ntag_.np         = intBuffer[iEntry++]; // 6
        if (version>=2)
            ntag_.mctruth_nn = intBuffer[iEntry++]; // 7

        int nCandidates = version>=4 ? ntag_.np : ntag_.nn;
        for (int iCandidate=0; iCandidate<nCandidates; iCandidate++) {
                ntag_.ntime[iCandidate]     = floatBuffer[iEntry++];     // 8 + iCandidate*28 + 0
                ntag_.goodness[iCandidate]  = floatBuffer[iEntry++];     // 8 + iCandidate*28 + 1

            for(int iDim=0; iDim<3; iDim++)
                ntag_.nvx[iCandidate][iDim] = floatBuffer[iEntry++];     // 8 + iCandidate*28 + (2 to 4)
            for(int iDim=0; iDim<3; iDim++)
                ntag_.bvx[iCandidate][iDim] = floatBuffer[iEntry++];     // 8 + iCandidate*28 + (5 to 7)

                ntag_.nlow[iCandidate]      = intBuffer[iEntry++];       // 8 + iCandidate*28 + 8
                ntag_.n300[iCandidate]      = intBuffer[iEntry++];       // 8 + iCandidate*28 + 9
                ntag_.phi[iCandidate]       = floatBuffer[iEntry++];     // 8 + iCandidate*28 + 10
                ntag_.theta[iCandidate]     = floatBuffer[iEntry++];     // 8 + iCandidate*28 + 11
                ntag_.trmsold[iCandidate]   = floatBuffer[iEntry++];     // 8 + iCandidate*28 + 12
                ntag_.trmsdiff[iCandidate]  = floatBuffer[iEntry++];     // 8 + iCandidate*28 + 13
                ntag_.mintrms6[iCandidate]  = floatBuffer[iEntry++];     // 8 + iCandidate*28 + 14
                ntag_.mintrms3[iCandidate]  = floatBuffer[iEntry++];     // 8 + iCandidate*28 + 15
                ntag_.bswall[iCandidate]    = floatBuffer[iEntry++];     // 8 + iCandidate*28 + 16
                ntag_.bse[iCandidate]       = floatBuffer[iEntry++];     // 8 + iCandidate*28 + 17
                ntag_.fpdis[iCandidate]     = floatBuffer[iEntry++];     // 8 + iCandidate*28 + 18
                ntag_.bfdis[iCandidate]     = floatBuffer[iEntry++];     // 8 + iCandidate*28 + 19
                ntag_.nc[iCandidate]        = intBuffer[iEntry++];       // 8 + iCandidate*28 + 20
                ntag_.fwall[iCandidate]     = floatBuffer[iEntry++];     // 8 + iCandidate*28 + 21
                ntag_.n10[iCandidate]       = intBuffer[iEntry++];       // 8 + iCandidate*28 + 22
                ntag_.n10d[iCandidate]      = intBuffer[iEntry++];       // 8 + iCandidate*28 + 23
                ntag_.t0[iCandidate]        = floatBuffer[iEntry++];     // 8 + iCandidate*28 + 24

            if(version>=2)
                ntag_.mctruth_neutron[iCandidate] = intBuffer[iEntry++]; // 8 + iCandidate*28 + 25
            if(version>=3)
                ntag_.bse2[iCandidate] = floatBuffer[iEntry++];          // 8 + iCandidate*28 + 26
            if(version>=4)
                ntag_.tag[iCandidate] = intBuffer[iEntry++];             // 8 + iCandidate*28 + 27
        }
    }
}

void FillNTAGBank()
{
    int version = 4;
    const char* bankName = "NTAG";
    int nameLength = strlen(bankName);

    // initialize buffer
    int bufferSize = 8 + MAXNP*28;
    char buffer[4*bufferSize];
    float* floatBuffer = (float*)buffer;
    int* intBuffer = (int*)buffer;

    int bankExists;
    kzbloc_(bankName, bankExists, nameLength);    // locate bank
    if(bankExists) kzbdel_(bankName, nameLength); // delete bank if exists

    int errorStatus;
    kzbcr0_(bankName, errorStatus, nameLength);   // create bank

    int iEntry = 0;
    intBuffer[iEntry++]   = version;          // 0
    intBuffer[iEntry++]   = ntag_.nn;         // 1
    intBuffer[iEntry++]   = ntag_.trgtype;    // 2
    intBuffer[iEntry++]   = ntag_.n200m;      // 3
    floatBuffer[iEntry++] = ntag_.lasthit;    // 4
    floatBuffer[iEntry++] = ntag_.t200m;      // 5
    intBuffer[iEntry++]   = ntag_.np;         // 6
    intBuffer[iEntry++]   = ntag_.mctruth_nn; // 7

    // fill buffer with ntag common
    // each candidate has 28 saved properties
    int nCandidates = ntag_.np;
    for(int iCandidate=0; iCandidate<nCandidates; iCandidate++) {
            floatBuffer[iEntry++] = ntag_.ntime[iCandidate];           // 8 + iCandidate*28 + 0
            floatBuffer[iEntry++] = ntag_.goodness[iCandidate];        // 8 + iCandidate*28 + 1

        for(int iDim=0; iDim<3; iDim++)
            floatBuffer[iEntry++] = ntag_.nvx[iCandidate][iDim];       // 8 + iCandidate*28 + (2 to 4)
        for(int iDim=0; iDim<3; iDim++)
            floatBuffer[iEntry++] = ntag_.bvx[iCandidate][iDim];       // 8 + iCandidate*28 + (5 to 7)

            intBuffer[iEntry++]   = ntag_.nlow[iCandidate];            // 8 + iCandidate*28 + 8
            intBuffer[iEntry++]   = ntag_.n300[iCandidate];            // 8 + iCandidate*28 + 9
            floatBuffer[iEntry++] = ntag_.phi[iCandidate];             // 8 + iCandidate*28 + 10
            floatBuffer[iEntry++] = ntag_.theta[iCandidate];           // 8 + iCandidate*28 + 11
            floatBuffer[iEntry++] = ntag_.trmsold[iCandidate];         // 8 + iCandidate*28 + 12
            floatBuffer[iEntry++] = ntag_.trmsdiff[iCandidate];        // 8 + iCandidate*28 + 13
            floatBuffer[iEntry++] = ntag_.mintrms6[iCandidate];        // 8 + iCandidate*28 + 14
            floatBuffer[iEntry++] = ntag_.mintrms3[iCandidate];        // 8 + iCandidate*28 + 15
            floatBuffer[iEntry++] = ntag_.bswall[iCandidate];          // 8 + iCandidate*28 + 16
            floatBuffer[iEntry++] = ntag_.bse[iCandidate];             // 8 + iCandidate*28 + 17
            floatBuffer[iEntry++] = ntag_.fpdis[iCandidate];           // 8 + iCandidate*28 + 18
            floatBuffer[iEntry++] = ntag_.bfdis[iCandidate];           // 8 + iCandidate*28 + 19
            intBuffer[iEntry++]   = ntag_.nc[iCandidate];              // 8 + iCandidate*28 + 20
            floatBuffer[iEntry++] = ntag_.fwall[iCandidate];           // 8 + iCandidate*28 + 21
            intBuffer[iEntry++]   = ntag_.n10[iCandidate];             // 8 + iCandidate*28 + 22
            intBuffer[iEntry++]   = ntag_.n10d[iCandidate];            // 8 + iCandidate*28 + 23
            floatBuffer[iEntry++] = ntag_.t0[iCandidate];              // 8 + iCandidate*28 + 24
            intBuffer[iEntry++]   = ntag_.mctruth_neutron[iCandidate]; // 8 + iCandidate*28 + 25
            floatBuffer[iEntry++] = ntag_.bse2[iCandidate];            // 8 + iCandidate*28 + 26
            intBuffer[iEntry++]   = ntag_.tag[iCandidate];             // 8 + iCandidate*28 + 27
    }

    // fill 0-th segment of NTAG bank with buffer
    int iSegment=0, iLength=1;
    kzrep0_(bankName, iSegment, "I", iEntry, intBuffer, nameLength, iLength);

    //int luno = 20;
    //kzwrit_(&luno); // write bank to file
    //kzeclr_();      // clear bank
}

void DumpNTAGBank()
{
    std::cout << "nn: " << ntag_.nn << "\n";
    std::cout << "trgtype: " << ntag_.trgtype << "\n";
    std::cout << "lasthit: " << ntag_.lasthit << "\n";
    std::cout << "n200m: " << ntag_.n200m << "\n";
    std::cout << "t200m: " << ntag_.t200m << "\n";
    std::cout << "np: " << ntag_.np << "\n";
    std::cout << "mctruth_nn: " << ntag_.mctruth_nn << "\n";

    for (int i=0; i<ntag_.np; i++) {
        std::cout << std::right << std::setw(4);
        std::cout << ntag_.goodness[i] << " ";
        std::cout << ntag_.ntime[i] << " ";
        std::cout << ntag_.nvx[i][0] << " ";
        std::cout << ntag_.nvx[i][1] << " ";
        std::cout << ntag_.nvx[i][2] << " ";
        std::cout << ntag_.bvx[i][0] << " ";
        std::cout << ntag_.bvx[i][1] << " ";
        std::cout << ntag_.bvx[i][2] << " ";
        std::cout << ntag_.nlow[i] << " ";
        std::cout << ntag_.n300[i] << " ";
        std::cout << ntag_.phi[i] << " ";
        std::cout << ntag_.theta[i] << " ";
        std::cout << ntag_.trmsold[i] << " ";
        std::cout << ntag_.trmsdiff[i] << " ";
        std::cout << ntag_.mintrms6[i] << " ";
        std::cout << ntag_.mintrms3[i] << " ";
        std::cout << ntag_.bswall[i] << " ";
        std::cout << ntag_.bse[i] << " ";
        std::cout << ntag_.fpdis[i] << " ";
        std::cout << ntag_.bfdis[i] << " ";
        std::cout << ntag_.nc[i] << " ";
        std::cout << ntag_.fwall[i] << " ";
        std::cout << ntag_.n10[i] << " ";
        std::cout << ntag_.n10d[i] << " ";
        std::cout << ntag_.t0[i] << " ";
        std::cout << ntag_.mctruth_neutron[i] << " ";
        std::cout << ntag_.bse2[i] << " ";
        std::cout << ntag_.tag[i] << " ";
        std::cout << "\n";
    }
}