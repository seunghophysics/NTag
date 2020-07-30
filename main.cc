#include <string.h>
#include <iostream>

#include <skparmC.h>
#include <skheadC.h>
#include <sktqC.h>

#include "main.hh"

int main(int argc, char** argv)
{
    // Initialize ZEBRA
    kzinit_();

    // Set rflist and open file
    int lun = 10;
    int ipt = 1;
    int openError;
    const char* fileName = argv[1];

    set_rflist_(&lun, fileName, "LOCAL", "", "RED", "", "", "recl=5670 status=old", "", "",
			    strlen(fileName),5,0,3,0,0,20,0,0);
    skopenf_(&lun, &ipt, "Z", &openError);

    if(openError){
        std::cerr << "[SKOPENF]: File open error." << std::endl;
        return -1;
    }

    // Set SK options and SK geometry
    const char* skoptn = "31,30,26,25"; skoptn_(skoptn, strlen(skoptn));
    skheadg_.sk_geometry = 5; geoset_();

    // Read data event-by-event
    int readStatus;
    
    while(1){
	    readStatus = skread_(&lun);
	    switch (readStatus){
	        case 0: // event read
                for(int i = 0; i < sktqz_.nqiskz; i++){
                    std::cout << sktqz_.tiskz[i] << " " << sktqz_.qiskz[i] << std::endl;
                }
                break;
            case 1: // read-error
                break;
            case 2: // end of input
                std::cout << "Reached the end of input. Closing file..." << std::endl;
                skclosef_(&lun);
                break;
        }
    }

    return 0;
}