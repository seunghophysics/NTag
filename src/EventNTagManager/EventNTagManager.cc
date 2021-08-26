#include "skparmC.h"
#include "sktqC.h"
#include "skheadC.h"
#include "apmringC.h"

#include "SKLibs.hh"
#include "EventNTagManager.hh"

EventNTagManager::EventNTagManager(Verbosity verbose) 
{
    fMsg = Printer("NTagManager", verbose);
    
    fSettings.Initialize("/disk02/usr6/han/phd/utillib/src/EventNTagManager/NTagConfig");
}

EventNTagManager::~EventNTagManager() {}

void EventNTagManager::ReadVariables()
{
    // basic information
    fEventVariables.Set("RunNo", skhead_.nrunsk);
    fEventVariables.Set("SubrunNo", skhead_.nsubsk);
    fEventVariables.Set("EventNo", skhead_.nevsk);
    
    // hit information
    int nhitac; odpc_2nd_s_(&nhitac);
    int trgtype = skhead_.idtgsk & 1<<28 ? tSHE : skhead_.idtgsk & 1<<29 ? tAFT : tELSE; 
    fEventVariables.Set("QISMSK", skhead_.nrunsk);
    fEventVariables.Set("NHITAC", nhitac);
    fEventVariables.Set("TrgType", trgtype);
        
    // reconstructed information
    int bank = 0; aprstbnk_(&bank);
        // evis
        fEventVariables.Set("EVis", apcomene_.apevis);
        // prompt vertex
        fEventVariables.Set("pvx", apcommul_.appos[0]);
        fEventVariables.Set("pvy", apcommul_.appos[1]);
        fEventVariables.Set("pvz", apcommul_.appos[2]);
        // dwall
        fEventVariables.Set("DWall", wallsk_(apcommul_.appos));
        // ring
            // nring
            fEventVariables.Set("NRing", apcommul_.apnring);
            // most energetic ring pid
            // most energetic ring mom
        //// muechk
        //    // nmue
        //    // etime
        //    // epos
            
    // mc information
        // ipnu
        // pnu
}

void EventNTagManager::ReadHits()
{
    fEventHits = PMTHitCluster(sktqz_);
}

void EventNTagManager::ReadParticles()
{
    skgetv_(); apflscndprt_();
    fEventParticles = ParticleCluster(skvect_, secndprt_);
    
    float geantT0; trginfo_(&geantT0);
    fEventParticles.SetT0(geantT0);
}

void EventNTagManager::ReadNCaptures()
{
    if (fEventParticles.IsEmpty()) {
        ReadParticles();
    }
    else
        fEventNCaptures = NCaptureCluster(fEventParticles);
}

void EventNTagManager::ReadEventFromCommon()
{
    ClearData();

    ReadVariables();
    ReadHits();
    
    // if MC
    ReadParticles();
    ReadNCaptures();
}

void EventNTagManager::ClearData()
{
    fEventVariables.Clear();
    fEventHits.Clear();
    fEventParticles.Clear();
    fEventNCaptures.Clear();
}