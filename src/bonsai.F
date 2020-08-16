      subroutine bonsai_ini()

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
      parameter (lun=20)

      real watert
      real dark, darklf, darks
      
c***  init water transparency table
      call skrunday
      call skwt
c***  in the case of MC, assign dark counts based on run 62428
      if (nrunsk.lt.61700) nrunsk = 62428
      dark = darklf(nrunsk)
c*** init. bonsai 
      CALL cfbsinit(MAXPM,xyzpm)
      call lfwater(relapse(nrunsk), watert)
      print *, "nrunsk/watert = ", nrunsk, watert

      return
 
      end


c------------------------------------------------------------------------------

      subroutine bonsai_fit(dt, tisksend, qisksend, cabsend, ntisksend,
     & tenergy, tvx, tvy, tvz, tvt, tgood, tbsdirks, tbspatlik, tbsovaq)

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

      real tisksend(1000), qisksend(1000)
      integer cabsend(1000)
      integer ntisksend, i, lfflag
      real dt,toff
      real x1,y1,z1, dis1, ttof(2000)
      real watert
      real effwallf, effwal
c      , poswal(3)
      real tenergy, tvx, tvy, tvz, tvt, tgood, g2d2, tbsdirks, tbspatlik, tbsovaq
c      real tisksend(ntisksend), qisksend(ntisksend)

c*** number of hit limit for clusfit
      integer NHITCUT
      parameter (NHITCUT =   1000)

c*** branches to skip
      character*100 temp
      integer   nintoskip, nouttoskip
      parameter (nintoskip  = 6)
      parameter (nouttoskip = 11)
      character*100 in_branches_to_skip(nintoskip)
      character*100 out_branches_to_skip(nouttoskip)

      real pawc
      common/pawc/pawc(6000000)
      integer ii, cab, n10, nc, nback, nlow
      real*8  like
      real offset,total,avg,ntot

c*** loop
         watert = 12431.3

c YN: Change the way time is passed to Bonsai:
         
c         total = 0
c         do i = 1, ntisksend
c            total = total + tisksend(i)
c         enddo
c         avg = total/(ntisksend)
c         offset = dt + 1000 - avg
c
c         do i = 1, ntisksend 
c            ihcab(i) = cabsend(i)
c            tisk(ihcab(i)) = tisksend(i) - dt + offset
c            qisk(ihcab(i)) = qisksend(i)
cc            print *, "tisk/qisk/cab : " , tisksend(i), tisk(ihcab(i)),qisk(ihcab(i)), ihcab(i)
c         end do


         do i = 1, ntisksend 
            ihcab(i) = cabsend(i)
            tisk(ihcab(i)) = tisksend(i) - dt + 1000
            qisk(ihcab(i)) = qisksend(i)
c            print *, "tisk/qisk/cab : " , tisksend(i), tisk(ihcab(i)),qisk(ihcab(i)), ihcab(i)
         end do

         nqisk = ntisksend

         call lfclear_all()
         write(*,*) 'before lfallfit',mxqisk
         call lfallfit_sk4_data(watert, NHITCUT, lfflag)
c         call lfallfit_sk4_data(watert, ntisksend, lfflag)
c         write(*,*) 'after lfallfit'
         effwal = effwallf(1,bsvertex,bsdir,poswal)
c*** save info

         tenergy = bsenergy
         tvx = bsvertex(1)
         tvy = bsvertex(2)
         tvz = bsvertex(3)
         tvt = bsvertex(4)
         tgood = bsgood(2)
         tbsdirks = bsdirks
         tbspatlik = bspatlik
         tbsovaq = bsovaq
c         print *, "Bonsai Results: ",  tenergy, tvx, tvy, tvz, tgood
      return

 1002 continue
 1001 continue
 10   continue
      stop
      end


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
