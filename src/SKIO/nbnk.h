***********************************************
*	----------------
*	INCLUDE "nbnk.h"
*   ----------------
*   Include file for tagged neutron information
* 
*   NN				:	# of neutrons tagged
*   TRGTYPE			:	the event trigger type:	
*						0 = 40us SHE trigger only 
*						1 = 350us AFT trigger
*						2 = 500us AFT trigger
*						3 = 800us AFT trigger
*	LASTHIT			:	the last timing hit of the event. This varies, so is needed for accurate MC/data comparisons.
*   NTIME(MAXN)		:	time of each neutron (ns)
*   GOODNESS(MAXN) 	:   goodness of neutron (output of neural net)
*   NVX(MAXN,3) 	:	neutron vertex fit (min-trms)
*   BVX(MAXN,3) 	:   neutron vertex fit (bonsai, minimum 11 hits)
*
*    2013.06.14: First version by T. Irvine
*
***********************************************

#ifndef NEUTRON_H_INCLUDED_
#define NEUTRON_H_INCLUDED_
      INTEGER MAXN
      PARAMETER (MAXN = 3000)
      INTEGER MAXNP
      PARAMETER (MAXNP = 3000)
      INTEGER NN
      INTEGER TRGTYPE
      INTEGER N200M
      REAL T200M
      REAL LASTHIT
      INTEGER NP
      REAL NTIME(MAXNP)
      REAL GOODNESS(MAXNP)
      REAL NVX(3,MAXNP)
      REAL BVX(3,MAXNP)
      INTEGER NLOW(MAXNP)
      INTEGER N300(MAXNP)
      REAL PHI(MAXNP)
      REAL THETA(MAXNP)
      REAL TRMSOLD(MAXNP)
      REAL TRMSDIFF(MAXNP)
      REAL MINTRMS6(MAXNP)
      REAL MINTRMS3(MAXNP)
      REAL BSWALL(MAXNP)
      REAL BSE(MAXNP)
      REAL BSE2(MAXNP)
      REAL FPDIS(MAXNP)
      REAL BFDIS(MAXNP)
      INTEGER NC(MAXNP)
      REAL FWALL(MAXNP)
      INTEGER N10(MAXNP)
      INTEGER N10D(MAXNP)
      REAL T0(MAXNP)
      INTEGER MCTRUTH_NEUTRON(MAXNP)
      INTEGER MCTRUTH_NN
      INTEGER TAG(MAXNP)
      COMMON /NTAG/ NN, TRGTYPE, LASTHIT, GOODNESS, NTIME, 
     $      NVX, BVX,
     $      N200M, T200M, NLOW, N300, 
     $      PHI, THETA, TRMSOLD, 
     $     TRMSDIFF, MINTRMS6,
     $     MINTRMS3, BSWALL, BSE, FPDIS, BFDIS, 
     $     NC, FWALL, N10, N10D, T0, NP,
     $     MCTRUTH_NEUTRON, MCTRUTH_NN,BSE2,TAG
#endif 
