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
#include <skparmC.h>
#include <skruninfC.h>
#include <skheadC.h>
#undef MAXPM
#undef MAXPMA
#include <tqrealroot.h>
#include <sktqC.h>

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
const int SoftwareTrgManager::IQ_INGATE_FLAG = 2048;

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
  fRawGate = 0;
  int currentHitID = 0;
  for (auto itr = signalHits->begin(); itr != signalHits->end(); ++itr) {
    if (1) {
      PMTHit hit = signalHits->At(currentHitID);
      this->MakeTQRAW(hit.i(), hit.t(), hit.q());
    }
    currentHitID++;
  }

  int iRunSK = 999999; // Not sure
  int iFirstHWCtr = 0; // Not sure
  int iInGateOnly = 1;
  int iMaxQBeeTBL = 1280;

  Int_t iCandidates = softtrg_inittrgtbl_(&iRunSK, &iFirstHWCtr, &iInGateOnly, &iMaxQBeeTBL);
  this->FindMainTrigger(iCandidates);

}

void SoftwareTrgManager::MakeTQRAW(int pmtID, float t, float q)
{
  // T digitization
  uint64_t iT_64 = (uint64_t)(t * COUNT_PER_NSEC);
  //int iBlock = (int)(iT_64 >> 15); // original is uint, but int is OK for this simulation
  //iBlock = (int)(iT_64 >> 15); // original is uint, but int is OK for this simulation
  int iT = (int)(iT_64 & 0x7fff);


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

void SoftwareTrgManager::FindMainTrigger(int numTriggers)
{
  //for ( int iTrig = 0; iTrig < numTriggers; iTrig++ ) {
  //  if (  (swtrgtbl_.swtrgtype[iTrig] == TRGID_SW_LE && skruninf_.softtrg_mask&(1<<TRGID_SW_LE)) ||
  //      (swtrgtbl_.swtrgtype[iTrig] == TRGID_SW_HE && skruninf_.softtrg_mask&(1<<TRGID_SW_HE)) ||
  //      (swtrgtbl_.swtrgtype[iTrig] == TRGID_SHE   && skruninf_.softtrg_mask&(1<<TRGID_SHE))   ||
  //      (swtrgtbl_.swtrgtype[iTrig] == TRGID_SW_OD && skruninf_.softtrg_mask&(1<<TRGID_SW_OD))) {
  //  }
  //}
}



void SoftwareTrgManager::FillCommon()
{
  skhead_.idtgsk =0; 
}

