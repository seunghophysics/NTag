#ifndef DATAMODEL_HH
#define DATAMODEL_HH

#include "PMTHitCluster.hh"
#include "EventCandidates.hh"
#include "EventVariables.hh"
#include "EventParticles.hh"
#include "EventTrueCaptures.hh"
#include "NTagInfo.hh"

class DataModel
{
    public:
        void ReadConfig(std::string configFilePath) { ntagInfo.Initialize(configFilePath); }

        // Hits
        PMTHitCluster eventPMTHits;
        // Candidates
        EventCandidates eventCandidates;
        // Event vars
        EventVariables eventVariables;

        // Primaries (MC)
        EventParticles eventPrimaries;
        // Secondaries (MC)
        EventParticles eventSecondaries;
        // Neutron captures (MC)
        EventTrueCaptures eventTrueCaptures;

        // NTag vars (versioning, ntag options, etc)
        NTagInfo ntagInfo;
};

#endif