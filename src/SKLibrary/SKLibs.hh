/*******************************************
*
* @file SKLibs.hh
*
* @brief Defines SKOFL and ATMPD functions
* to use in NTag.
*
********************************************/

#ifndef SKLIBS_HH
#define SKLIBS_HH 1

// SK I/O
extern "C" {
    void kzinit_();
    void geoset_();
    void set_rflist_(int*, const char*, const char*, const char*, const char*,
                     const char*, const char*, const char*, const char*, const char*,
                     int, int, int, int, int, int, int, int, int);
    void skopenf_(int*, int*, const char*, int*);
    void skoptn_(const char*, int);
    void skbadopt_(int*);
    void skbadch_(int*, int*, int*);
    int  skread_(int*);
    void skclosef_(int*);
    void skroot_init_(int*);
}

// data control
extern "C" {
    void  nerdnebk_(float*);
    void  skgetv_();
    void  apflscndprt_();
    void  trginfo_(float*);
    void  aprstbnk_(int*);
    void  odpc_2nd_s_(int*);
    void  inpmt_(float*, int&);
    float wallsk_(float*);
}

// BONSAI
extern "C" {
    void bonsai_ini_(int*);
    void bonsai_fit_(int*, float*, float*, float*, int*, int*, float*, float*, float*,
                     float*, float*, float*, float*, float*, float*);
    void bonsai_end_();
}

// stopmu fit
extern "C" {
    void stmfit_(float*, float*, float&, float&);
    void sparisep_(int&, int&, int&, int&);
    void pfdodirfit_(int&);
    void sppang_(int&, float&, float&);
    void spfinalsep_();
//	float pttruewaterlen_(float&);
}

#endif