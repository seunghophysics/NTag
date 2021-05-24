#ifndef EVENTTRUECAPTURES_HH
#define EVENTTRUECAPTURES_HH

#include <memory>

#include "Cluster.hh"
#include "TrueCapture.hh"

class EventTrueCaptures : public Cluster<TrueCapture>
{
    public:
        void Sort();
        void DumpAllElements();
        
    ClassDef(EventTrueCaptures, 1)
};

#endif