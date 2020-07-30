#include <string.h>

#include "skheadC.h"

extern "C" {
    void kzinit_();
    void geoset_();
    void set_rflist_(int*, const char*, const char*, const char*, const char*, 
                     const char*, const char*, const char*, const char*, const char*,
                     int, int, int, int, int, int, int, int, int);
    void skopenf_(int*, int*, const char*, int*);
}

int main(int argc, char** argv)
{
    int lun = 10;
    const char* fname = "/disk02/usr6/han/root2zbs/test.zbs";
    int npt = 1;
    int ierr;

    kzinit_();

    skheadg_.sk_geometry = 5;
    geoset_();

    set_rflist_(&lun, fname, "LOCAL", "", "RED", "", "", "recl=5670 status=old", "", "",
			    strlen(fname),5,0,3,0,0,20,0,0);
    skopenf_(&lun, &npt, "Z", &ierr);
}