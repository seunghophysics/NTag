//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Fri May 21 11:38:29 2021 by ROOT version 5.28/00h
// from TTree h1/DST
// found on file: /disk02/atmpd6/sk4_dst/may19/fc_mc/ntuple/2500.photon.fcred.ntag.root
//////////////////////////////////////////////////////////

#ifndef NTupleReader_h
#define NTupleReader_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

class NTupleReader {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           nring;
   UInt_t          nrun;
   Int_t           nev;
   Int_t           nsub;
   UInt_t          cate;
   Float_t         potot;
   UInt_t          nhit;
   Float_t         pomax;
   Float_t         potota;
   UInt_t          nhita;
   UInt_t          nhitac;
   Float_t         pomaxa;
   Float_t         wall;
   Float_t         evis;
   Float_t         rtsum;
   Float_t         rtmax;
   Float_t         wlen;
   UInt_t          ip[10];   //[nring]
   Float_t         pos[3];
   Float_t         dir[10][3];   //[nring]
   Float_t         dirtot[3];
   Float_t         ang[10];   //[nring]
   Float_t         rtot[10];   //[nring]
   Float_t         amom[10];   //[nring]
   Float_t         rtote[10];   //[nring]
   Float_t         amome[10];   //[nring]
   Float_t         rtotm[10];   //[nring]
   Float_t         amomm[10];   //[nring]
   UInt_t          nsube;
   UInt_t          ndcy;
   UInt_t          ngate;
   UInt_t          nbye;
   Float_t         probms[10][6];   //[nring]
   Float_t         prmslg[10][6];   //[nring]
   Int_t           date[3];
   Int_t           time[4];
   Float_t         elpsday;
   Int_t           numpo[10];   //[nring]
   Float_t         apos[3];
   Float_t         adir[3];
   Float_t         aang;
   Float_t         agood;
   Float_t         wgain;
   Int_t           nbad;
   Int_t           nbada;
   Float_t         msdir[10][3][6];   //[nring]
   Float_t         amomp[10];
   Float_t         ange[10];
   Float_t         angm[10];
   Float_t         angp[10];
   Int_t           ntot[10];
   Float_t         probth[10][6];
   Float_t         probpt[10][6];
   Float_t         pi0like[2];
   Float_t         pi0_e[2][2];
   Float_t         pi0_dir[2][2][3];
   Float_t         pi0mass[2];
   Float_t         evisold;
   Float_t         evisoldxe;
   Float_t         evisnew;
   Int_t           nmue;
   UInt_t          etype[10];   //[nmue]
   Float_t         etime[10];   //[nmue]
   Float_t         epos[10][3];   //[nmue]
   Float_t         edir[10][3];   //[nmue]
   Float_t         egood[10];   //[nmue]
   Float_t         ehit[10];   //[nmue]
   Float_t         mueprob[2];
   Int_t           nmue_sel;
   Int_t           Rnring;
   Float_t         Rdir[30][3];   //[Rnring]
   Float_t         Rang[30];   //[Rnring]
   UInt_t          Riring;
   Float_t         Rtwout[30];   //[Rnring]
   Float_t         Rtwith[30];   //[Rnring]
   Float_t         Alwout;
   Float_t         Alwith;
   Float_t         Qsmi;
   Float_t         Qsmo;
   Float_t         Qexi;
   Float_t         Qexo;
   Float_t         Pe5d;
   Float_t         En5d;
   Float_t         Eh5d;
   Float_t         Pe5do;
   Float_t         En5do;
   Float_t         Eh5do;
   Float_t         Rtadd;
   Float_t         Pdgeta;
   Float_t         Pd5d;
   Float_t         Pdthre;
   Float_t         Pd5do;
   Float_t         Delpd;
   Float_t         Ropena[30];   //[Rnring]
   Int_t           Maxth;
   Float_t         Pkang;
   Float_t         Qrfct;
   Float_t         Pdfct;
   Float_t         Pkfct;
   Float_t         Agfct;
   Float_t         Dlfct;
   Int_t           Iflag;
   Float_t         Pmfct;
   Float_t         Imfct;
   Float_t         Rilike;
   Int_t           ri_ver;
   Float_t         ri_pid;
   Int_t           ri_nring;
   Float_t         ri_flag[10];   //[ri_nring]
   Float_t         ri_dlfct[10];   //[ri_nring]
   Float_t         ri_pdfct[10];   //[ri_nring]
   Float_t         ri_pkfct[10];   //[ri_nring]
   Float_t         ri_vafct[10];   //[ri_nring]
   Float_t         ri_total[10];   //[ri_nring]
   Float_t         ri_dir[10][3];   //[ri_nring]
   Float_t         ri_imfct[10];   //[ri_nring]
   Float_t         ri_pmfct[10];   //[ri_nring]
   Int_t           npar;
   Float_t         wallv;
   UInt_t          ipv[50];   //[npar]
   Float_t         posv[3];
   Float_t         dirv[50][3];   //[npar]
   Float_t         pmomv[50];   //[npar]
   Int_t           light_flag[50];   //[npar]
   Int_t           npar2;
   Float_t         wallv2[50];   //[npar2]
   UInt_t          ipv2[50];   //[npar2]
   UInt_t          iorg[50];   //[npar2]
   Float_t         posv2[50][3];   //[npar2]
   Float_t         dirv2[50][3];   //[npar2]
   Float_t         pmomv2[50];   //[npar2]
   Int_t           numnu;
   Int_t           mode;
   Int_t           ipnu[50];   //[numnu]
   Float_t         pnu[50];   //[numnu]
   Float_t         dirnu[50][3];   //[numnu]
   Float_t         flxg[3];
   Float_t         flxh01[3];
   Float_t         kflux[4];
   Float_t         bs71[3];
   Float_t         bs74[3];
   Float_t         flxf[3];
   Float_t         flxh1d[3];
   Float_t         flxb03[3];
   Float_t         flxf03[3];
   Float_t         flxh06[3];
   Float_t         flxh11[3];
   Int_t           scan[2];
   Int_t           nuceff_version;
   Int_t           charge_exchange;
   Int_t           absorbed;
   Int_t           multipi_gen;
   Int_t           scattering;
   Int_t           piless_dcy;
   Int_t           nscndprt;
   Int_t           itrkscnd[1000];   //[nscndprt]
   Int_t           istakscnd[1000];   //[nscndprt]
   Float_t         vtxscnd[1000][3];   //[nscndprt]
   Float_t         pscnd[1000][3];   //[nscndprt]
   Int_t           iprtscnd[1000];   //[nscndprt]
   Float_t         tscnd[1000];   //[nscndprt]
   Int_t           iprntprt[1000];   //[nscndprt]
   Int_t           lmecscnd[1000];   //[nscndprt]
   Int_t           iprnttrk[1000];   //[nscndprt]
   Int_t           iorgprt[1000];   //[nscndprt]
   Float_t         pprnt[1000][3];   //[nscndprt]
   Int_t           iflgscnd[1000];   //[nscndprt]
   Float_t         pprntinit[1000][3];   //[nscndprt]
   Float_t         vtxprnt[1000][3];   //[nscndprt]
   Int_t           iprntidx[1000];   //[nscndprt]
   Int_t           nchilds[1000];   //[nscndprt]
   Int_t           ichildidx[1000];   //[nscndprt]
   Int_t           Npvcscnd;
   Int_t           Nchildsvc[100];   //[Npvcscnd]
   Int_t           Ichildidxvc[100];   //[Npvcscnd]
   Int_t           Neutmode;
   Int_t           Npne;
   Int_t           Ipne[100];   //[Npne]
   Float_t         Abspne[100];   //[Npne]
   Float_t         Pvecne[100][3];   //[Npne]
   Int_t           Npvc;
   Int_t           Ipvc[100];   //[Npvc]
   Int_t           Ichvc[100];   //[Npvc]
   Int_t           Iorgvc[100];   //[Npvc]
   Int_t           Iflvc[100];   //[Npvc]
   Float_t         Abspvc[100];   //[Npvc]
   Float_t         Pvc[100][3];   //[Npvc]
   Float_t         Crsx;
   Float_t         Crsy;
   Float_t         Crsz;
   Float_t         Crsphi;
   Int_t           Numbndn;
   Int_t           Numbndp;
   Int_t           Numfrep;
   Int_t           Numatom;
   Int_t           Ibound;
   Int_t           Neutinpmt;
   Float_t         prid_ang;
   Float_t         prid_len;
   Float_t         prid_pmom;
   Float_t         prid_mang;
   Float_t         prid_plike;
   Float_t         prid_mlike;
   Int_t           prid_nused;
   UInt_t          ntag_nn;
   Int_t           ntag_trgtype;
   Int_t           ntag_n200m;
   Float_t         ntag_t200m;
   Float_t         ntag_lasthit;
   Int_t           ntag_np;
   Float_t         ntag_ntime[500];   //[ntag_np]
   Float_t         ntag_ngood[500];   //[ntag_np]
   Float_t         ntag_nvx[500][3];   //[ntag_np]
   Float_t         ntag_bvx[500][3];   //[ntag_np]
   Int_t           ntag_mctruth_nn;
   Int_t           ntag_nlow[500];   //[ntag_np]
   Int_t           ntag_n300[500];   //[ntag_np]
   Float_t         ntag_phi[500];   //[ntag_np]
   Float_t         ntag_theta[500];   //[ntag_np]
   Float_t         ntag_trmsold[500];   //[ntag_np]
   Float_t         ntag_trmsdiff[500];   //[ntag_np]
   Float_t         ntag_mintrms6[500];   //[ntag_np]
   Float_t         ntag_mintrms3[500];   //[ntag_np]
   Float_t         ntag_bswall[500];   //[ntag_np]
   Float_t         ntag_bse[500];   //[ntag_np]
   Float_t         ntag_fpdist[500];   //[ntag_np]
   Float_t         ntag_bfdist[500];   //[ntag_np]
   Float_t         ntag_fwall[500];   //[ntag_np]
   Int_t           ntag_nc[500];   //[ntag_np]
   Int_t           ntag_n10[500];   //[ntag_np]
   Float_t         ntag_t0[500];   //[ntag_np]
   Int_t           ntag_mctruth_neutron[500];   //[ntag_np]
   Float_t         ntag_bse2[500];   //[ntag_np]
   Int_t           ntag_tag[500];   //[ntag_np]
   Int_t           ntag_n10d[500];   //[ntag_np]
   Float_t         dirtotepi[3];
   Float_t         dirtotenpi[3];
   Float_t         dirtotmue[3];
   Float_t         dirsum[3];
   Float_t         etot;
   Float_t         etotepi;
   Float_t         etotenpi;
   Float_t         etotmue;
   Float_t         oscweight[2][4];
   Float_t         oscwgt;
   Float_t         live;
   Float_t         sacth;
   Float_t         sactg;
   Float_t         sacth1d;
   Float_t         ent_pos[3];
   Float_t         ent_dir[3];
   Float_t         length;
   Float_t         tr_mom1;
   Float_t         A_ent_mom;
   Float_t         A_ent_pos[3];
   Float_t         A_ent_dir[3];
   Float_t         A_ext_mom;
   Float_t         A_ext_pos[3];
   Float_t         A_ext_dir[3];
   Float_t         Fit_pos[3];
   Float_t         Fit_dir[3];
   Float_t         Fit_len;
   Float_t         Fit_mom;
   Int_t           Fit_pid;
   Int_t           Um_ehit8m;
   Int_t           Um_ohit8m;
   Float_t         Um_qent;
   Float_t         Sh_chi1p;
   Float_t         Sh_delta;
   Float_t         Sh_mean;
   Float_t         Sh_meanq;
   Int_t           Sh_id;
   Float_t         Tr_stop[3];
   Float_t         Tr_mom;
   Float_t         Tr_len;
   Float_t         Tr_len1;
   Int_t           Pid_flg;
   Float_t         Crs1;
   Float_t         Crs2;
   Int_t           iclass;
   Int_t           mu_class;
   Int_t           mu_dec;
   Float_t         mu_dir[3];
   Float_t         mu_pos[3];
   Float_t         mu_good;
   Int_t           history;
   Int_t           Pdst;
   Int_t           idoff;
   Float_t         anthit;
   Int_t           idseq;
   Float_t         tstfrac;
   Int_t           judge;
   Float_t         Upcrs1;
   Float_t         Upcrs2;
   Float_t         lst;
   Int_t           jd;
   Float_t         fjd;
   Float_t         alt;
   Float_t         azi;
   Float_t         ra;
   Float_t         dec;
   Float_t         glat;
   Float_t         glong;

   // List of branches
   TBranch        *b_nring;   //!
   TBranch        *b_nrun;   //!
   TBranch        *b_nev;   //!
   TBranch        *b_nsub;   //!
   TBranch        *b_cate;   //!
   TBranch        *b_potot;   //!
   TBranch        *b_nhit;   //!
   TBranch        *b_pomax;   //!
   TBranch        *b_potota;   //!
   TBranch        *b_nhita;   //!
   TBranch        *b_nhitac;   //!
   TBranch        *b_pomaxa;   //!
   TBranch        *b_wall;   //!
   TBranch        *b_evis;   //!
   TBranch        *b_rtsum;   //!
   TBranch        *b_rtmax;   //!
   TBranch        *b_wlen;   //!
   TBranch        *b_ip;   //!
   TBranch        *b_pos;   //!
   TBranch        *b_dir;   //!
   TBranch        *b_dirtot;   //!
   TBranch        *b_ang;   //!
   TBranch        *b_rtot;   //!
   TBranch        *b_amom;   //!
   TBranch        *b_rtote;   //!
   TBranch        *b_amome;   //!
   TBranch        *b_rtotm;   //!
   TBranch        *b_amomm;   //!
   TBranch        *b_nsube;   //!
   TBranch        *b_ndcy;   //!
   TBranch        *b_ngate;   //!
   TBranch        *b_nbye;   //!
   TBranch        *b_probms;   //!
   TBranch        *b_prmslg;   //!
   TBranch        *b_date;   //!
   TBranch        *b_time;   //!
   TBranch        *b_elpsday;   //!
   TBranch        *b_numpo;   //!
   TBranch        *b_apos;   //!
   TBranch        *b_adir;   //!
   TBranch        *b_aang;   //!
   TBranch        *b_agood;   //!
   TBranch        *b_wgain;   //!
   TBranch        *b_nbad;   //!
   TBranch        *b_nbada;   //!
   TBranch        *b_msdir;   //!
   TBranch        *b_amomp;   //!
   TBranch        *b_ange;   //!
   TBranch        *b_angm;   //!
   TBranch        *b_angp;   //!
   TBranch        *b_ntot;   //!
   TBranch        *b_probth;   //!
   TBranch        *b_probpt;   //!
   TBranch        *b_pi0like;   //!
   TBranch        *b_pi0_e;   //!
   TBranch        *b_pi0_dir;   //!
   TBranch        *b_pi0mass;   //!
   TBranch        *b_evisold;   //!
   TBranch        *b_evisoldxe;   //!
   TBranch        *b_evisnew;   //!
   TBranch        *b_nmue;   //!
   TBranch        *b_etype;   //!
   TBranch        *b_etime;   //!
   TBranch        *b_epos;   //!
   TBranch        *b_edir;   //!
   TBranch        *b_egood;   //!
   TBranch        *b_ehit;   //!
   TBranch        *b_mueprob;   //!
   TBranch        *b_nmue_sel;   //!
   TBranch        *b_Rnring;   //!
   TBranch        *b_Rdir;   //!
   TBranch        *b_Rang;   //!
   TBranch        *b_Riring;   //!
   TBranch        *b_Rtwout;   //!
   TBranch        *b_Rtwith;   //!
   TBranch        *b_Alwout;   //!
   TBranch        *b_Alwith;   //!
   TBranch        *b_Qsmi;   //!
   TBranch        *b_Qsmo;   //!
   TBranch        *b_Qexi;   //!
   TBranch        *b_Qexo;   //!
   TBranch        *b_Pe5d;   //!
   TBranch        *b_En5d;   //!
   TBranch        *b_Eh5d;   //!
   TBranch        *b_Pe5do;   //!
   TBranch        *b_En5do;   //!
   TBranch        *b_Eh5do;   //!
   TBranch        *b_Rtadd;   //!
   TBranch        *b_Pdgeta;   //!
   TBranch        *b_Pd5d;   //!
   TBranch        *b_Pdthre;   //!
   TBranch        *b_Pd5do;   //!
   TBranch        *b_Delpd;   //!
   TBranch        *b_Ropena;   //!
   TBranch        *b_Maxth;   //!
   TBranch        *b_Pkang;   //!
   TBranch        *b_Qrfct;   //!
   TBranch        *b_Pdfct;   //!
   TBranch        *b_Pkfct;   //!
   TBranch        *b_Agfct;   //!
   TBranch        *b_Dlfct;   //!
   TBranch        *b_Iflag;   //!
   TBranch        *b_Pmfct;   //!
   TBranch        *b_Imfct;   //!
   TBranch        *b_Rilike;   //!
   TBranch        *b_ri_ver;   //!
   TBranch        *b_ri_pid;   //!
   TBranch        *b_ri_nring;   //!
   TBranch        *b_ri_flag;   //!
   TBranch        *b_ri_dlfct;   //!
   TBranch        *b_ri_pdfct;   //!
   TBranch        *b_ri_pkfct;   //!
   TBranch        *b_ri_vafct;   //!
   TBranch        *b_ri_total;   //!
   TBranch        *b_ri_dir;   //!
   TBranch        *b_ri_imfct;   //!
   TBranch        *b_ri_pmfct;   //!
   TBranch        *b_npar;   //!
   TBranch        *b_wallv;   //!
   TBranch        *b_ipv;   //!
   TBranch        *b_posv;   //!
   TBranch        *b_dirv;   //!
   TBranch        *b_pmomv;   //!
   TBranch        *b_light_flag;   //!
   TBranch        *b_npar2;   //!
   TBranch        *b_wallv2;   //!
   TBranch        *b_ipv2;   //!
   TBranch        *b_iorg;   //!
   TBranch        *b_posv2;   //!
   TBranch        *b_dirv2;   //!
   TBranch        *b_pmomv2;   //!
   TBranch        *b_numnu;   //!
   TBranch        *b_mode;   //!
   TBranch        *b_ipnu;   //!
   TBranch        *b_pnu;   //!
   TBranch        *b_dirnu;   //!
   TBranch        *b_flxg;   //!
   TBranch        *b_flxh01;   //!
   TBranch        *b_kflux;   //!
   TBranch        *b_bs71;   //!
   TBranch        *b_bs74;   //!
   TBranch        *b_flxf;   //!
   TBranch        *b_flxh1d;   //!
   TBranch        *b_flxb03;   //!
   TBranch        *b_flxf03;   //!
   TBranch        *b_flxh06;   //!
   TBranch        *b_flxh11;   //!
   TBranch        *b_scan;   //!
   TBranch        *b_nuceff_version;   //!
   TBranch        *b_charge_exchange;   //!
   TBranch        *b_absorbed;   //!
   TBranch        *b_multipi_gen;   //!
   TBranch        *b_scattering;   //!
   TBranch        *b_piless_dcy;   //!
   TBranch        *b_nscndprt;   //!
   TBranch        *b_itrkscnd;   //!
   TBranch        *b_istakscnd;   //!
   TBranch        *b_vtxscnd;   //!
   TBranch        *b_pscnd;   //!
   TBranch        *b_iprtscnd;   //!
   TBranch        *b_tscnd;   //!
   TBranch        *b_iprntprt;   //!
   TBranch        *b_lmecscnd;   //!
   TBranch        *b_iprnttrk;   //!
   TBranch        *b_iorgprt;   //!
   TBranch        *b_pprnt;   //!
   TBranch        *b_iflgscnd;   //!
   TBranch        *b_pprntinit;   //!
   TBranch        *b_vtxprnt;   //!
   TBranch        *b_iprntidx;   //!
   TBranch        *b_nchilds;   //!
   TBranch        *b_ichildidx;   //!
   TBranch        *b_Npvcscnd;   //!
   TBranch        *b_Nchildsvc;   //!
   TBranch        *b_Ichildidxvc;   //!
   TBranch        *b_Neutmode;   //!
   TBranch        *b_Npne;   //!
   TBranch        *b_Ipne;   //!
   TBranch        *b_Abspne;   //!
   TBranch        *b_Pvecne;   //!
   TBranch        *b_Npvc;   //!
   TBranch        *b_Ipvc;   //!
   TBranch        *b_Ichvc;   //!
   TBranch        *b_Iorgvc;   //!
   TBranch        *b_Iflvc;   //!
   TBranch        *b_Abspvc;   //!
   TBranch        *b_Pvc;   //!
   TBranch        *b_Crsx;   //!
   TBranch        *b_Crsy;   //!
   TBranch        *b_Crsz;   //!
   TBranch        *b_Crsphi;   //!
   TBranch        *b_Numbndn;   //!
   TBranch        *b_Numbndp;   //!
   TBranch        *b_Numfrep;   //!
   TBranch        *b_Numatom;   //!
   TBranch        *b_Ibound;   //!
   TBranch        *b_Neutinpmt;   //!
   TBranch        *b_prid_ang;   //!
   TBranch        *b_prid_len;   //!
   TBranch        *b_prid_pmom;   //!
   TBranch        *b_prid_mang;   //!
   TBranch        *b_prid_plike;   //!
   TBranch        *b_prid_mlike;   //!
   TBranch        *b_prid_nused;   //!
   TBranch        *b_ntag_nn;   //!
   TBranch        *b_ntag_trgtype;   //!
   TBranch        *b_ntag_n200m;   //!
   TBranch        *b_ntag_t200m;   //!
   TBranch        *b_ntag_lasthit;   //!
   TBranch        *b_ntag_np;   //!
   TBranch        *b_ntag_ntime;   //!
   TBranch        *b_ntag_ngood;   //!
   TBranch        *b_ntag_nvx;   //!
   TBranch        *b_ntag_bvx;   //!
   TBranch        *b_ntag_mctruth_nn;   //!
   TBranch        *b_ntag_nlow;   //!
   TBranch        *b_ntag_n300;   //!
   TBranch        *b_ntag_phi;   //!
   TBranch        *b_ntag_theta;   //!
   TBranch        *b_ntag_trmsold;   //!
   TBranch        *b_ntag_trmsdiff;   //!
   TBranch        *b_ntag_mintrms6;   //!
   TBranch        *b_ntag_mintrms3;   //!
   TBranch        *b_ntag_bswall;   //!
   TBranch        *b_ntag_bse;   //!
   TBranch        *b_ntag_fpdist;   //!
   TBranch        *b_ntag_bfdist;   //!
   TBranch        *b_ntag_fwall;   //!
   TBranch        *b_ntag_nc;   //!
   TBranch        *b_ntag_n10;   //!
   TBranch        *b_ntag_t0;   //!
   TBranch        *b_ntag_mctruth_neutron;   //!
   TBranch        *b_ntag_bse2;   //!
   TBranch        *b_ntag_tag;   //!
   TBranch        *b_ntag_n10d;   //!
   TBranch        *b_dirtotepi;   //!
   TBranch        *b_dirtotenpi;   //!
   TBranch        *b_dirtotmue;   //!
   TBranch        *b_dirsum;   //!
   TBranch        *b_etot;   //!
   TBranch        *b_etotepi;   //!
   TBranch        *b_etotenpi;   //!
   TBranch        *b_etotmue;   //!
   TBranch        *b_oscweight;   //!
   TBranch        *b_oscwgt;   //!
   TBranch        *b_live;   //!
   TBranch        *b_sacth;   //!
   TBranch        *b_sactg;   //!
   TBranch        *b_sacth1d;   //!
   TBranch        *b_ent_pos;   //!
   TBranch        *b_ent_dir;   //!
   TBranch        *b_length;   //!
   TBranch        *b_tr_mom1;   //!
   TBranch        *b_A_ent_mom;   //!
   TBranch        *b_A_ent_pos;   //!
   TBranch        *b_A_ent_dir;   //!
   TBranch        *b_A_ext_mom;   //!
   TBranch        *b_A_ext_pos;   //!
   TBranch        *b_A_ext_dir;   //!
   TBranch        *b_Fit_pos;   //!
   TBranch        *b_Fit_dir;   //!
   TBranch        *b_Fit_len;   //!
   TBranch        *b_Fit_mom;   //!
   TBranch        *b_Fit_pid;   //!
   TBranch        *b_Um_ehit8m;   //!
   TBranch        *b_Um_ohit8m;   //!
   TBranch        *b_Um_qent;   //!
   TBranch        *b_Sh_chi1p;   //!
   TBranch        *b_Sh_delta;   //!
   TBranch        *b_Sh_mean;   //!
   TBranch        *b_Sh_meanq;   //!
   TBranch        *b_Sh_id;   //!
   TBranch        *b_Tr_stop;   //!
   TBranch        *b_Tr_mom;   //!
   TBranch        *b_Tr_len;   //!
   TBranch        *b_Tr_len1;   //!
   TBranch        *b_Pid_flg;   //!
   TBranch        *b_Crs1;   //!
   TBranch        *b_Crs2;   //!
   TBranch        *b_iclass;   //!
   TBranch        *b_mu_class;   //!
   TBranch        *b_mu_dec;   //!
   TBranch        *b_mu_dir;   //!
   TBranch        *b_mu_pos;   //!
   TBranch        *b_mu_good;   //!
   TBranch        *b_history;   //!
   TBranch        *b_Pdst;   //!
   TBranch        *b_idoff;   //!
   TBranch        *b_anthit;   //!
   TBranch        *b_idseq;   //!
   TBranch        *b_tstfrac;   //!
   TBranch        *b_judge;   //!
   TBranch        *b_Upcrs1;   //!
   TBranch        *b_Upcrs2;   //!
   TBranch        *b_lst;   //!
   TBranch        *b_jd;   //!
   TBranch        *b_fjd;   //!
   TBranch        *b_alt;   //!
   TBranch        *b_azi;   //!
   TBranch        *b_ra;   //!
   TBranch        *b_dec;   //!
   TBranch        *b_glat;   //!
   TBranch        *b_glong;   //!

   NTupleReader(TTree *tree=0);
   virtual ~NTupleReader();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef NTupleReader_cxx
NTupleReader::NTupleReader(TTree *tree)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("/disk02/atmpd6/sk4_dst/may19/fc_mc/ntuple/2500.photon.fcred.ntag.root");
      if (!f) {
         f = new TFile("/disk02/atmpd6/sk4_dst/may19/fc_mc/ntuple/2500.photon.fcred.ntag.root");
      }
      tree = (TTree*)gDirectory->Get("h1");

   }
   Init(tree);
}

NTupleReader::~NTupleReader()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t NTupleReader::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t NTupleReader::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (!fChain->InheritsFrom(TChain::Class()))  return centry;
   TChain *chain = (TChain*)fChain;
   if (chain->GetTreeNumber() != fCurrent) {
      fCurrent = chain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void NTupleReader::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("nring", &nring, &b_nring);
   fChain->SetBranchAddress("nrun", &nrun, &b_nrun);
   fChain->SetBranchAddress("nev", &nev, &b_nev);
   fChain->SetBranchAddress("nsub", &nsub, &b_nsub);
   fChain->SetBranchAddress("cate", &cate, &b_cate);
   fChain->SetBranchAddress("potot", &potot, &b_potot);
   fChain->SetBranchAddress("nhit", &nhit, &b_nhit);
   fChain->SetBranchAddress("pomax", &pomax, &b_pomax);
   fChain->SetBranchAddress("potota", &potota, &b_potota);
   fChain->SetBranchAddress("nhita", &nhita, &b_nhita);
   fChain->SetBranchAddress("nhitac", &nhitac, &b_nhitac);
   fChain->SetBranchAddress("pomaxa", &pomaxa, &b_pomaxa);
   fChain->SetBranchAddress("wall", &wall, &b_wall);
   fChain->SetBranchAddress("evis", &evis, &b_evis);
   fChain->SetBranchAddress("rtsum", &rtsum, &b_rtsum);
   fChain->SetBranchAddress("rtmax", &rtmax, &b_rtmax);
   fChain->SetBranchAddress("wlen", &wlen, &b_wlen);
   fChain->SetBranchAddress("ip", ip, &b_ip);
   fChain->SetBranchAddress("pos", pos, &b_pos);
   fChain->SetBranchAddress("dir", dir, &b_dir);
   fChain->SetBranchAddress("dirtot", dirtot, &b_dirtot);
   fChain->SetBranchAddress("ang", ang, &b_ang);
   fChain->SetBranchAddress("rtot", rtot, &b_rtot);
   fChain->SetBranchAddress("amom", amom, &b_amom);
   fChain->SetBranchAddress("rtote", rtote, &b_rtote);
   fChain->SetBranchAddress("amome", amome, &b_amome);
   fChain->SetBranchAddress("rtotm", rtotm, &b_rtotm);
   fChain->SetBranchAddress("amomm", amomm, &b_amomm);
   fChain->SetBranchAddress("nsube", &nsube, &b_nsube);
   fChain->SetBranchAddress("ndcy", &ndcy, &b_ndcy);
   fChain->SetBranchAddress("ngate", &ngate, &b_ngate);
   fChain->SetBranchAddress("nbye", &nbye, &b_nbye);
   fChain->SetBranchAddress("probms", probms, &b_probms);
   fChain->SetBranchAddress("prmslg", prmslg, &b_prmslg);
   fChain->SetBranchAddress("date", date, &b_date);
   fChain->SetBranchAddress("time", time, &b_time);
   fChain->SetBranchAddress("elpsday", &elpsday, &b_elpsday);
   fChain->SetBranchAddress("numpo", numpo, &b_numpo);
   fChain->SetBranchAddress("apos", apos, &b_apos);
   fChain->SetBranchAddress("adir", adir, &b_adir);
   fChain->SetBranchAddress("aang", &aang, &b_aang);
   fChain->SetBranchAddress("agood", &agood, &b_agood);
   fChain->SetBranchAddress("wgain", &wgain, &b_wgain);
   fChain->SetBranchAddress("nbad", &nbad, &b_nbad);
   fChain->SetBranchAddress("nbada", &nbada, &b_nbada);
   fChain->SetBranchAddress("msdir", msdir, &b_msdir);
   fChain->SetBranchAddress("amomp", amomp, &b_amomp);
   fChain->SetBranchAddress("ange", ange, &b_ange);
   fChain->SetBranchAddress("angm", angm, &b_angm);
   fChain->SetBranchAddress("angp", angp, &b_angp);
   fChain->SetBranchAddress("ntot", ntot, &b_ntot);
   fChain->SetBranchAddress("probth", probth, &b_probth);
   fChain->SetBranchAddress("probpt", probpt, &b_probpt);
   fChain->SetBranchAddress("pi0like", pi0like, &b_pi0like);
   fChain->SetBranchAddress("pi0_e", pi0_e, &b_pi0_e);
   fChain->SetBranchAddress("pi0_dir", pi0_dir, &b_pi0_dir);
   fChain->SetBranchAddress("pi0mass", pi0mass, &b_pi0mass);
   fChain->SetBranchAddress("evisold", &evisold, &b_evisold);
   fChain->SetBranchAddress("evisoldxe", &evisoldxe, &b_evisoldxe);
   fChain->SetBranchAddress("evisnew", &evisnew, &b_evisnew);
   fChain->SetBranchAddress("nmue", &nmue, &b_nmue);
   fChain->SetBranchAddress("etype", etype, &b_etype);
   fChain->SetBranchAddress("etime", etime, &b_etime);
   fChain->SetBranchAddress("epos", epos, &b_epos);
   fChain->SetBranchAddress("edir", edir, &b_edir);
   fChain->SetBranchAddress("egood", egood, &b_egood);
   fChain->SetBranchAddress("ehit", ehit, &b_ehit);
   fChain->SetBranchAddress("mueprob", mueprob, &b_mueprob);
   fChain->SetBranchAddress("nmue_sel", &nmue_sel, &b_nmue_sel);
   fChain->SetBranchAddress("Rnring", &Rnring, &b_Rnring);
   fChain->SetBranchAddress("Rdir", Rdir, &b_Rdir);
   fChain->SetBranchAddress("Rang", Rang, &b_Rang);
   fChain->SetBranchAddress("Riring", &Riring, &b_Riring);
   fChain->SetBranchAddress("Rtwout", Rtwout, &b_Rtwout);
   fChain->SetBranchAddress("Rtwith", Rtwith, &b_Rtwith);
   fChain->SetBranchAddress("Alwout", &Alwout, &b_Alwout);
   fChain->SetBranchAddress("Alwith", &Alwith, &b_Alwith);
   fChain->SetBranchAddress("Qsmi", &Qsmi, &b_Qsmi);
   fChain->SetBranchAddress("Qsmo", &Qsmo, &b_Qsmo);
   fChain->SetBranchAddress("Qexi", &Qexi, &b_Qexi);
   fChain->SetBranchAddress("Qexo", &Qexo, &b_Qexo);
   fChain->SetBranchAddress("Pe5d", &Pe5d, &b_Pe5d);
   fChain->SetBranchAddress("En5d", &En5d, &b_En5d);
   fChain->SetBranchAddress("Eh5d", &Eh5d, &b_Eh5d);
   fChain->SetBranchAddress("Pe5do", &Pe5do, &b_Pe5do);
   fChain->SetBranchAddress("En5do", &En5do, &b_En5do);
   fChain->SetBranchAddress("Eh5do", &Eh5do, &b_Eh5do);
   fChain->SetBranchAddress("Rtadd", &Rtadd, &b_Rtadd);
   fChain->SetBranchAddress("Pdgeta", &Pdgeta, &b_Pdgeta);
   fChain->SetBranchAddress("Pd5d", &Pd5d, &b_Pd5d);
   fChain->SetBranchAddress("Pdthre", &Pdthre, &b_Pdthre);
   fChain->SetBranchAddress("Pd5do", &Pd5do, &b_Pd5do);
   fChain->SetBranchAddress("Delpd", &Delpd, &b_Delpd);
   fChain->SetBranchAddress("Ropena", Ropena, &b_Ropena);
   fChain->SetBranchAddress("Maxth", &Maxth, &b_Maxth);
   fChain->SetBranchAddress("Pkang", &Pkang, &b_Pkang);
   fChain->SetBranchAddress("Qrfct", &Qrfct, &b_Qrfct);
   fChain->SetBranchAddress("Pdfct", &Pdfct, &b_Pdfct);
   fChain->SetBranchAddress("Pkfct", &Pkfct, &b_Pkfct);
   fChain->SetBranchAddress("Agfct", &Agfct, &b_Agfct);
   fChain->SetBranchAddress("Dlfct", &Dlfct, &b_Dlfct);
   fChain->SetBranchAddress("Iflag", &Iflag, &b_Iflag);
   fChain->SetBranchAddress("Pmfct", &Pmfct, &b_Pmfct);
   fChain->SetBranchAddress("Imfct", &Imfct, &b_Imfct);
   fChain->SetBranchAddress("Rilike", &Rilike, &b_Rilike);
   fChain->SetBranchAddress("ri_ver", &ri_ver, &b_ri_ver);
   fChain->SetBranchAddress("ri_pid", &ri_pid, &b_ri_pid);
   fChain->SetBranchAddress("ri_nring", &ri_nring, &b_ri_nring);
   fChain->SetBranchAddress("ri_flag", ri_flag, &b_ri_flag);
   fChain->SetBranchAddress("ri_dlfct", ri_dlfct, &b_ri_dlfct);
   fChain->SetBranchAddress("ri_pdfct", ri_pdfct, &b_ri_pdfct);
   fChain->SetBranchAddress("ri_pkfct", ri_pkfct, &b_ri_pkfct);
   fChain->SetBranchAddress("ri_vafct", ri_vafct, &b_ri_vafct);
   fChain->SetBranchAddress("ri_total", ri_total, &b_ri_total);
   fChain->SetBranchAddress("ri_dir", ri_dir, &b_ri_dir);
   fChain->SetBranchAddress("ri_imfct", ri_imfct, &b_ri_imfct);
   fChain->SetBranchAddress("ri_pmfct", ri_pmfct, &b_ri_pmfct);
   fChain->SetBranchAddress("npar", &npar, &b_npar);
   fChain->SetBranchAddress("wallv", &wallv, &b_wallv);
   fChain->SetBranchAddress("ipv", ipv, &b_ipv);
   fChain->SetBranchAddress("posv", posv, &b_posv);
   fChain->SetBranchAddress("dirv", dirv, &b_dirv);
   fChain->SetBranchAddress("pmomv", pmomv, &b_pmomv);
   fChain->SetBranchAddress("light_flag", light_flag, &b_light_flag);
   fChain->SetBranchAddress("npar2", &npar2, &b_npar2);
   fChain->SetBranchAddress("wallv2", wallv2, &b_wallv2);
   fChain->SetBranchAddress("ipv2", ipv2, &b_ipv2);
   fChain->SetBranchAddress("iorg", iorg, &b_iorg);
   fChain->SetBranchAddress("posv2", posv2, &b_posv2);
   fChain->SetBranchAddress("dirv2", dirv2, &b_dirv2);
   fChain->SetBranchAddress("pmomv2", pmomv2, &b_pmomv2);
   fChain->SetBranchAddress("numnu", &numnu, &b_numnu);
   fChain->SetBranchAddress("mode", &mode, &b_mode);
   fChain->SetBranchAddress("ipnu", ipnu, &b_ipnu);
   fChain->SetBranchAddress("pnu", pnu, &b_pnu);
   fChain->SetBranchAddress("dirnu", dirnu, &b_dirnu);
   fChain->SetBranchAddress("flxg", flxg, &b_flxg);
   fChain->SetBranchAddress("flxh01", flxh01, &b_flxh01);
   fChain->SetBranchAddress("kflux", kflux, &b_kflux);
   fChain->SetBranchAddress("bs71", bs71, &b_bs71);
   fChain->SetBranchAddress("bs74", bs74, &b_bs74);
   fChain->SetBranchAddress("flxf", flxf, &b_flxf);
   fChain->SetBranchAddress("flxh1d", flxh1d, &b_flxh1d);
   fChain->SetBranchAddress("flxb03", flxb03, &b_flxb03);
   fChain->SetBranchAddress("flxf03", flxf03, &b_flxf03);
   fChain->SetBranchAddress("flxh06", flxh06, &b_flxh06);
   fChain->SetBranchAddress("flxh11", flxh11, &b_flxh11);
   fChain->SetBranchAddress("scan", scan, &b_scan);
   fChain->SetBranchAddress("nuceff_version", &nuceff_version, &b_nuceff_version);
   fChain->SetBranchAddress("charge_exchange", &charge_exchange, &b_charge_exchange);
   fChain->SetBranchAddress("absorbed", &absorbed, &b_absorbed);
   fChain->SetBranchAddress("multipi_gen", &multipi_gen, &b_multipi_gen);
   fChain->SetBranchAddress("scattering", &scattering, &b_scattering);
   fChain->SetBranchAddress("piless_dcy", &piless_dcy, &b_piless_dcy);
   fChain->SetBranchAddress("nscndprt", &nscndprt, &b_nscndprt);
   fChain->SetBranchAddress("itrkscnd", itrkscnd, &b_itrkscnd);
   fChain->SetBranchAddress("istakscnd", istakscnd, &b_istakscnd);
   fChain->SetBranchAddress("vtxscnd", vtxscnd, &b_vtxscnd);
   fChain->SetBranchAddress("pscnd", pscnd, &b_pscnd);
   fChain->SetBranchAddress("iprtscnd", iprtscnd, &b_iprtscnd);
   fChain->SetBranchAddress("tscnd", tscnd, &b_tscnd);
   fChain->SetBranchAddress("iprntprt", iprntprt, &b_iprntprt);
   fChain->SetBranchAddress("lmecscnd", lmecscnd, &b_lmecscnd);
   fChain->SetBranchAddress("iprnttrk", iprnttrk, &b_iprnttrk);
   fChain->SetBranchAddress("iorgprt", iorgprt, &b_iorgprt);
   fChain->SetBranchAddress("pprnt", pprnt, &b_pprnt);
   fChain->SetBranchAddress("iflgscnd", iflgscnd, &b_iflgscnd);
   fChain->SetBranchAddress("pprntinit", pprntinit, &b_pprntinit);
   fChain->SetBranchAddress("vtxprnt", vtxprnt, &b_vtxprnt);
   fChain->SetBranchAddress("iprntidx", iprntidx, &b_iprntidx);
   fChain->SetBranchAddress("nchilds", nchilds, &b_nchilds);
   fChain->SetBranchAddress("ichildidx", ichildidx, &b_ichildidx);
   fChain->SetBranchAddress("Npvcscnd", &Npvcscnd, &b_Npvcscnd);
   fChain->SetBranchAddress("Nchildsvc", Nchildsvc, &b_Nchildsvc);
   fChain->SetBranchAddress("Ichildidxvc", Ichildidxvc, &b_Ichildidxvc);
   fChain->SetBranchAddress("Neutmode", &Neutmode, &b_Neutmode);
   fChain->SetBranchAddress("Npne", &Npne, &b_Npne);
   fChain->SetBranchAddress("Ipne", Ipne, &b_Ipne);
   fChain->SetBranchAddress("Abspne", Abspne, &b_Abspne);
   fChain->SetBranchAddress("Pvecne", Pvecne, &b_Pvecne);
   fChain->SetBranchAddress("Npvc", &Npvc, &b_Npvc);
   fChain->SetBranchAddress("Ipvc", Ipvc, &b_Ipvc);
   fChain->SetBranchAddress("Ichvc", Ichvc, &b_Ichvc);
   fChain->SetBranchAddress("Iorgvc", Iorgvc, &b_Iorgvc);
   fChain->SetBranchAddress("Iflvc", Iflvc, &b_Iflvc);
   fChain->SetBranchAddress("Abspvc", Abspvc, &b_Abspvc);
   fChain->SetBranchAddress("Pvc", Pvc, &b_Pvc);
   fChain->SetBranchAddress("Crsx", &Crsx, &b_Crsx);
   fChain->SetBranchAddress("Crsy", &Crsy, &b_Crsy);
   fChain->SetBranchAddress("Crsz", &Crsz, &b_Crsz);
   fChain->SetBranchAddress("Crsphi", &Crsphi, &b_Crsphi);
   fChain->SetBranchAddress("Numbndn", &Numbndn, &b_Numbndn);
   fChain->SetBranchAddress("Numbndp", &Numbndp, &b_Numbndp);
   fChain->SetBranchAddress("Numfrep", &Numfrep, &b_Numfrep);
   fChain->SetBranchAddress("Numatom", &Numatom, &b_Numatom);
   fChain->SetBranchAddress("Ibound", &Ibound, &b_Ibound);
   fChain->SetBranchAddress("Neutinpmt", &Neutinpmt, &b_Neutinpmt);
   fChain->SetBranchAddress("prid_ang", &prid_ang, &b_prid_ang);
   fChain->SetBranchAddress("prid_len", &prid_len, &b_prid_len);
   fChain->SetBranchAddress("prid_pmom", &prid_pmom, &b_prid_pmom);
   fChain->SetBranchAddress("prid_mang", &prid_mang, &b_prid_mang);
   fChain->SetBranchAddress("prid_plike", &prid_plike, &b_prid_plike);
   fChain->SetBranchAddress("prid_mlike", &prid_mlike, &b_prid_mlike);
   fChain->SetBranchAddress("prid_nused", &prid_nused, &b_prid_nused);
   fChain->SetBranchAddress("ntag_nn", &ntag_nn, &b_ntag_nn);
   fChain->SetBranchAddress("ntag_trgtype", &ntag_trgtype, &b_ntag_trgtype);
   fChain->SetBranchAddress("ntag_n200m", &ntag_n200m, &b_ntag_n200m);
   fChain->SetBranchAddress("ntag_t200m", &ntag_t200m, &b_ntag_t200m);
   fChain->SetBranchAddress("ntag_lasthit", &ntag_lasthit, &b_ntag_lasthit);
   fChain->SetBranchAddress("ntag_np", &ntag_np, &b_ntag_np);
   fChain->SetBranchAddress("ntag_ntime", ntag_ntime, &b_ntag_ntime);
   fChain->SetBranchAddress("ntag_ngood", ntag_ngood, &b_ntag_ngood);
   fChain->SetBranchAddress("ntag_nvx", ntag_nvx, &b_ntag_nvx);
   fChain->SetBranchAddress("ntag_bvx", ntag_bvx, &b_ntag_bvx);
   fChain->SetBranchAddress("ntag_mctruth_nn", &ntag_mctruth_nn, &b_ntag_mctruth_nn);
   fChain->SetBranchAddress("ntag_nlow", ntag_nlow, &b_ntag_nlow);
   fChain->SetBranchAddress("ntag_n300", ntag_n300, &b_ntag_n300);
   fChain->SetBranchAddress("ntag_phi", ntag_phi, &b_ntag_phi);
   fChain->SetBranchAddress("ntag_theta", ntag_theta, &b_ntag_theta);
   fChain->SetBranchAddress("ntag_trmsold", ntag_trmsold, &b_ntag_trmsold);
   fChain->SetBranchAddress("ntag_trmsdiff", ntag_trmsdiff, &b_ntag_trmsdiff);
   fChain->SetBranchAddress("ntag_mintrms6", ntag_mintrms6, &b_ntag_mintrms6);
   fChain->SetBranchAddress("ntag_mintrms3", ntag_mintrms3, &b_ntag_mintrms3);
   fChain->SetBranchAddress("ntag_bswall", ntag_bswall, &b_ntag_bswall);
   fChain->SetBranchAddress("ntag_bse", ntag_bse, &b_ntag_bse);
   fChain->SetBranchAddress("ntag_fpdist", ntag_fpdist, &b_ntag_fpdist);
   fChain->SetBranchAddress("ntag_bfdist", ntag_bfdist, &b_ntag_bfdist);
   fChain->SetBranchAddress("ntag_fwall", ntag_fwall, &b_ntag_fwall);
   fChain->SetBranchAddress("ntag_nc", ntag_nc, &b_ntag_nc);
   fChain->SetBranchAddress("ntag_n10", ntag_n10, &b_ntag_n10);
   fChain->SetBranchAddress("ntag_t0", ntag_t0, &b_ntag_t0);
   fChain->SetBranchAddress("ntag_mctruth_neutron", ntag_mctruth_neutron, &b_ntag_mctruth_neutron);
   fChain->SetBranchAddress("ntag_bse2", ntag_bse2, &b_ntag_bse2);
   fChain->SetBranchAddress("ntag_tag", ntag_tag, &b_ntag_tag);
   fChain->SetBranchAddress("ntag_n10d", ntag_n10d, &b_ntag_n10d);
   fChain->SetBranchAddress("dirtotepi", dirtotepi, &b_dirtotepi);
   fChain->SetBranchAddress("dirtotenpi", dirtotenpi, &b_dirtotenpi);
   fChain->SetBranchAddress("dirtotmue", dirtotmue, &b_dirtotmue);
   fChain->SetBranchAddress("dirsum", dirsum, &b_dirsum);
   fChain->SetBranchAddress("etot", &etot, &b_etot);
   fChain->SetBranchAddress("etotepi", &etotepi, &b_etotepi);
   fChain->SetBranchAddress("etotenpi", &etotenpi, &b_etotenpi);
   fChain->SetBranchAddress("etotmue", &etotmue, &b_etotmue);
   fChain->SetBranchAddress("oscweight", oscweight, &b_oscweight);
   fChain->SetBranchAddress("oscwgt", &oscwgt, &b_oscwgt);
   fChain->SetBranchAddress("live", &live, &b_live);
   fChain->SetBranchAddress("sacth", &sacth, &b_sacth);
   fChain->SetBranchAddress("sactg", &sactg, &b_sactg);
   fChain->SetBranchAddress("sacth1d", &sacth1d, &b_sacth1d);
   fChain->SetBranchAddress("ent_pos", ent_pos, &b_ent_pos);
   fChain->SetBranchAddress("ent_dir", ent_dir, &b_ent_dir);
   fChain->SetBranchAddress("length", &length, &b_length);
   fChain->SetBranchAddress("tr_mom1", &tr_mom1, &b_tr_mom1);
   fChain->SetBranchAddress("A_ent_mom", &A_ent_mom, &b_A_ent_mom);
   fChain->SetBranchAddress("A_ent_pos", A_ent_pos, &b_A_ent_pos);
   fChain->SetBranchAddress("A_ent_dir", A_ent_dir, &b_A_ent_dir);
   fChain->SetBranchAddress("A_ext_mom", &A_ext_mom, &b_A_ext_mom);
   fChain->SetBranchAddress("A_ext_pos", A_ext_pos, &b_A_ext_pos);
   fChain->SetBranchAddress("A_ext_dir", A_ext_dir, &b_A_ext_dir);
   fChain->SetBranchAddress("Fit_pos", Fit_pos, &b_Fit_pos);
   fChain->SetBranchAddress("Fit_dir", Fit_dir, &b_Fit_dir);
   fChain->SetBranchAddress("Fit_len", &Fit_len, &b_Fit_len);
   fChain->SetBranchAddress("Fit_mom", &Fit_mom, &b_Fit_mom);
   fChain->SetBranchAddress("Fit_pid", &Fit_pid, &b_Fit_pid);
   fChain->SetBranchAddress("Um_ehit8m", &Um_ehit8m, &b_Um_ehit8m);
   fChain->SetBranchAddress("Um_ohit8m", &Um_ohit8m, &b_Um_ohit8m);
   fChain->SetBranchAddress("Um_qent", &Um_qent, &b_Um_qent);
   fChain->SetBranchAddress("Sh_chi1p", &Sh_chi1p, &b_Sh_chi1p);
   fChain->SetBranchAddress("Sh_delta", &Sh_delta, &b_Sh_delta);
   fChain->SetBranchAddress("Sh_mean", &Sh_mean, &b_Sh_mean);
   fChain->SetBranchAddress("Sh_meanq", &Sh_meanq, &b_Sh_meanq);
   fChain->SetBranchAddress("Sh_id", &Sh_id, &b_Sh_id);
   fChain->SetBranchAddress("Tr_stop", Tr_stop, &b_Tr_stop);
   fChain->SetBranchAddress("Tr_mom", &Tr_mom, &b_Tr_mom);
   fChain->SetBranchAddress("Tr_len", &Tr_len, &b_Tr_len);
   fChain->SetBranchAddress("Tr_len1", &Tr_len1, &b_Tr_len1);
   fChain->SetBranchAddress("Pid_flg", &Pid_flg, &b_Pid_flg);
   fChain->SetBranchAddress("Crs1", &Crs1, &b_Crs1);
   fChain->SetBranchAddress("Crs2", &Crs2, &b_Crs2);
   fChain->SetBranchAddress("iclass", &iclass, &b_iclass);
   fChain->SetBranchAddress("mu_class", &mu_class, &b_mu_class);
   fChain->SetBranchAddress("mu_dec", &mu_dec, &b_mu_dec);
   fChain->SetBranchAddress("mu_dir", mu_dir, &b_mu_dir);
   fChain->SetBranchAddress("mu_pos", mu_pos, &b_mu_pos);
   fChain->SetBranchAddress("mu_good", &mu_good, &b_mu_good);
   fChain->SetBranchAddress("history", &history, &b_history);
   fChain->SetBranchAddress("Pdst", &Pdst, &b_Pdst);
   fChain->SetBranchAddress("idoff", &idoff, &b_idoff);
   fChain->SetBranchAddress("anthit", &anthit, &b_anthit);
   fChain->SetBranchAddress("idseq", &idseq, &b_idseq);
   fChain->SetBranchAddress("tstfrac", &tstfrac, &b_tstfrac);
   fChain->SetBranchAddress("judge", &judge, &b_judge);
   fChain->SetBranchAddress("Upcrs1", &Upcrs1, &b_Upcrs1);
   fChain->SetBranchAddress("Upcrs2", &Upcrs2, &b_Upcrs2);
   fChain->SetBranchAddress("lst", &lst, &b_lst);
   fChain->SetBranchAddress("jd", &jd, &b_jd);
   fChain->SetBranchAddress("fjd", &fjd, &b_fjd);
   fChain->SetBranchAddress("alt", &alt, &b_alt);
   fChain->SetBranchAddress("azi", &azi, &b_azi);
   fChain->SetBranchAddress("ra", &ra, &b_ra);
   fChain->SetBranchAddress("dec", &dec, &b_dec);
   fChain->SetBranchAddress("glat", &glat, &b_glat);
   fChain->SetBranchAddress("glong", &glong, &b_glong);
   Notify();
}

Bool_t NTupleReader::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void NTupleReader::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t NTupleReader::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef NTupleReader_cxx
