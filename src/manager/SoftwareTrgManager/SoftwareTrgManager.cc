#include <TROOT.h>
#include <TF1.h>
#include <TH1F.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TTree.h>
#include <TFitResult.h>
#include <TFitResultPtr.h>
#include <TChain.h>
#include <TRandom3.h>

// Software trigger include
#include <skheadC.h>
#include <skparmC.h>
#include <skruninfC.h>
#include <skonl/softtrg_tbl.h>
#undef MAXPM
#undef MAXPMA
#include <tqrealroot.h>
#include <sktqC.h>
#include <softtrg_cond.h>

#include <Calculator.hh>
#include <SKIO.hh>
#include "SKLibs.hh"
#include "SoftwareTrgManager.hh"

const float SoftwareTrgManager::PC2PE_MC = 2.465;
const float SoftwareTrgManager::CNT2PC_S = 0.100;
const float SoftwareTrgManager::CNT2PC_M = 0.732;
const float SoftwareTrgManager::CNT2PC_L = 5.141;

const int SoftwareTrgManager::QBEE_QTC_SMALL  = 0;
const int SoftwareTrgManager::QBEE_QTC_MEDIUM = 1;
const int SoftwareTrgManager::QBEE_QTC_LARGE  = 2;
const int SoftwareTrgManager::IQ_INGATE_FLAG  = 2048;

SoftwareTrgManager::SoftwareTrgManager(int refRunNo=0)
{
  int softtrg_detector[32];
  int softtrg_thr[32];
  int softtrg_t0_offset[32];
  int softtrg_pre_t0[32];
  int softtrg_post_t0[32];
  softtrg_get_cond_(softtrg_detector, softtrg_thr, softtrg_t0_offset,softtrg_pre_t0, softtrg_post_t0);
  if (refRunNo)
    get_run_softtrg_(&refRunNo, softtrg_detector, softtrg_thr, softtrg_t0_offset, softtrg_pre_t0, softtrg_post_t0);
  softtrg_set_cond_(softtrg_detector,softtrg_thr,softtrg_t0_offset,softtrg_pre_t0,softtrg_post_t0);
}

void SoftwareTrgManager::ApplyTrigger(PMTHitCluster* signalHits)
{
  // Make tqrawinfo_ struct
  skruninf_.softtrg_mask = -1;
  fRawGate = 0;
  int currentHitID = 0;
  float minGate = -5.e3; 
  float maxGate = 35.0e3;
  float tmpTOffset = (1024.*32./COUNT_PER_NSEC);
  for (auto itr = signalHits->begin(); itr != signalHits->end(); ++itr) {
    if (itr->t() > minGate && itr->t() < maxGate) {
      PMTHit hit = signalHits->At(currentHitID);
      this->MakeTQRAW(hit.i(), hit.t(), hit.q(), tmpTOffset);
    }
    currentHitID++;
  }

  int iRunSK = 999999; // Not sure
  int iFirstHWCtr = 0; // Not sure
  int iInGateOnly = 1;
  int iMaxQBeeTBL = 1280;

  int iCandidates = softtrg_inittrgtbl_(&iRunSK, &iFirstHWCtr, &iInGateOnly, &iMaxQBeeTBL);
  std::cout <<" N cand: "<<iCandidates<<std::endl;
  int iPrimaryTrigger = this->FindMainTrigger(iCandidates);
  int it0sk = swtrgtbl_.swtrgt0ctr[iPrimaryTrigger];
  int idtgsk = swtrgtbl_.swtrgtype[iPrimaryTrigger];
  int iGateStart = -1000 + it0sk;
  int iGateEnd   =  1496 + it0sk;
  std::cout <<"main trigger: "<<"\n"
            <<"\t TRGID: "<<idtgsk<<", t0: "<<it0sk<<", min: "<<iGateStart<<", max: "<<iGateEnd<<std::endl;

  // Turn on the flag for 1.3 us around T0
  for (auto& hit: *signalHits) {
    hit.SetFlag(hit.f() & 0xFFFE);
    uint64_t iT_64 = (uint64_t)((hit.t() + tmpTOffset)*COUNT_PER_NSEC);
    int iT = (int)(iT_64);
    if (iT > iGateStart && iT < iGateEnd) {
      hit.SetFlag(hit.f() | 1);
    }    
    int iT_diff = -it0sk;
    hit.SetT(hit.t() + (float)(iT_diff/COUNT_PER_NSEC) + tmpTOffset + 1000.);
  }
}

void SoftwareTrgManager::MakeTQRAW(int pmtID, float t, float q, float tOffset)
{
  // T digitization
  //uint64_t iT_64 = (uint64_t)(t*COUNT_PER_NSEC);
  uint64_t iT_64 = (uint64_t)((t + tOffset)*COUNT_PER_NSEC);
  //int iBlock = (int)(iT_64 >> 15); // original is uint, but int is OK for this simulation
  //iBlock = (int)(iT_64 >> 15); // original is uint, but int is OK for this simulation
  //int iT_low = (int)(iT_64 & 0x7fff);
  //int iT_high = (int)(iT_64>>15);
  int iT = int(iT_64); 


  // Q digitization.
  // This is a very simple approximation.
  // More precise calibration results should be put here.  
  int iQsmall  = int( q * PC2PE_MC / CNT2PC_S ) + 961;
  int iQmedium = int( q * PC2PE_MC / CNT2PC_M ) + 961;
  int iQlarge  = int( q * PC2PE_MC / CNT2PC_L ) + 961;

    int iGainFlag = QBEE_QTC_SMALL;
  int iQtmp = iQsmall;

  if ( iQsmall >= 1350  ) {
    // Small range saturation -> Medium
    iGainFlag = QBEE_QTC_MEDIUM;
    iQtmp     = iQmedium;
  }
  if ( iQmedium >= 2047 ) {
    // Medium range saturation -> Large
    iGainFlag = QBEE_QTC_LARGE;
    iQtmp     = iQlarge;
  }
  int iQ = iQtmp + iGainFlag*pow(2,14) + IQ_INGATE_FLAG;

  // cable 
  int iW = ( (iQ >>11) & 0x3E ); // seems quite complex for not so much
  int cable =0; 
  cable |= ( ( pmtID & 0xFFFF )       ); // Lower 16 bin is cable Id
  cable |= ( ( iW    & 0xFFFF ) << 16 ); // Upper 16 bin is ??

  rawtqinfo_.itiskz_raw[fRawGate] = iT;
  rawtqinfo_.iqiskz_raw[fRawGate] = iQ;
  rawtqinfo_.icabbf_raw[fRawGate] = cable;

  // Useless for software trigger
  rawtqinfo_.tbuf_raw  [fRawGate] = 0;
  rawtqinfo_.qbuf_raw  [fRawGate] = 0;

  fRawGate += 1;
  rawtqinfo_.nqisk_raw = fRawGate;
}

int SoftwareTrgManager::FindMainTrigger(int numTriggers)
{
  bool isT0Found = false;
	int iPrimaryTrigger = -1;
  int it0sk_tmp = 0;
  int idtgsk_tmp = 0;
  for ( int iTrig = 0; iTrig < numTriggers; iTrig++ ) {
    std::cout <<"\t"<< iTrig << " " 
	       << swtrgtbl_.swtrgtype[iTrig] << " " 
	       << (1 << swtrgtbl_.swtrgtype[iTrig]) << " " 
	       << swtrgtbl_.swtrgidx[iTrig] << " " 
	       << swtrgtbl_.swtrgt0hwctr[iTrig] << " " 
	       << swtrgtbl_.swtrgt0loc[iTrig] << " " 
	       << swtrgtbl_.swtrgt0ctr[iTrig] << " " 
	       << swtrgtbl_.swtrggsctr[iTrig] << std::endl;
    if ( (swtrgtbl_.swtrgtype[iTrig] == TRGID_SW_LE && skruninf_.softtrg_mask&(1<<TRGID_SW_LE)) ||
         (swtrgtbl_.swtrgtype[iTrig] == TRGID_SW_HE && skruninf_.softtrg_mask&(1<<TRGID_SW_HE)) ||
         (swtrgtbl_.swtrgtype[iTrig] == TRGID_SHE   && skruninf_.softtrg_mask&(1<<TRGID_SHE))   ||
         (swtrgtbl_.swtrgtype[iTrig] == TRGID_SW_OD && skruninf_.softtrg_mask&(1<<TRGID_SW_OD))) {

			if ( isT0Found == false ) {
				//If this is the first trigger, then store and set flag
				isT0Found       = true;
				iPrimaryTrigger = iTrig;

				it0sk_tmp  = swtrgtbl_.swtrgt0ctr[iTrig];
				idtgsk_tmp = (1 << swtrgtbl_.swtrgtype[iTrig]);
			}
			else if ( swtrgtbl_.swtrgt0ctr[iTrig] <= it0sk_tmp ) {
				// Trigger time is earlier, change primary trigger
				iPrimaryTrigger = iTrig;

				it0sk_tmp  = swtrgtbl_.swtrgt0ctr[iTrig];
				idtgsk_tmp = (1 << swtrgtbl_.swtrgtype[iTrig]);
			}
		}
	}

	for ( int iTrig = 0; iTrig < numTriggers; iTrig++ ) {
		if (  (swtrgtbl_.swtrgtype[iTrig] == TRGID_SW_LE  && skruninf_.softtrg_mask&(1<< TRGID_SW_LE))  || 
				  (swtrgtbl_.swtrgtype[iTrig] == TRGID_SW_HE  && skruninf_.softtrg_mask&(1<< TRGID_SW_HE))  ||
				  (swtrgtbl_.swtrgtype[iTrig] == TRGID_SW_SLE && skruninf_.softtrg_mask&(1<< TRGID_SW_SLE)) ||
				  (swtrgtbl_.swtrgtype[iTrig] == TRGID_SHE    && skruninf_.softtrg_mask&(1<< TRGID_SHE))    ||
				  (swtrgtbl_.swtrgtype[iTrig] == TRGID_SW_OD  && skruninf_.softtrg_mask&(1<< TRGID_SW_OD)) ) {
			if ( isT0Found == false ) {
				//If this is the first trigger, then store and set flag
				isT0Found       = true;
				iPrimaryTrigger = iTrig;

				it0sk_tmp  = swtrgtbl_.swtrgt0ctr[iTrig];
				idtgsk_tmp = (1 << swtrgtbl_.swtrgtype[iTrig]);
			}

			// 1.92 count/ns -> 200ns = 384 count
			if ( abs( swtrgtbl_.swtrgt0ctr[iTrig] - it0sk_tmp ) < 384 ) {
				idtgsk_tmp |= (1 << swtrgtbl_.swtrgtype[iTrig]);
			}
    }
  }

  // no LE/HE/SLE/OD trigger, t0 is set to GEANT t0 (in units of hardware clock position)	
	if ( isT0Found == 0 ) {
		it0sk_tmp   &= 0x00000000L;
		//fIT0SK   |= ( ( int(fTrgTimeOffset / SKP::TDC_UNIT_SK4)        )      ); // Lower bin is time in count
		//fIT0SK   |= ( ( (fDummyTrg+2)				& 0xFFFF ) <<15 ); // Upper 17 bin is TRG number
		idtgsk_tmp  = 0;
	}

  // overwrite trigger ID of primary trigger
  swtrgtbl_.swtrgtype[iPrimaryTrigger] = idtgsk_tmp;
  return iPrimaryTrigger;
}



void SoftwareTrgManager::FillCommon()
{
  skhead_.idtgsk =0; 
}

