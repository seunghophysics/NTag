#ifndef SKIOLIB_HH
#define SKIOLIB_HH 1

// SK I/O
extern "C" {
    void kzinit_();
    void geoset_();
    void set_rflist_(int*, const char*, const char*, const char*, const char*, 
                     const char*, const char*, const char*, const char*, const char*,
                     int, int, int, int, int, int, int, int, int);
    void skopenf_(int*, int*, const char*, int*);
    void skoptn_(const char*, int);
    int skread_(int*);
    void skclosef_(int*);
}

// bank control
extern "C" {
    void nerdnebk_(float*);
    void skgetv_();
    void apflscndprt_();
    void trginfo_(float*);
    void aprstbnk_(int*);
    void odpc_2nd_s_(int*);
    void inpmt_(float*, int&);
}

// BONSAI
extern "C" {
    void bonsai_ini_();
    void bonsai_fit_(float*, float*, float*, int*, int*, float*, float*, float*, 
                     float*, float*, float*, float*, float*, float*);
    void bonsai_end_();
    void bonsai_energy_(float*, float*, float*, float*, int*, int*, float*);
}

#endif