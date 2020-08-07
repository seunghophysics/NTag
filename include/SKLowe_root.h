//
//  skroot_lowe.h                06-Aug-2020    M.Harada
//  (Original Fortran version] : 03-JUL-2008    Y.Takeuchi
//
//  definition of variables for skroot_lowe files
//
//  09-MAY-2008 spaevnum,spadll,spadl->spadlt, muboy_* added  by y.t
//  22-MAY-2008 added SK-I parmeters, cleffwal, bseffwal
//  08-MAR-2014 added Ariadne parameters
//  07-OCT-2014 added T2K parameters by Koshio
//  09-JUN-2016 added software trigger parameters by takeuchi
//
      float    bsvertex[4];    // bonsai fit
      float    bsresult[6];
      float    bsdir[3];
      float    bsgood[3];
      float    bsdirks;
      float    bseffhit[12];
      float    bsenergy;
      int      bsn50;
      float    bscossun;       
      float    clvertex[4];    // clusfit
      float    clresult[4];   // clusfit
      float    cldir[3];
      float    clgoodness;
      float    cldirks;
      float    cleffhit[12];
      float    clenergy;
      int      cln50;
      float    clcossun;       
      int      latmnum;
      int      latmh;
      int      lmx24;
      double    ltimediff;      // time to the previous LE event [in raw data]
      float    lnsratio;     // NS ratio
      float    lsdir[3];       // solar direction
      int      spaevnum;       // event number of the parent muon
      float    spaloglike;     // spallation log likelihood
      float    sparesq;
      float    spadt;
      float    spadll;         // longitudinal distance
      float    spadlt;         // traversal distance [usual delta-L]
      int      spamuyn;
      float    spamugdn;
      float    posmc[3];       // MC vertex
      float    dirmc[2][3];     // MC direction (1st and 2nd particles)
      float    pabsmc[2];      // MC absolute momentum (1st & 2nd)
      float    energymc[2];    // MC energy (1st & 2nd)
      float    darkmc;        // MC dark rate for generation
      int      islekeep;
      float    bspatlik;
      float    clpatlik;
      float    lwatert;
      int      lninfo;
      int      linfo[255];
      float    rlinfo[255];
//      EQUIVALENCE [linfo(1],rlinfo(1))

//      extern struct skroot_lowe {
//        float    bsvertex[4];    // bonsai fit
//        float    bsresult[6];
//        float    bsdir[3];
//        float    bsgood[3];
//        float    bsdirks;
//        float    bseffhit[12];
//        float    bsenergy;
//        int      bsn50;
//        float    bscossun;       
//        float    clvertex[4];    // clusfit
//        float    clresult[4];   // clusfit
//        float    cldir[3];
//        float    clgoodness;
//        float    cldirks;
//        float    cleffhit[12];
//        float    clenergy;
//        int      cln50;
//        float    clcossun;       
//        int      latmnum;
//        int      latmh;
//        int      lmx24;
//        float    ltimediff;      // time to the previous LE event [in raw data]
//        float    lnsratio;     // NS ratio
//        float    lsdir[3];       // solar direction
//        int      spaevnum;       // event number of the parent muon
//        float    spaloglike;     // spallation log likelihood
//        float    sparesq;
//        float    spadt;
//        float    spadll;         // longitudinal distance
//        float    spadlt;         // traversal distance [usual delta-L]
//        int      spamuyn;
//        float    spamugdn;
//        float    posmc[3];       // MC vertex
//        float    dirmc[3][2];     // MC direction (1st and 2nd particles)
//        float    pabsmc[2];      // MC absolute momentum (1st & 2nd)
//        float    energymc[2];    // MC energy (1st & 2nd)
//        float    darkmc;        // MC dark rate for generation
//        int      islekeep;
//        float    bspatlik;
//        float    clpatlik;
//        float    lwatert;
//        int      lninfo;
//        int      linfo[255];
//        float    rlinfo[255];
//			}
//     $     ltimediff, bsvertex, bsdir, bsresult, bsgood,     
//     $     bsdirks, bseffhit, bsenergy, bsn50, bscossun,
//     $     clvertex, clresult, cldir, clgoodness,     
//     $     cldirks, cleffhit, clenergy, cln50, clcossun,
//     $     latmnum, latmh, lmx24, lnsratio, lsdir, 
//     $     spaevnum,spaloglike,sparesq,spadt,spadll,spadlt,spamuyn,spamugdn,
//     $     posmc, dirmc, pabsmc, energymc, darkmc, islekeep,
//     $     bspatlik, clpatlik, lwatert, lninfo, linfo

//*** muon info.
      float    muentpoint[3];
      float    mudir[3];
      float    mutimediff;
      float    mugoodness;
      float    muqismsk;
      int      muyn;
      int      mufast_flag;
      int      muboy_status;         // muboy status
      int      muboy_ntrack;        // number of tracks
      float    muboy_entpos[4][10];   // up to 10 tracks
      float    muboy_dir[3];         // common direction
      float    muboy_goodness;       // goodness
      float    muboy_length;         // track length
      float    muboy_dedx[200];      // dE/dX histogram
      float    mubff_entpos[3];      // bff entpoint
      float    mubff_dir[3];         // bff direction
      float    mubff_goodness;       // bff goodness
      int      muninfo;              // number of additional data in muinfo[]
      int      muinfo[255];          // additional muon data (integer)
      float    rmuinfo[255];        // additional muon data (real)
//      EQUIVALENCE [muinfo(1],rmuinfo(1))

//      extern struct skroot_mu {
//        float    muentpoint[3];
//        float    mudir[3];
//        float    mutimediff;
//        float    mugoodness;
//        float    muqismsk;
//        int      muyn;
//        int      mufast_flag;
//        int      muboy_status;         // muboy status
//        int      muboy_ntrack;        // number of tracks
//        float    muboy_entpos[4][10];   // up to 10 tracks
//        float    muboy_dir[3];         // common direction
//        float    muboy_goodness;       // goodness
//        float    muboy_length;         // track length
//        float    muboy_dedx[200];      // dE/dX histogram
//        float    mubff_entpos[3];      // bff entpoint
//        float    mubff_dir[3];         // bff direction
//        float    mubff_goodness;       // bff goodness
//        int      muninfo;              // number of additional data in muinfo[]
//        int      muinfo[255];          // additional muon data (integer)
//        float    rmuinfo[255];        // additional muon data (real)
//      muentpoint,mudir,mutimediff,mugoodness,
//     $     muqismsk,muyn,mufast_flag,muboy_status,muboy_ntrack,
//     $     muboy_entpos,muboy_dir,muboy_goodness,muboy_length,muboy_dedx,
//     $     mubff_entpos,mubff_dir,mubff_goodness,muninfo,muinfo
//			}

//*** SLE info.
      float    itwallcut;
      int      itnsel;
      float    itbsvertex[4];    // bonsai fit
      float    itbsresult[6];
      float    itbsgood[3];
      int      itnbonsai;
      float    itcfvertex[4];    // clusfit
      float    itcfresult[4];
      float    itcfgoodness;
      int      itnclusfit;

//*** to keep SK-IV SLE reduction info.
      float   itbsdir[3], itbsenergy, itbsdirks, itbswallsk;
//      EQUIVALENCE [itcfvertex(1], itbsdir(1))
//      EQUIVALENCE [itcfresult(1], itbsenergy)
//      EQUIVALENCE [itcfresult(2], itbsdirks)
//      EQUIVALENCE [itcfresult(3], itbswallsk)

//      common/skroot_sle/itwallcut, itnsel,
//     $     itbsvertex, itbsresult, itbsgood, itnbonsai,     
//     $     itcfvertex, itcfresult, itcfgoodness, itnclusfit 
    
//*** linfo **************************************************************

//*** linfo=1: flasher 
      int nflf; 
//    EQUIVALENCE [linfo(1],nflf)

//*** linfo=2: lnahit
      int lnahit; 
//    EQUIVALENCE [linfo(2],lnahit)

//*** linfo=3: lnqisk
      int lnqisk;
//    EQUIVALENCE [linfo(3],lnqisk)

//*** linfo=4: dist
      float dist;
//    EQUIVALENCE [linfo(4],dist)

//*** linfo=5: cleffwal
      float    cleffwal;
//    EQUIVALENCE [linfo(5],cleffwal)

//*** linfo=6: bseffwal
      float    bseffwal;
//    EQUIVALENCE [linfo(6],bseffwal)

//*** linfo=7: cleffh    // clusfit effective hit at lwatert
      float    cleffh;
//    EQUIVALENCE [linfo(7],cleffh)

//*** linfo=8: bseffh    // bonsai effective hit at lwatert
      float    bseffh;
//    EQUIVALENCE [linfo(8],bseffh)

//*** linfo=9: clwallsk
      float    clwallsk;
//    EQUIVALENCE [linfo(9],clwallsk)

//*** linfo=10: bswallsk
      float    bswallsk;
//    EQUIVALENCE [linfo(10],bswallsk)

//*** linfo=11,12,13: bsdir_lfdir2
      float    bsdir_lfdir2[3];
//    EQUIVALENCE [linfo(11],bsdir_lfdir2(1))

//*** linfo=14: bsenergy_lfdir2
      float    bsenergy_lfdir2;
//      EQUIVALENCE [linfo(14],bsenergy_lfdir2)

//*** linfo=15,16,17: spa_random_pos
      float    spa_random_pos[3];
//      EQUIVALENCE [linfo(15],spa_random_pos(3))

//*** linfo=18: spa_random_loglike
      float    spa_random_loglike;
//      EQUIVALENCE [linfo(18],spa_random_loglike)

//*** linfo=19: spa_random_resq
      float    spa_random_resq;
//      EQUIVALENCE [linfo(19],spa_random_resq)

//*** linfo=20: spa_random_dt
      float    spa_random_dt;
//      EQUIVALENCE [linfo(20],spa_random_dt)

//*** linfo=21: spa_random_dlt
      float    spa_random_dlt;
//      EQUIVALENCE [linfo(21],spa_random_dlt)

//*** linfo=22: spa_random_dll
      float    spa_random_dll;
//      EQUIVALENCE [linfo(22],spa_random_dll)

//*** linfo=23: bonsai N20raw
      int bsn20raw;
//      EQUIVALENCE [linfo(23],bsn20raw)

//*** linfo=24: bonsai R02
      float    bsr02;
//      EQUIVALENCE [linfo(24],bsr02)

//*** linfo=25: bonsai clik
      float    bsclik;
//      EQUIVALENCE [linfo(25],bsclik)

//*** linfo=26: bonsai ovaq
      float    bsovaq;
//      EQUIVALENCE [linfo(26],bsovaq)

//*** linfo=27-33: Ariadne
      float    adir[3],amsg,aratio,acosscat;
      int anscat;
//      EQUIVALENCE [linfo(27],adir)
//      EQUIVALENCE [linfo(30],amsg)
//      EQUIVALENCE [linfo(31],aratio)
//      EQUIVALENCE [linfo(32],anscat)
//      EQUIVALENCE [linfo(33],acosscat)

//*** linfo=34-46: solar analysis
      float    poswal[3], dir_slered[3], ene_slered, effwal_slered, ovaq_slered;
      int idtgsk_wobadch;
      int multi_spal, wit_isgood, cloud;
//      EQUIVALENCE [linfo(34],poswal(1)) // poswal from effwal
//      EQUIVALENCE [linfo(35],poswal(2)) // poswal from effwal
//      EQUIVALENCE [linfo(36],poswal(3)) // poswal from effwal
//      EQUIVALENCE [linfo(37],idtgsk_wobadch) // skdetsim output without badch mask
//      EQUIVALENCE [linfo(38],dir_slered(1)) // for reprocess/B8MC  direction
//      EQUIVALENCE [linfo(39],dir_slered(2)) // for reprocess/B8MC  direction
//      EQUIVALENCE [linfo(40],dir_slered(3)) // for reprocess/B8MC  direction
//      EQUIVALENCE [linfo(41],ene_slered)    // for reprocess/B8MC  energy
//      EQUIVALENCE [linfo(42],effwal_slered) // for reprocess/B8MC  effective wall
//      EQUIVALENCE [linfo(43],ovaq_slered)   // for reprocess/B8MC  ovaq
//      EQUIVALENCE [linfo(44],multi_spal)    // for spallation by Scott Locke
//      EQUIVALENCE [linfo(45],wit_isgood)    // for spallation by Scott Locke
//      EQUIVALENCE [linfo(46],cloud)         // for spallation by Scott Locke
    
//*** linfo=51-54: relic analysis
      float q50, angle, mult, pilike;
//      EQUIVALENCE [linfo(51],q50)           // for relic analysis
//      EQUIVALENCE [linfo(52],angle)         // for relic analysis
//      EQUIVALENCE [linfo(53],mult)          // for relic analysis
//      EQUIVALENCE [linfo(54],pilike)        // for relic analysis

//*** linfo=61-65: software trigger
      int swtrig, swtrig_thr[3];
//      EQUIVALENCE [linfo(61], swtrig)
//      EQUIVALENCE [linfo(62], swtrig_thr(0)) // threshold of trigid = 0 
//      EQUIVALENCE [linfo(63], swtrig_thr(1)) // threshold of trigid = 1 
//      EQUIVALENCE [linfo(64], swtrig_thr(2)) // threshold of trigid = 2 
//      EQUIVALENCE [linfo(65], swtrig_thr(3)) // threshold of trigid = 3 

//*** for T2K lowe [71-85]
      int t2kiseldt0, t2kn30max;
      float    t2kdt0, t2kgpstvtx, t2kn30time;
//      EQUIVALENCE [linfo(71],t2kdt0)
//      EQUIVALENCE [linfo(72],t2kgpstvtx)
//      EQUIVALENCE [linfo(73],t2kiseldt0)
//      EQUIVALENCE [linfo(74],t2kn30max)
//      EQUIVALENCE [linfo(75],t2kn30time)
//      EQUIVALENCE [linfo(76],) # need in future
//      EQUIVALENCE [linfo(77],)
//      EQUIVALENCE [linfo(78],)
//      EQUIVALENCE [linfo(79],)
//      EQUIVALENCE [linfo(80],)
//      EQUIVALENCE [linfo(81],)
//      EQUIVALENCE [linfo(82],)
//      EQUIVALENCE [linfo(83],)
//      EQUIVALENCE [linfo(84],)
//      EQUIVALENCE [linfo(85],)

//*** save SK-I info in linfo[101-141]
      int kain30, kain50, kain200;
      int kainume, kaindeno, kaimaxnn,kainnall;
      float    kaivertex[3], kaidir[3], kaigoodness, kaienergy, kaipatlik;
      float    kaidirks, clcl_goodness, clcl_vertex[4], kaieffhit[6];
      float    mrcvertex[3], mrcgoodness;
      float    spaloglike0, sparesq0, spadt0, spadlt0, spamugdn0;
      int spamuyn0,spaevnum0, kailfflag, clcl_flag;

//      EQUIVALENCE [linfo(101],kaienergy)
//      EQUIVALENCE [linfo(102],kaivertex(1))
//      EQUIVALENCE [linfo(105],kaigoodness)
//      EQUIVALENCE [linfo(106],kaidir(1))
//      EQUIVALENCE [linfo(109],kaidirks)
//      EQUIVALENCE [linfo(110],kain30)
//      EQUIVALENCE [linfo(111],kain50)
//      EQUIVALENCE [linfo(112],kain200)
//      EQUIVALENCE [linfo(113],kaipatlik)
//      EQUIVALENCE [linfo(114],kaimaxnn)
//      EQUIVALENCE [linfo(115],kainnall)
//      EQUIVALENCE [linfo(116],kainume)
//      EQUIVALENCE [linfo(117],kaindeno)
//      EQUIVALENCE [linfo(118],clcl_vertex(1))
//      EQUIVALENCE [linfo(122],clcl_goodness)
//      EQUIVALENCE [linfo(123],mrcvertex(1))
//      EQUIVALENCE [linfo(126],mrcgoodness)
//*** for muyn=0 spacut
//      EQUIVALENCE [linfo(127],spaevnum0)
//      EQUIVALENCE [linfo(128],spaloglike0)
//      EQUIVALENCE [linfo(129],sparesq0)
//      EQUIVALENCE [linfo(130],spadt0)
//      EQUIVALENCE [linfo(131],spadlt0)
//      EQUIVALENCE [linfo(132],spamuyn0)
//      EQUIVALENCE [linfo(133],spamugdn0)
//*** for B8MC
//      EQUIVALENCE [linfo(134],kailfflag)
//      EQUIVALENCE [linfo(135],clcl_flag)
//      EQUIVALENCE [linfo(136],kaieffhit(1))  // up to 141

//*** linfo **************************************************************


//*** muinfo **************************************************************

//*** rmuinfo=1: original qismsk before fix_maxqisk[]

//*** muinfo=2: number of sub-trigger 

//*** rmuinfo=3: original qimxsk before fix_maxqisk[]

//*** rmuinfo=4: qimxsk after fix_maxqisk[]
      float    muqimxsk;
//      EQUIVALENCE [muinfo(4], muqimxsk)

//*** muinfo **************************************************************
