#include "sktqC.h"

#include "EventNTagManager.hh"

EventNTagManager::EventNTagManager(Verbosity verbose) 
{
    fMsg = Printer("NTagManager", verbose);
}

EventNTagManager::~EventNTagManager() {}

void EventNTagManager::ReadHits()
{
    fEventHits = PMTHitCluster(sktqz_);
}

void EventNTagManager::ReadEventFromCommon()
{
    // read variables
    // read mc info
    ReadHits();

}