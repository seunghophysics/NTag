#ifndef SKIOLIB_HH
#define SKIOLIB_HH 1

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

#endif