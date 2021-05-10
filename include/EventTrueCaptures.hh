#ifndef EVENTTRUECAPTURES_HH
#define EVENTTRUECAPTURES_HH

#include "Cluster.hh"
#include "TrueCapture.hh"

class EventTrueCaptures : public Cluster<TrueCapture*>
{
    public:
        ~EventTrueCaptures();

        void DumpAllElements();
};

#endif