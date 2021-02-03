*
      subroutine pfdodirfit(ip)
*
*    direction fit routine
*
      implicit none

#include "skparm.h"
#include "skbadc.h"
#include "geotnk.h"
#include "apmring.h"
#include "appatsp.h"
#include "apringsp.h"
#include "apmsfit.h"
#include "apfitinf.h"


      integer ip,i,ipx,ipir,k,ir,ipstat,ipend,itel,maxitel,isq
      integer iangfit,iphifit,maxstep,ic,in,npmt(2)
      real    thmax,thmin,ang,dang,dpos,tolang,tolpos,dmove0,dang0
      real    dcor,angcor
      real    iptmp(APNMAXRG),postmp(3),dirtmp(3,APNMAXRG)
      real    angtmp(APNMAXRG),posold(3),dirold(3),hpos(3),dist,t0
      parameter ( TOLANG = 0.5 ,  ! degree
     $            TOLPOS = 5.  ,  ! cm
     $            MAXITEL = 7 )

#ifdef _MSFIT_VERSION_
      pffitver = _MSFIT_VERSION_
#endif

      call ucopy(apip,iptmp,APNMAXRG)
      call ucopy(appos,postmp,3)
      call ucopy(apdir,dirtmp,APNMAXRG*3)
      call ucopy(apangcer,angtmp,APNMAXRG)

      ir = 1

      if ( ip.eq.2 .or. ip.eq.11 ) then          ! fit as e-like
         ipstat = 2
         ipend  = 2
      else if ( ip.eq.3 .or. ip.eq.13 ) then     ! fit as mu-like
         ipstat = 3
         ipend  = 3
      else                                       ! fit as both e and mu
         ipstat = 2
         ipend  = 3
      endif


*     fit
      do ipx=ipstat,ipend
         ipir=ipms(ipx)
         apip(ir)=ipir
C         apamom(ir)=amomip(ipx)
         call ptasmo70(ipir,aprtot(1),apamom(1),apsmom(1))
C         call sparisep(1,0,1,3)


         itel = 1
         dmove0 = 50.
         dang0 = 5.
         dang = 5.
         dpos = 50.
         call pfpang(apip(ir),apamom(ir),PFREFN,ang)
*---  special correction for e-like event ----
         if ( ipx.eq.2 ) then
            call crstnksk(appos,apdir(1,1),ZPINTK,RINTK,1,IC,IN,HPOS)
            dist=sqrt((hpos(1)-appos(1))**2+(hpos(2)-appos(2))**2+
     &           (hpos(3)-appos(3))**2)
            dcor = 0.02207*dist - 33.1
            angcor = (dist-dcor)/
     &       sqrt((dist-dcor)**2+(dist*tan(ang*3.141592/180.))**2)
            angcor = acos(angcor)*180./3.141592
            print *,'opening angle correction ',ang,' --> ',angcor
C            ang = ang+(angcor-ang)*0.589
            ang = angcor
         endif




*     calc scatter light
         call spthini
         call pfperini

C         do while (  dang.gt.TOLANG
C     $              .and. itel.le.MAXITEL )
C         do istep = 1,4

            call ucopy(appos,posold,3)
            call ucopy(apdir(1,ir),dirold,3)
C
C            call crstnksk(appos,apdir(1,1),ZPINTK,RINTK,1,IC,IN,HPOS)
C            dist=sqrt((hpos(1)-appos(1))**2+(hpos(2)-appos(2))**2+
C     $           (hpos(3)-appos(3))**2)


C*         position fit perpendicular to particle direction
C            apangcer(ir) = ang
C            maxstep = 2
CC            dmove0 = dist*dang*3.141592/180.
C            dmove0 = dmove0 + dist*dang*3.141592/180./5.
C            if (dmove0.gt.100.) then
C               dmove0=100.
C            else if (dmove0.lt.TOLPOS*2.) then
C               dmove0=TOLPOS*2.
C            endif
C            print *,'dmove0 ',dmove0,'  dpos ',dpos,'  dang  ',dang,
C     $              'dist   ',dist
C
CC            print *,' CALL PF3DMSTFIT(',dmove0,',',maxstep,')'
CC            call pf3dmstfit(dmove0,maxstep)
C
C            print *,' CALL PFPERMSTFIT(',dmove0,',',maxstep,')'
C            call pfpermstfit(dmove0,maxstep)


            CALL PFTIME0(T0)
*           select use PMT by T0
            call spthini
            thmax = 1.5 * apangcer(ir)
            thmin = 0.7 * apangcer(ir)
            call vzero(npmt,2)
            do isq=1,MAXPM
               if ( apthop(isq,ir).le.thmax ) then
                  if ( ipx.eq.3 .and. apthop(isq,ir).lt.thmin ) then
                     pfmask(isq) = 0
                  else IF( IBAD(ISQ).EQ.1) then
                     pfmask(isq) = 0
                  else IF ( (APRESID(ISQ)-T0) .lt. -30.) then
                     pfmask(isq) = 0
                  else IF ( APSIGMA(ISQ) .GT. 0.1  .AND.
     &                 (APRESID(ISQ)-T0) .GT.
     $                 (AMAX1(APSIGMA(ISQ),3.)*2.+5.)) THEN
                     pfmask(isq) = 2
                     npmt(2) = npmt(2) + 1
                  else
                     pfmask(isq) = 1
                     npmt(1) = npmt(1) + 1
                  endif
               else
                  pfmask(isq) = 0
               endif
            enddo
            print *,'pftime0 : t0= ',t0,
     $        '  Scatter light : Direct light ',npmt(2),' : ',npmt(1)


C*          position fit along particle direction
C            print *,' CALL PFDXMSTFIT(',dmove0,',',maxstep,')'
C            call pfdxmstfit(dmove0,maxstep)
C
C            itel = itel + 1
C            dpos = sqrt((appos(1)-posold(1))**2+(appos(2)-posold(2))**2
C     $                 +(appos(3)-posold(3))**2)
C            dmove0 = dmove0/2.**maxstep



*          direction fit
             maxstep = 4
             dang0 = 10.

             print *,' CALL PFDIRMSTFIT(',dang0,',',maxstep,')'
             print *,ir
             call pfdirmstfit(ir,dang0,maxstep)
             print *,'   ip amom angle ',
     $            apip(ir),apamom(ir),apangcer(ir)

            call skopenr(apdir(1,ir),dirold,dang)
            dang = dang*180./3.141592
            dang0 = dang0/2.**maxstep

            print *,'PFDOFIT : ITEL DPOS DANG ',itel,dpos,dang

C         enddo

         do k=1,3
            pfpos(k,ipx)=appos(k)
            pfdir(k,ir,ipx)=apdir(k,ir)
            appos(k)=postmp(k)
            apdir(k,ir)=dirtmp(k,ir)
         enddo
         pfangcer(ir,ipx)=apangcer(ir)
         apangcer(ir)=angtmp(ir)

      enddo

      return
      end