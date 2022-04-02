***********************************************************************
*
*     ------------------
      SUBROUTINE ZBSINIT
*     ------------------
*
*     (Purpose)
*       Create a bank structure ZBS (Zebra Bank Structure).
*
*     (History)
*       Sep. 24, 1991.    First version created.        J. Kanzaki
*       Sep. 25, 1998.              modified by         Y. Koshio
*
***********************************************************************
    
          IMPLICIT NONE
    
          INTEGER IDIM
          REAL ZBS
          PARAMETER (IDIM=4000000)
          COMMON/KZBS/ZBS(IDIM)
    
          CALL KZLIMIT(IDIM)
    
          RETURN
          END