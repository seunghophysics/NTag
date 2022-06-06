      subroutine filltqreal

      implicit none

#include "skparm.h"
#include "sktq.h"
#include "skpdst.h"
#include "geotnk.h"
#include "geopmt.h"
#include "skbadc.h"
#include "skvect.h"

#include "nbnk.h"
#include "apmring.h"
#include "appatsp.h"
#include "appoint.h"
#include "apversion.h"
#include "apmue.h"
#include "zbs.h"
#include "ringcom.h"

      integer ldat, nent, ipbank, ierr, i
      integer ndata, ibuf(7),ibufz(3,MAXPM*30)
      real rbuf(7),rbufz(3,MAXPM*30)
      equivalence(ibuf(1),rbuf(1))
      equivalence(ibufz(1,1),rbufz(1,1))

cc  Get 0th element of TQREAL bank

      CALL KZBLOC('TQREAL',IPBANK)

      if (IPBANK.gt.0) then
         CALL KZGET1('TQREAL',0,NDATA,IBUF)
      endif

cc  Delete TQREAL bank

      call kzbdel('TQREAL')

cc  Modify number of hits

      ibuf(1) = nqiskz

cc  Create TQREAL bank

      CALL KZBCR1('TQREAL',7,'1I 1F 5I',nqiskz,3,'1I 2F',IERR)
      CALL KZREP1('TQREAL',0,7,ibuf)

cc  Copy to TQREAL bank

      do i = 1,nqiskz
         ibufz(1,i) = icabiz(i) + ihtiflz(i)
         rbufz(2,i) = tiskz(i)
         rbufz(3,i) = qiskz(i)
      enddo

      IF (nqiskz .NE. 0) THEN
         CALL KZLDAT('TQREAL',LDAT)
         nent = 3*nqiskz
         CALL LBFCOPY(IBUFZ,IZBS(LDAT+7+1),nent)
      END IF

cc  Get 0th element of TQAREAL bank

      CALL KZBLOC('TQAREAL',IPBANK)

      if (IPBANK.gt.0) then
         CALL KZGET1('TQAREAL',0,NDATA,IBUF)
      endif

cc  Delete TQAREAL bank

      call kzbdel('TQAREAL')

cc  Modify number of hits

      ibuf(1) = nhitaz

cc  Create TQAREAL bank

      CALL KZBCR1('TQAREAL',7,'1I 1F 5I',nhitaz,3,'1I 2F',IERR)
      CALL KZREP1('TQAREAL',0,7,ibuf)

cc  Copy to TQAREAL bank

      do i = 1, nhitaz
         ibufz(1,i) = icabaz(i) + ihtflz(i)
         rbufz(2,i) = taskz(i)
         rbufz(3,i) = qaskz(i)
      enddo

      IF (nqiskz .NE. 0) THEN
         CALL KZLDAT('TQAREAL',LDAT)
         nent = 3*nhitaz
         CALL LBFCOPY(IBUFZ,IZBS(LDAT+7+1),nent)
      END IF

      return

cc   Make additional bank

C      call kzbcr0('MERGE',IERR)
C      call kzrep0('MERGE',0,'I',39,ibuf_bank)

cc   Save event

c      write(*,*)"writing bank"
c      CALL KZWRIT(20)
c      write(*,*) "done"
c      CALL KZECLR
      return

      End