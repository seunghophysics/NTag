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

#include <cstddef>

// ZBS I/O
extern "C" {
    void kzinit_();
    void kzbloc_(const char*, int&, size_t);
    void kzbdel_(const char*, size_t);
    void kzbcr0_(const char*, int&, size_t);
    void kzrep0_(const char*, int&, const char*, int&, int*, size_t, size_t);
    void kznsg0_(const char*, int&, size_t);
    void kzget0_(const char*, int&, int&, int*, size_t);
    void kzwrit_(int*);
    void kzeclr_();
}

// SK I/O
extern "C" {
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
    void skdark_(int*, int*);
    void skbadch_mask_tqz_();
    void tqrealsk_();
}

// Software trigger
extern "C" {
    int softtrg_inittrgtbl_(int*, int*, int*, int*);
    void softtrg_get_cond_(int*, int*, int*, int*, int*);
    void get_run_softtrg_(int*, int*, int*, int*, int*, int*);
    void softtrg_set_cond_(int*, int*, int*, int*, int*);
}

// atmpd data control
extern "C" {
    void  nerdnebk_(float*);
    void  skgetv_();
    void  apflscndprt_();
    void  trginfo_(float*);
    void  aprstbnk_(int*);
    void  odpc_2nd_s_(int*);
    void  inpmt_(float*, int&);
    float wallsk_(float*);
    void  muechk_gate_(float*, float&, int&);
    void  apclrmue_();
    void  apgetmue_(int*);
    void  readfqzbsbank_(int*);
    float coseffsk_(float*);
}

// BONSAI
extern "C" {
    float darklf_(int*);
    void  skrunday_();
    void  skwt_();
    void  cfbsinit_(int*, float*);
    void  cfbsexit_();
    void  lfwater_(int*, float*);
    void  lfallfit_sk4_data_(float*, int*, int*);
    void  lfallfit_sk4_final_qe43_(float*, int*, int*, int*, int*);
    void  lfallfit_sk5_data_(float*, int*, int*, int*, int*);
    void  lfallfit_sk6_data_(float*, int*, int*, int*, int*);
    void  lfallfit_sk4_mc_(float*, int*, int*);
    void  lfallfit_sk4_final_qe43_mc_(float*, int*, int*, int*, int*);
    void  lfallfit_sk5_mc_(float*, int*, int*, int*, int*);
    void  lfallfit_sk6_mc_(float*, int*, int*, int*, int*);
    void  lowbs3get_();
    float effwallf_(int*, float*, float*, float*);
}

// stopmu fit
extern "C" {
    void stmfit_(float*, float*, float&, float&);
    void sparisep_(int&, int&, int&, int&);
    void pfdodirfit_(int&);
    void sppang_(int&, float&, float&);
    void spfinalsep_();
}

#endif
