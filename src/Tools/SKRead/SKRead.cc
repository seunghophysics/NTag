#include <TString.h>

#include <skheadC.h>
#undef MAXHWSK
#include <fortran_interface.h>

#include "SKLibs.hh"

#include "SKRead.hh"

bool SKRead::Initialize()
{
    // input file name
    TString inFilePath = "/disk02/usr6/han/gd/mc/out/annri/cmb/annri_000.cmb";

    skoptn_("31,30,26,25", 11);
    skheadg_.sk_geometry = 6; geoset_();
    
    int lun = 10;
    if (false) {
        skroot_open_read_(&lun);
        skroot_set_input_file_(&lun, inFilePath.Data(), inFilePath.Length());
        skroot_init_(&lun);
    }
    // ZEBRA
    else {
        // Initialize ZEBRA
        kzinit_();
        
        // Set rflist and open file
        int fileIndex = 1; // 1st file in rflist
        int openError;
    
        set_rflist_(&lun, inFilePath.Data(), 
                    "LOCAL", "", "RED", "", "", "recl=5670 status=old", "", "",
                    inFilePath.Length(), 5, 0, 3, 0, 0, 20, 0, 0);
        skopenf_(&lun, &fileIndex, "Z", &openError);
    
        if (openError) {
            //Log("Error occurred while opening the input ZEBRA file: " + inFilePath);
            return false;    
        }
    }

    return true;
}

bool SKRead::Execute()
{
    int readStatus;
    int lun = 10;
    readStatus = skread_(&lun);
    std::cout << "readStatus: " << readStatus << std::endl;
    return true;
}

bool SKRead::Finalize()
{
    int lun = 10;
    
    if (false) {
        skroot_close_(&lun);
        skroot_end_();
    }
    else {
        skclosef_(&lun);
    }

    return true;
}