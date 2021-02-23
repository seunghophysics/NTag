      subroutine bonsai_ini(refrun)

      implicit none

#include "skhead.h"
#include "skparm.h"
#include "sktq.h"
#include "skbadc.h"
#include "geopmt.h"
#include "skroot_lowe.h"
#include "skday.h"
#include "skwt.h"

      integer lun
      integer refrun
      parameter (lun=20)

      real watert
      real dark, darklf, darks

c***  initialize water transparency table
      call skrunday
      call skwt
      
c***  in case of MC, assign dark counts based on run 62428
      dark = darklf(refrun)
      
c***  initialize bonsai
      CALL cfbsinit(MAXPM,xyzpm)
      call lfwater(relapse(refrun), watert)

      return
      end
      
c------------------------------------------------------------------------------

      subroutine bonsai_fit(bdata, dt, tisksend, qisksend, cabsend, ntisksend,
     & tenergy, tvx, tvy, tvz, tvt, tgood, tdirks, tpatlik, tovaq)

      implicit none

#include "skhead.h"
#include "skparm.h"
#include "sktq.h"
#include "skbadc.h"
#include "geopmt.h"
#include "skroot_lowe.h"
#include "skday.h"
#include "skwt.h"

      integer lun, bdata

      logical duplicate
      integer ntisksend, i, j, lfflag, dupid
      real tisksend(ntisksend), qisksend(ntisksend)
      integer cabsend(ntisksend)
      real dt
      real watert
      real effwallf, effwal
      real tenergy, tvx, tvy, tvz, tvt
      real tgood, tdirks, tpatlik, tovaq

c***  number of hit limit for clusfit
      integer NHITCUT
      parameter (NHITCUT = 1000)

      real pawc, mxq
      common/pawc/pawc(6000000)

c*** loop
      watert = 12431.3
      
      do i = 1, MAXPM
         ihcab(i) = 0
         qisk(i) = 0
         tisk(i) = 0
      end do

      nqisk = 0
      mxq = 0
      mxqisk = 0
      do i = 1, ntisksend
         duplicate = .false.
         dupid = 0
         do j = 1, i-1
             if((i.ne.j).and.(cabsend(i).eq.cabsend(j))) then
                 duplicate = .true.
                 dupid = j
             endif
         enddo
         if(duplicate) then
             qisk(ihcab(dupid)) = qisk(ihcab(dupid)) + qisksend(i)
         else
             ihcab(i) = cabsend(i)
             tisk(ihcab(i)) = tisksend(i) - dt + 1000
             qisk(ihcab(i)) = qisksend(i)
             nqisk = nqisk + 1
             
             if (qisk(ihcab(i)).gt.mxq) then
                  mxq = qisk(ihcab(i))
                  mxqisk = ihcab(i)
             endif
         endif
       enddo

      call lfclear_all()
      if (bdata == 1) then
        call lfallfit_sk4_data(watert, NHITCUT, lfflag)
      else
        call lfallfit_sk4_mc(watert, NHITCUT, lfflag)
      endif
      effwal = effwallf(1,bsvertex,bsdir,poswal)
         
c*** save info
         tenergy = bsenergy
         tvx = bsvertex(1)
         tvy = bsvertex(2)
         tvz = bsvertex(3)
         tvt = bsvertex(4) + dt - 1000
         tgood = bsgood(2)
         tdirks = bsdirks
         tpatlik = bspatlik
         tovaq = bsovaq
      return

 1002 continue
 1001 continue
 10   continue
 
      stop
      end
      
c------------------------------------------------------------------------------

      subroutine bonsai_end()

      implicit none

#include "skhead.h"
#include "skparm.h"
#include "sktq.h"
#include "skbadc.h"
#include "geopmt.h"
#include "skroot_lowe.h"
#include "skday.h"
#include "skwt.h"

      call cfbsexit

      return
      end
