#include <TString.h>

#include <skheadC.h>
#undef MAXHWSK
#include <fortran_interface.h>
#undef MAXPM
#undef MAXPMA

#include "SKLibs.hh"
#include "SKRead.hh"

bool SKRead::Initialize()
{
    // Read options from config file
    TString inFilePath, skOptions;
    int skBadChOption, skGeometry, refRunNo;
    sharedData->ntagInfo.Get("input_file_path", inFilePath);
    sharedData->ntagInfo.Get("sk_options", skOptions);
    sharedData->ntagInfo.Get("sk_bad_channel_option", skBadChOption);
    sharedData->ntagInfo.Get("sk_geometry", skGeometry);
    sharedData->ntagInfo.Get("reference_run", refRunNo);

    if (!fileExists(inFilePath.Data())) {
        Log("Input file does not exist. Aborting NTag!", pERROR);
        return false;
    }

    inputIsSKROOT = inFilePath.EndsWith(".root");
    readStatus = 0;

    //////////////////////////////////////////
    // Set read options before opening file //
    //////////////////////////////////////////

    skOptions.ReplaceAll(';', ',');
    skOptions.Remove(TString::kBoth, '"');
    skoptn_(skOptions.Data(), skOptions.Length());
    skheadg_.sk_geometry = skGeometry; geoset_();
    
    // Custom bad-channel masking (M. Harada)
    // 25: mask bad channel
    // 26: read bad channel from input file
    if ( !skOptions.Contains("25") && !skOptions.Contains("26") ) {
        Log(Form("Masking bad channels with reference run %d", refRunNo));
        int refSubRunNo = 0;
        int outputErrorStatus = 0;
        skbadch_(&refRunNo, &refSubRunNo, &outputErrorStatus);
        skbadopt_(&skBadChOption);
    }

    ///////////////
    // Open file //
    ///////////////

    int lun = 10;
    // SKROOT
    if (inputIsSKROOT) {
        skroot_open_read_(&lun);
        skroot_set_input_file_(&lun, inFilePath.Data(), inFilePath.Length());
        skroot_init_(&lun);
    }
    // ZEBRA
    else {
        kzinit_(); // Initialize ZEBRA

        // Set rflist and open file
        int fileIndex = 1; // 1st file in rflist
        int openError;

        set_rflist_(&lun, inFilePath.Data(),
                    "LOCAL", "", "RED", "", "", "recl=5670 status=old", "", "",
                    inFilePath.Length(), 5, 0, 3, 0, 0, 20, 0, 0);
        skopenf_(&lun, &fileIndex, "Z", &openError);

        if (openError) {
            Log("Error occurred while opening the input ZEBRA file: " + inFilePath, pERROR);
            return false;
        }
    }

    return true;
}

bool SKRead::CheckSafety()
{
    safeToExecute = true;
    return safeToExecute;
}

bool SKRead::Execute()
{
    int lun = 10;
    readStatus = skread_(&lun);

    if (!exeCounter) {
        if (skhead_.nrunsk == 999999)
            sharedData->ntagInfo.Set("is_mc", true);
        else
            sharedData->ntagInfo.Set("is_mc", false);
    }

    switch (readStatus) {
        case readOK: {
            Log(Form("Read event #%d successfully.", exeCounter+1));
            return true;
        }
        case readError: {
            Log("Read-error occured!", pWARNING);
            throw eSKIPEVENT;
            return false;
        }
        case readEOF: {
            Log("Reached end-of-file.");
            throw eENDRUN;
            return true;
        }
    }
        
    return true;
}

bool SKRead::Finalize()
{
    int lun = 10;

    if (inputIsSKROOT) {
        skroot_close_(&lun);
        skroot_end_();
    }
    else {
        skclosef_(&lun);
    }

    return true;
}