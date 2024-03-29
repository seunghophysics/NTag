#include <skheadC.h>
#include <skparmC.h>
#include <skruninfC.h>
#include <stdint.h>
#include <skonl/softtrg_tbl.h>
#undef MAXPM
#undef MAXPMA
#include <tqrealroot.h>
#include <sktqC.h>
#include <softtrg_cond.h>

#include "Calculator.hh"
#include "SKIO.hh"
#include "SKLibs.hh"
#include "TriggerManager.hh"

const float TriggerManager::PC2PE_MC = 2.465;
const float TriggerManager::CNT2PC_S = 0.100;
const float TriggerManager::CNT2PC_M = 0.732;
const float TriggerManager::CNT2PC_L = 5.141;

const int TriggerManager::QBEE_QTC_SMALL  = 0;
const int TriggerManager::QBEE_QTC_MEDIUM = 1;
const int TriggerManager::QBEE_QTC_LARGE  = 2;
const int TriggerManager::IQ_INGATE_FLAG  = 2048;

const int TriggerManager::SWTRG_SAME_GATE_WIDTH = 768;

TriggerManager::TriggerManager(int refRunNo=0)
{
    int softtrg_detector[32];
    int softtrg_thr[32];
    int softtrg_t0_offset[32];
    int softtrg_pre_t0[32];
    int softtrg_post_t0[32];
    softtrg_get_cond_(softtrg_detector, softtrg_thr, softtrg_t0_offset,softtrg_pre_t0, softtrg_post_t0);
    if (refRunNo)
        get_run_softtrg_(&refRunNo, softtrg_detector, softtrg_thr, softtrg_t0_offset, softtrg_pre_t0, softtrg_post_t0);
    softtrg_set_cond_(softtrg_detector, softtrg_thr, softtrg_t0_offset, softtrg_pre_t0, softtrg_post_t0);
}

void TriggerManager::ApplyTrigger(PMTHitCluster& hits)
{
    // initialize it0sk;
    fIT0SK  = skheadqb_.it0sk;
    fIDTGSK = skhead_.idtgsk;

    // Make tqrawinfo_ struct
    skruninf_.softtrg_mask = -1;
    fRawGate = 0;
    fRawGateOD = 0;
    //int currentHitID = 0;
    float minGate = -5.e3;
    float maxGate = 35.0e3;
    float tmpTOffset = (1024.*32./COUNT_PER_NSEC);

    for (auto const& hit: hits) {
        if (minGate < hit.t() && hit.t() < maxGate) {
            MakeTQRAW(hit.i(), hit.t(), hit.q(), tmpTOffset);
        }
    }

    int iRunSK = 999999; // Not sure
    int iFirstHWCtr = 0; // Not sure
    int iInGateOnly = 0;
    int iMaxQBeeTBL = 1280;

    // Apply software trigger
    int iCandidates = softtrg_inittrgtbl_(&iRunSK, &iFirstHWCtr, &iInGateOnly, &iMaxQBeeTBL);
    //std::cout <<" N cand: "<<iCandidates<<std::endl;
    int iPrimaryTrigger = FindMainTrigger(iCandidates, tmpTOffset);
    fIT0SK  = swtrgtbl_.swtrgt0ctr[iPrimaryTrigger];
    fIDTGSK = swtrgtbl_.swtrgtype[iPrimaryTrigger];

    int iGateStart = -1000 + fIT0SK;
    int iGateEnd   =  1496 + fIT0SK;
    //std::cout <<"main trigger: "<<"\n"
    //          <<"\t TRGID: "<<idtgsk<<", t0: "<<it0sk<<", min: "<<iGateStart<<", max: "<<iGateEnd<<std::endl;

    // Turn on the flag for 1.3 us around T0
    for (auto& hit: hits) {
        hit.SetFlag(hit.f() & 0xFFFE);
        uint64_t iT_64 = (uint64_t)((hit.t() + tmpTOffset)*COUNT_PER_NSEC);
        int iT = (int)(iT_64);
        if (iT > iGateStart && iT < iGateEnd) {
            hit.SetFlag(hit.f() | 1);
        }
        int iT_diff = -fIT0SK;
        hit.SetT(hit.t() + (float)(iT_diff/COUNT_PER_NSEC) + tmpTOffset + 1000.);
    }
}

void TriggerManager::MakeTQRAW(int pmtID, float t, float q, float tOffset)
{
    // T digitization
    uint64_t iT_64 = (uint64_t)((t + tOffset)*COUNT_PER_NSEC);
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
    int cable = 0;
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

int TriggerManager::FindMainTrigger(int numTriggers, float tOffset)
{
    bool isT0Found = false;
    int iPrimaryTrigger = -1;
    int it0sk_tmp = 0;
    int idtgsk_tmp = 0;
    for ( int iTrig = 0; iTrig < numTriggers; iTrig++ ) {
        if ( (swtrgtbl_.swtrgtype[iTrig] == TRGID_SW_LE && skruninf_.softtrg_mask&(1<<TRGID_SW_LE)) ||
             (swtrgtbl_.swtrgtype[iTrig] == TRGID_SW_HE && skruninf_.softtrg_mask&(1<<TRGID_SW_HE)) ||
             (swtrgtbl_.swtrgtype[iTrig] == TRGID_SHE   && skruninf_.softtrg_mask&(1<<TRGID_SHE))   ||
             (swtrgtbl_.swtrgtype[iTrig] == TRGID_SW_OD && skruninf_.softtrg_mask&(1<<TRGID_SW_OD)) ) {

            if ( isT0Found == false ) {
                // if this is the first trigger, then store and set flag
                isT0Found       = true;
                iPrimaryTrigger = iTrig;

                it0sk_tmp  = swtrgtbl_.swtrgt0ctr[iTrig];
                idtgsk_tmp = (1 << swtrgtbl_.swtrgtype[iTrig]);
            }
            else if ( swtrgtbl_.swtrgt0ctr[iTrig] <= it0sk_tmp ) {
                // trigger time is earlier, change primary trigger
                iPrimaryTrigger = iTrig;

                it0sk_tmp  = swtrgtbl_.swtrgt0ctr[iTrig];
                idtgsk_tmp = (1 << swtrgtbl_.swtrgtype[iTrig]);
            }
        }
    }

    for ( int iTrig = 0; iTrig < numTriggers; iTrig++ ) {
        if ( (swtrgtbl_.swtrgtype[iTrig] == TRGID_SW_LE  && skruninf_.softtrg_mask&(1<< TRGID_SW_LE))  ||
             (swtrgtbl_.swtrgtype[iTrig] == TRGID_SW_HE  && skruninf_.softtrg_mask&(1<< TRGID_SW_HE))  ||
             (swtrgtbl_.swtrgtype[iTrig] == TRGID_SW_SLE && skruninf_.softtrg_mask&(1<< TRGID_SW_SLE)) ||
             (swtrgtbl_.swtrgtype[iTrig] == TRGID_SHE    && skruninf_.softtrg_mask&(1<< TRGID_SHE))    ||
             (swtrgtbl_.swtrgtype[iTrig] == TRGID_SW_OD  && skruninf_.softtrg_mask&(1<< TRGID_SW_OD)) ) {
            if ( isT0Found == false ) {
                // if this is the first trigger, then store and set flag
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

    // if no trigger, t0 is set to GEANT t0 (in units of hardware clock position)
    if ( isT0Found == 0 ) {
        it0sk_tmp   &= 0x00000000L;
        idtgsk_tmp  = 0;
    }

    // Record MC trigger info.
    fSubTrigger_Type.clear();
    fSubTrigger_Time.clear();
    fSubTrigger_TimeRel.clear();
    fSubTrigger_Index.clear();

    // Primary trigger
    int iTriggerBit = 0;
    if (iPrimaryTrigger != -1) {
        iTriggerBit = (1 << swtrgtbl_.swtrgtype[iPrimaryTrigger]);
        fSubTrigger_Type.push_back(iTriggerBit);
    }
    else {
        fSubTrigger_Type.push_back(iTriggerBit);
    }
    fSubTrigger_Time.push_back(it0sk_tmp);
    fSubTrigger_TimeRel.push_back(it0sk_tmp/COUNT_PER_NSEC - tOffset);
    fSubTrigger_Index.push_back(iPrimaryTrigger);

    // Loop for overlap triggers (mimics output of data)
    if ( iPrimaryTrigger != -1 ) {
        for ( int iTrig = 0; iTrig < numTriggers; iTrig++ ) {
        // loop over triggers again to get overlap triggers

            if (  iTrig != iPrimaryTrigger
                && abs( it0sk_tmp - swtrgtbl_.swtrgt0ctr[iTrig] ) < SWTRG_SAME_GATE_WIDTH ) {

                iTriggerBit = (1 << swtrgtbl_.swtrgtype[iTrig]); // Trigger bit
                idtgsk_tmp |= iTriggerBit; // Add bit if needed
            }
        }
    }

    // Store each trigger, up to max
    for ( int iTrig = 0; iTrig < numTriggers; iTrig++ ) {
        iTriggerBit = (1 << swtrgtbl_.swtrgtype[iTrig]);

        fSubTrigger_Type   .push_back(iTriggerBit);
        fSubTrigger_Time   .push_back(swtrgtbl_.swtrgt0ctr[iTrig]);
        fSubTrigger_TimeRel.push_back( swtrgtbl_.swtrgt0ctr[iTrig]/COUNT_PER_NSEC - tOffset );
        // ns from event trigger to geant t0 (should be negative)
        fSubTrigger_Index  .push_back( iTrig );
    }


    // overwrite trigger ID of primary trigger
    swtrgtbl_.swtrgtype[iPrimaryTrigger] = idtgsk_tmp;
    return iPrimaryTrigger;
}

void TriggerManager::FillCommon()
{
    skheadqb_.it0sk = fIT0SK;
    skheadqb_.it0xsk = fIT0SK;
    skhead_.idtgsk = fIDTGSK;

    //Many other items in skhead are NOT implemented yet
}

void TriggerManager::FillTrgOffset(MCInfo& inputMCINFO)
{
    for (int i=0; i<10; i++) {
        inputMCINFO.trigbit[i]    = fSubTrigger_Type[i];
        inputMCINFO.it0sk_temp[i] = fSubTrigger_Time[i];
        inputMCINFO.prim_pret0[i] = fSubTrigger_TimeRel[i] - 1000. + 500./COUNT_PER_NSEC;;
        inputMCINFO.prim_trg[i]   = fSubTrigger_Index[i];
    }
}