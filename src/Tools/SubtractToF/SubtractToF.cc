#include <skparmC.h>
#include <sktqC.h>

#include "SubtractToF.hh"

bool SubtractToF::Initialize()
{
    return true;
}

bool SubtractToF::Execute()
{
    TVector3 promptVertex;
    sharedData->eventVariables.Get("prompt_vertex", promptVertex);

    // ToF is automatically subtracted by setting vertex
    sharedData->eventPMTHits.SetVertex(promptVertex);
    sharedData->eventPMTHits.Sort();

    return true;
}

bool SubtractToF::Finalize()
{
    return true;
}