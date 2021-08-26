#ifndef NCAPTURECLUSTER_HH
#define NCAPTURECLUSTER_HH

#include <memory>

#include "Cluster.hh"
#include "ParticleCluster.hh"
#include "NCapture.hh"

class NCaptureCluster : public Cluster<NCapture>
{
    public:
        NCaptureCluster();
        NCaptureCluster(ParticleCluster& particleCluster);
    
        void Sort();
        void DumpAllElements();
        
    //ClassDef(NCaptureCluster, 1)
};

#endif